#include "MPEG4EncoderFFMPEG.h"

#include "FilePathManager.h"


extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"
#include "libswscale/swscale.h"
}


IMPLEMENT_CLASS_INFO(MPEG4EncoderFFMPEG);

IMPLEMENT_CONSTRUCTOR(MPEG4EncoderFFMPEG)
{
}

MPEG4EncoderFFMPEG::~MPEG4EncoderFFMPEG(){}

void MPEG4EncoderFFMPEG::InitModifiable()
{
	CoreModifiable::InitModifiable();

	if (IsInit())
	{
	}
}

AVPixelFormat getAVFormat(unsigned int format)
{
	switch (format)
	{
	case 0:		// rgb24
		return AV_PIX_FMT_RGB24;
	case 1:		// rgba32
		return AV_PIX_FMT_RGBA;
	case 2:		// yuv420p
		return AV_PIX_FMT_YUV420P;
	default:
		return AV_PIX_FMT_NONE;
	}
}

/* add a video output stream */
static AVStream * add_video_stream(EncoderInfo& EI, int width, int height)
{
	AVCodecContext *c;
	AVCodec *codec;
	AVStream *st;

	st = avformat_new_stream(EI.oc, 0);
	if (!st) {
		fprintf(stderr, "Could not alloc stream\n");
		return 0;
	}

	st->time_base.num = 1;
	st->time_base.den = 25;

	c = st->codec;
	c->codec_id = EI.oc->oformat->video_codec;
	c->codec_type = AVMEDIA_TYPE_VIDEO;

	/* put sample parameters */
	c->bit_rate = 400000;
	c->width = width;
	c->height = height;
	c->me_range = 16;
	c->qmin = 10;
	c->qmax = 30;
	c->gop_size = 10;
	c->ticks_per_frame = 2;
	c->max_b_frames = 1;
	c->pix_fmt = (AVPixelFormat)EI.PixFormat;

	// some formats want stream headers to be separate
	if (EI.oc->oformat->flags & AVFMT_GLOBALHEADER)
		c->flags |= CODEC_FLAG_GLOBAL_HEADER;
	
	/* find the video encoder */
	codec = avcodec_find_encoder(EI.oc->oformat->video_codec);
	if (!codec) {
		fprintf(stderr, "Codec not found\n");
		return 0;
	}

	/* open it */
	if (avcodec_open2(c, codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		return 0;
	}

	return st;
}

/*
* Video encoding example
*/
bool MPEG4EncoderFFMPEG::InitEncoder(const char * filename, bool flipInput)
{
	mLastFramePts = -1;

	//AVFormatContext *oc;
	AVOutputFormat *fmt;
	AVStream *audio_st = NULL, *vStream = NULL;
	
	printf("Encode video file %s\n", filename);

	/* auto detect the output format from the name. default is
	mpeg. */
	fmt = av_guess_format(NULL, filename, NULL);
	if (!fmt) {
		fprintf(stderr, "Could not find suitable output format\n");
		return false;
	}

	/* allocate the output media context */
	mEncoderInfo.oc = avformat_alloc_context();
	if (!mEncoderInfo.oc) {
		fprintf(stderr, "Memory error : could not allocate the output media context\n");
		return false;
	}

	/* set the media context used format */
	mEncoderInfo.oc->oformat = fmt;

	/* add the audio and video streams using the default format codecs and initialize the codecs */
	if (fmt->video_codec != AV_CODEC_ID_NONE)
		vStream = add_video_stream(mEncoderInfo, mEncoderInfo.width, mEncoderInfo.height);

	// no audio stream for now
	fmt->audio_codec = AV_CODEC_ID_NONE;

	// allocate the video inframe 
	mEncoderInfo.inframe = av_frame_alloc();
	if (!mEncoderInfo.inframe) {
		fprintf(stderr, "Could not allocate video inframe \n");
		return false;
	}

	// set inframe  parameters
	mEncoderInfo.inframe->pts = 0;
	mEncoderInfo.inframe->format = getAVFormat(mSourceFormat);
	mEncoderInfo.inframe->width = mSourceWidth;
	mEncoderInfo.inframe->height = mSourceHeight;

	// allocate the video inframe  data
	int ret = av_image_alloc(mEncoderInfo.inframe->data, mEncoderInfo.inframe->linesize, mSourceWidth, mSourceHeight, getAVFormat(mSourceFormat), 32);
	if (ret < 0) {
		fprintf(stderr, "Could not allocate raw picture buffer\n");
		return false;
	}

	mEncoderInfo.FlippedInput= flipInput;
	mEncoderInfo.InputTarget = mEncoderInfo.inframe->data[0];

	if (flipInput)
	{
		for (int i = 0; i < AV_NUM_DATA_POINTERS; i++) 
		{
			if (mEncoderInfo.inframe->data[i] == nullptr)
				break;

			if (i) 
				mEncoderInfo.inframe->data[i] += mEncoderInfo.inframe->linesize[i] * ((mEncoderInfo.inframe->height >> 1) - 1);
			else 
				mEncoderInfo.inframe->data[i] += mEncoderInfo.inframe->linesize[i] * (mEncoderInfo.inframe->height - 1);
			mEncoderInfo.inframe->linesize[i] = -mEncoderInfo.inframe->linesize[i];
		}
	}


	mIsScaleNeeded = (flipInput||(mSourceWidth != mTargetWidth) || (mSourceHeight != mTargetHeight) || (mSourceFormat != mTargetFormat)) ? true : false;
	if (mIsScaleNeeded)
	{
		// create the scale context
		mEncoderInfo.Sws = sws_getContext(mSourceWidth, mSourceHeight, getAVFormat(mSourceFormat), mTargetWidth, mTargetHeight, getAVFormat(mTargetFormat), SWS_BICUBIC, NULL, NULL, NULL);

		// allocate the video outframe 
		mEncoderInfo.outframe = av_frame_alloc();
		if (!mEncoderInfo.outframe) {
			fprintf(stderr, "Could not allocate video outframe \n");
			return false;
		}

		// set outframe  parameters
		mEncoderInfo.outframe->pts = 0;
		mEncoderInfo.outframe->format = getAVFormat(mTargetFormat);
		mEncoderInfo.outframe->width = mTargetWidth;
		mEncoderInfo.outframe->height = mTargetHeight;

		// allocate the video outframe  data
		int ret = av_image_alloc(mEncoderInfo.outframe->data, mEncoderInfo.outframe->linesize, mTargetWidth, mTargetHeight, getAVFormat(mTargetFormat), 32);
		if (ret < 0) {
			fprintf(stderr, "Could not allocate raw picture buffer\n");
			return false;
		}

	}
	else
		mEncoderInfo.outframe = mEncoderInfo.inframe;







	// dump codec info
	av_dump_format(mEncoderInfo.oc, 0, filename, 1);

	// open the output file
	if (!(fmt->flags & AVFMT_NOFILE)) {
		if (avio_open(&mEncoderInfo.oc->pb, filename, AVIO_FLAG_WRITE) < 0) {
			KIGS_ERROR("Could not open mp4 file output\n", 0);
		}
	}

	/* write the stream header, if any */
	avformat_write_header(mEncoderInfo.oc, NULL);

	mEncoderInfo.pkt = new AVPacket();
	return true;
}

void MPEG4EncoderFFMPEG::EncodeFrame(double Time, void* data, int dataLen, int stride)
{
	AVStream * vStream = mEncoderInfo.oc->streams[0];

	// init packet
	av_init_packet(mEncoderInfo.pkt);

	/*if (needFlip)
	{
		int lineSize = dataLen / mEncoderInfo.inframe->height;
		for (int h = 1; h < mEncoderInfo.inframe->height; h++)
		{
			unsigned char* buffer = (unsigned char*)data;
			memcpy(&mEncoderInfo.inframe->data[0][dataLen-(h*lineSize)], buffer +(h*lineSize), lineSize);
		}


	}
	else*/
	{
		// copy data in inframe
		memcpy(mEncoderInfo.InputTarget, data, dataLen);
	}

	// do rescale if needed
	if (mIsScaleNeeded)
		sws_scale(mEncoderInfo.Sws, mEncoderInfo.inframe->data, mEncoderInfo.inframe->linesize, 0, mEncoderInfo.inframe->height, mEncoderInfo.outframe->data, mEncoderInfo.outframe->linesize);

	mEncoderInfo.outframe->pts = av_rescale_q((int64_t)(Time*AV_TIME_BASE), { 1, AV_TIME_BASE }, vStream->time_base);
	if (mEncoderInfo.outframe->pts <= mLastFramePts)
		return;

	mLastFramePts = mEncoderInfo.outframe->pts;

	// encode the packet
	int got_output;
	int ret = avcodec_encode_video2(vStream->codec, mEncoderInfo.pkt, mEncoderInfo.outframe, &got_output);
	if (ret < 0) {
		fprintf(stderr, "Error encoding frame\n");
		return;
	}

	if (got_output) {
		if (mEncoderInfo.pkt->pts != AV_NOPTS_VALUE)
			mEncoderInfo.pkt->pts = av_rescale_q(mEncoderInfo.pkt->pts, vStream->time_base, vStream->time_base);
		if (mEncoderInfo.pkt->dts != AV_NOPTS_VALUE)
			mEncoderInfo.pkt->dts = av_rescale_q(mEncoderInfo.pkt->dts, vStream->time_base, vStream->time_base);

		//printf("Write frame %lld (size=%d)\n", mEncoderInfo.pkt->pts, mEncoderInfo.pkt->size);
		//av_interleaved_write_frame(mEncoderInfo.oc, mEncoderInfo.pkt);
		av_write_frame(mEncoderInfo.oc, mEncoderInfo.pkt);
	}

	av_free_packet(mEncoderInfo.pkt);
}

void MPEG4EncoderFFMPEG::CloseEncoder()
{
	AVStream * vStream = mEncoderInfo.oc->streams[0];

	for (int got_output = 1; got_output;)
	{
		av_init_packet(mEncoderInfo.pkt);
		mEncoderInfo.pkt->data = NULL;
		mEncoderInfo.pkt->size = 0;
		avcodec_encode_video2(vStream->codec, mEncoderInfo.pkt, NULL, &got_output);
		if (got_output)
		{
			if (mEncoderInfo.pkt->pts != AV_NOPTS_VALUE)
				mEncoderInfo.pkt->pts = av_rescale_q(mEncoderInfo.pkt->pts, vStream->time_base, vStream->time_base);
			if (mEncoderInfo.pkt->dts != AV_NOPTS_VALUE)
				mEncoderInfo.pkt->dts = av_rescale_q(mEncoderInfo.pkt->dts, vStream->time_base, vStream->time_base);

			//printf("Write delayed %lld (size=%d)\n", mEncoderInfo.pkt->pts, mEncoderInfo.pkt->size);
			av_write_frame(mEncoderInfo.oc, mEncoderInfo.pkt);
			av_free_packet(mEncoderInfo.pkt);
		}
	}
	av_write_trailer(mEncoderInfo.oc);

	if (!(mEncoderInfo.oc->oformat->flags & AVFMT_NOFILE)) {
		/* close the output file */
		avio_close(mEncoderInfo.oc->pb);
	}


	/* free the streams */
	for (unsigned int i = 0; i < mEncoderInfo.oc->nb_streams; i++) {
		av_freep(mEncoderInfo.oc->streams[i]->codec);
		av_freep(mEncoderInfo.oc->streams[i]);
	}

	/* free the inframe */
	av_freep(&mEncoderInfo.InputTarget);
	av_frame_free(&mEncoderInfo.inframe);

	if (mIsScaleNeeded)
	{
		/* free Sws scale struct */
		sws_freeContext(mEncoderInfo.Sws);

		/* free the outframe */
		av_freep(&mEncoderInfo.outframe->data[0]);
		av_frame_free(&mEncoderInfo.outframe);
	}

	/* free the stream */
	av_free(mEncoderInfo.oc);

	delete mEncoderInfo.pkt;
}

bool MPEG4EncoderFFMPEG::StartRecording()
{
	SmartPointer<FileHandle> file = Platform_FindFullName(mFileName);
	const char * filename = file ? file->mFullFileName.c_str() : mFileName.c_str();


	Platform_CreateFolderTree(file.get());


	/* initialize libavcodec, and register all codecs and formats */
	av_register_all();


	mEncoderInfo.width = mTargetWidth;
	mEncoderInfo.height = mTargetHeight;
	mEncoderInfo.PixFormat = getAVFormat(mTargetFormat);

	if (InitEncoder(filename, mVFlip))
	{
		mIsRecording = true;
		return true;
	}

	mIsRecording = false;
	return false;


}

void MPEG4EncoderFFMPEG::StopRecording()
{
	mIsRecording = false;
	mNeedCloseEncoder = true;
}
