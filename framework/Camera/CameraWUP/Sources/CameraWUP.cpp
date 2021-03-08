#include <unknwn.h>

#include "CameraWUP.h"
#include "Core.h"
#include "CoreBaseApplication.h"
#include "Timer.h"


#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Media.MediaProperties.h>
#include <winrt/Windows.Media.Capture.h>
#include <winrt/Windows.Media.Capture.Frames.h>
#include <winrt/Windows.Media.Devices.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Storage.Streams.h>


#include "winrt_helpers.h"


using namespace winrt::Windows::Media;
using namespace winrt::Windows::Media::MediaProperties;
using namespace winrt::Windows::Media::Capture;
using namespace winrt::Windows::Media::Capture::Frames;
using namespace winrt::Windows::Graphics::Imaging;
using namespace winrt::Windows::Devices::Enumeration;


struct __declspec(uuid("5b0d3235-4dba-4d44-865e-8f1d0e4fd04d")) __declspec(novtable) IMemoryBufferByteAccess : ::IUnknown
{
	virtual long __stdcall GetBuffer(uint8_t** value, uint32_t* capacity) = 0;
};

#ifdef FFMPEG
extern "C"
{
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}
#endif

IMPLEMENT_CLASS_INFO(CameraWUP)
IMPLEMENT_CONSTRUCTOR(CameraWUP)
{
	CoreBaseApplication* currentApp = KigsCore::GetCoreApplication();
	currentApp->AddAutoUpdate(this);
}



void CameraWUP::InitModifiable()
{
	ParentClassType::InitModifiable();
	mState = isStopped;
	QueryPerformanceFrequency(&mQPCFrequency);
}

winrt::Windows::Foundation::IAsyncAction CameraWUP::InitMediaCaptureAsync()
{
	auto source_groups = co_await MediaFrameSourceGroup::FindAllAsync();

	MediaFrameSourceGroup selected_source_group = nullptr;
	MediaFrameSourceInfo selected_source_info = nullptr;

	for (auto source_group : source_groups)
	{
		kigsprintf("grp : %ls\n", source_group.DisplayName().c_str());
	}
	for (auto source_group : source_groups)
	{
		for (MediaFrameSourceInfo source_info : source_group.SourceInfos())
		{
			if (source_info.SourceKind() == MediaFrameSourceKind::Color)
			{
				selected_source_info = source_info;
				break;
			}
		}
		if (selected_source_info != nullptr)
		{
			selected_source_group = source_group;
			break;
		}
	}

	if (selected_source_group == nullptr)
	{
		co_return;
	}
	
	co_await resume_on_kigs_thread();
	mMediaCapture = MediaCapture();
	mMediaCapture.Failed([](const MediaCapture& mMediaCapture, const MediaCaptureFailedEventArgs& args)
	{
		// TODO
		//__debugbreak();
	});

	MediaCaptureInitializationSettings settings = MediaCaptureInitializationSettings();
	settings.MemoryPreference(MediaCaptureMemoryPreference::Cpu);
	settings.StreamingCaptureMode(StreamingCaptureMode::Video);
	settings.SourceGroup(selected_source_group);
	
	//co_await mMediaCapture.InitializeAsync(settings);
	co_await mMediaCapture.InitializeAsync();

	mFrameSource = mMediaCapture.FrameSources().Lookup(selected_source_info.Id());

	int count = mFrameSource.SupportedFormats().Size();

	
	winrt::hstring wanted_format = MediaEncodingSubtypes::Nv12();
	switch ((int)mFormat)
	{
	case 0: // RGB555
		co_return;
		break;
	case 1: // RGB565
		co_return;
		break;
	case 2: // YUV422
		co_return;
		break;
	case 3: // RGB24
		wanted_format = MediaEncodingSubtypes::Rgb24();
		break;
	case 4: // RGB32
		co_return;
		break;
	case 5: // YUV24
		// Can't get yuv24 as a source, so anything is fine
		break;
	}

	MediaFrameFormat preferred_format = nullptr;
	for (auto format : mFrameSource.SupportedFormats())
	{
		auto type = format.Subtype();
		auto w = format.VideoFormat().Width();
		auto h = format.VideoFormat().Height();

		if (w == mInitResX && h == mInitResY)
		{
			preferred_format = format;
			if (wanted_format == type)
				break;
		}
	}

	if (preferred_format)
	{
		co_await mFrameSource.SetFormatAsync(preferred_format);
	}

	auto final_format = mFrameSource.CurrentFormat();

	auto type = final_format.Subtype();
	auto w = final_format.VideoFormat().Width();
	auto h = final_format.VideoFormat().Height();

	mInitResX = mWidth = w;
	mInitResY = mHeight = h;
	if (type == MediaEncodingSubtypes::Nv12() || type == L"NV12")
	{
		if (mFormat == 3)
		{
			// NV12 to RGB24
			mConversionMethod = &ConvertFormat_NV12ToRGB24;
		}
		else if (mFormat == 5)
		{
			// NV12 to YUV24
			mConversionMethod = &ConvertFormat_NV12ToYUV24;
		}
		else if (mFormat == 6)
		{
			// NV12 to YUV24
			mConversionMethod = &CopyFormat_12;
		}
		else{
			// Unsupported format conversion
			co_return;
		}
	}
	else if (type == MediaEncodingSubtypes::Rgb24() || type == L"RGB24")
	{
		if (mFormat == 3)
		{
			// RGB24 to RGB24
			mConversionMethod = &CopyFormat_24;
		}
		else if (mFormat == 5)
		{
			// RGB24 to YUV24
			mConversionMethod = &ConvertFormat_RGB24ToYUV24;
		}
		else
		{
			// Unsupported format conversion
			co_return;
		}
	}
	else
	{
		// Unsupported format
		co_return;
	}

	if (mUseMRC)
	{
		auto effect = winrt::make_self<MrcVideoEffectDefinition>();
		effect->StreamType(MediaStreamType::VideoPreview);
		effect->RecordingIndicatorEnabled(false);
		//effect.GlobalOpacityCoefficient(1.0f);
		co_await mMediaCapture.AddVideoEffectAsync(*effect, MediaStreamType::VideoPreview);
	}

	mFrameReader = co_await mMediaCapture.CreateFrameReaderAsync(mFrameSource);
	mFrameReader.AcquisitionMode(MediaFrameReaderAcquisitionMode::Realtime);
	co_await resume_on_kigs_thread();

	RecomputeAllParams();
	mState = isInit;
}

winrt::Windows::Foundation::IAsyncAction CameraWUP::StartCaptureAsync()
{
	if (mState == isInInit) co_return;
	if (mState == isStopped)
	{
		mState = isInInit;
		co_await InitMediaCaptureAsync();
		if (mState != isInit)
		{
			co_return;
		}
	}
	if (mState == isRunning) co_return; // Already running
	
	MediaFrameReaderStartStatus status = co_await mFrameReader.StartAsync();
	if (status == MediaFrameReaderStartStatus::Success)
	{
		co_await resume_on_kigs_thread();
		mState = isRunning;
		mFrameReaderFrameArrivedRevokeToken = mFrameReader.FrameArrived(winrt::auto_revoke, [this](const MediaFrameReader& reader, const MediaFrameArrivedEventArgs& args)
		{
			if (MediaFrameReference frame = reader.TryAcquireLatestFrame(); frame != nullptr)
			{
				std::unique_lock lock(mMtx);
				mLatestFrame = frame;
			}
		});
	}
}

winrt::Windows::Foundation::IAsyncAction CameraWUP::StopCaptureAsync()
{
	if (mState != isRunning) co_return;
	mState = isPaused;
	co_await mFrameReader.StopAsync();
	//co_await mMediaCapture.ClearEffectsAsync(MediaStreamType::VideoPreview);
}

// crop directly in input buffer
void CameraWUP::DoCrop(u8 *pBuffer, long &BufferLen)
{
	mFrameCropMethod(pBuffer, BufferLen, mInitResX, mInitResY, mWidth, mHeight, mOffsetX, mOffsetY);
}

void CameraWUP::SetMinimumTimeFromNow(int additional_milliseconds)
{
	LARGE_INTEGER ElapsedNanoseconds;
	QueryPerformanceCounter(&mMinTime);
	mMinTime.QuadPart *= (1'000'000'000 / 100); // SystemRelativeTime from MediaReferenceFrame is in 100ns intervals
	mMinTime.QuadPart += additional_milliseconds * 1'000'000 / 100;
	mMinTime.QuadPart /= mQPCFrequency.QuadPart;
}

void CameraWUP::Update(const Timer& timer, void* addParam)
{
	ParentClassType::Update(timer, addParam);
	
	std::unique_lock lock(mMtx);

	if (mLatestFrame != nullptr)
	{
		auto frame = mLatestFrame;
		auto video_format = frame.Format().VideoFormat();

		uint8_t* value = nullptr;
		uint32_t value_size = 0;

		auto time_value = frame.SystemRelativeTime().GetTimeSpan();
		if (time_value.count() < mMinTime.QuadPart)
		{
			mLatestFrame = nullptr;
			return;
		}

		double time = time_value.count() / 1'000'000'000.0;
		if (auto buffer_media = frame.BufferMediaFrame(); buffer_media)
		{
			value = buffer_media.Buffer().data();
			value_size = buffer_media.Buffer().Length();
		}
		else if (auto video_frame = frame.VideoMediaFrame(); video_frame)
		{
			auto bitmap = video_frame.SoftwareBitmap();

			using namespace winrt;
			using namespace Windows::Foundation;

			auto bitmap_buffer = bitmap.LockBuffer(BitmapBufferAccessMode::Read);
			IMemoryBufferReference reference = bitmap_buffer.CreateReference();
			auto interop = reference.as<IMemoryBufferByteAccess>();
			interop->GetBuffer(&value, &value_size);
		}

		if (value)
		{
			BufferReceived(time, value, value_size);
		}
		mLatestFrame = nullptr;
	}
}

void CameraWUP::BufferReceived(double Time, u8 *pBuffer, long BufferLen)
{
#ifdef FFMPEG
	if (isRecording)
	{
		EncodeFrame(Time, pBuffer, BufferLen);
	}
	else if (needCloseEncoder)
	{
		needCloseEncoder = false;
		EncodeFrame(Time, pBuffer, BufferLen);
		CloseEncoder();
	}
#endif
	mRejectFrames++;
	if (mRejectFrames < 2)
	{
		return;
	}
	mFrameCounter++;

	if ((mFrameCounter % ((int)mSkippedFrameCount + 1)) != 0)
	{
		return;
	}
	mFrameCounter = 0;


	// copy buffer
	int buffertoset = GetFreeBuffer();
	if (buffertoset == -1)
	{
		buffertoset = GetReadyBufferIndex();
		if (buffertoset == -1)
			return;
	}
	SetBufferState(buffertoset, ProcessingBuffer);

	if (mConversionMethod)
	{
		mConversionMethod(pBuffer, mFrameBuffers[buffertoset]->buffer(), mWidth, mHeight);
	}
	else // bgr 555 to rgb 555 
	{
		unsigned short*	bufferOffset = ((unsigned short*)pBuffer);


		for (int i = 0; i < mHeight; i++)
		{
			unsigned short* writebuffer = (((unsigned short*)mFrameBuffers[buffertoset]->buffer()) + (mWidth*i));

			for (int j = 0; j < mWidth; j++)
			{
				unsigned short pix = bufferOffset[j];

				unsigned short R = pix & 0x7C00;
				unsigned short G = pix & 0x03E0;
				unsigned short B = pix & 0x001F;


				R = R >> 10;
				B = B << 10;
				*writebuffer++ = R | G | B | 0x8000;
			}

			// next line
			bufferOffset += mWidth;
		}
	}

	// upside down ?
	if (mIsUpsideDown) // flip image
	{
		char* invertB = mFrameBuffers[buffertoset]->buffer();

		int PixelLineSize = mWidth * mPixelSize;

		char* tmpbuffer = new char[PixelLineSize];

		char* l1, *l2;

		int i;
		for (i = 0; i < (mHeight / 2); i++)
		{
			l1 = invertB + i*PixelLineSize;
			l2 = invertB + (mHeight - i - 1)*PixelLineSize;

			// copy l1 to buffer
			memcpy(tmpbuffer, l1, (int)PixelLineSize);
			// copy l2 to l1
			memcpy(l1, l2, (int)PixelLineSize);
			// copy buffer to l2
			memcpy(l2, tmpbuffer, (int)PixelLineSize);
		}

		delete[] tmpbuffer;

	}

	SetBufferState(buffertoset, ReadyBuffer);
}

void CameraWUP::StartPreview()
{
	no_await(StartCaptureAsync());
}

void CameraWUP::StopPreview()
{
	no_await(StopCaptureAsync());
}

void CameraWUP::AllocateFrameBuffers()
{
	int buffersize = mWidth*mHeight;

	for (int i = 0; i < BUFFER_COUNT; i++)
		mFrameBuffers[i] = OwningRawPtrToSmartPtr(new AlignedCoreRawBuffer<16, unsigned char>(buffersize*mPixelSize, false));
}

void CameraWUP::FreeFrameBuffers()
{
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		mFrameBuffers[i] = nullptr;
	}
}

#ifdef FFMPEG
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

	c = st->codec;
	c->codec_id = EI.oc->oformat->video_codec;
	c->codec_type = AVMEDIA_TYPE_VIDEO;

	/* put sample parameters */
	c->bit_rate = 400000;
	/* resolution must be a multiple of two */
	//c->width = 352;
	//c->height = 288;

	c->width = width;
	c->height = height;
	/* frames per second */
	c->time_base.num = 1;
	c->time_base.den = 25;
	/* emit one intra frame every ten frames
	* check frame pict_type before passing frame
	* to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
	* then gop_size is ignored and the output of encoder
	* will always be I frame irrespective to gop_size*/

	c->me_range = 16;
	c->qmin = 10;
	c->qmax = 51;

	c->gop_size = 10;
	c->max_b_frames = 1;
	c->pix_fmt = EI.PixFormat;

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
void CameraWUP::InitEncoder(const char *filename)
{
	//AVFormatContext *oc;
	AVOutputFormat *fmt;
	AVStream *audio_st = NULL, *vStream = NULL;

	/*int i, ret, x, y, got_output;
	AVFrame *frame;
	AVPacket pkt;*/

	printf("Encode video file %s\n", filename);

	/* auto detect the output format from the name. default is
	mpeg. */
	fmt = av_guess_format(NULL, filename, NULL);
	if (!fmt) {
		fprintf(stderr, "Could not find suitable output format\n");
		return;
	}

	/* allocate the output media context */
	mEI.oc = avformat_alloc_context();
	if (!mEI.oc) {
		fprintf(stderr, "Memory error : could not allocate the output media context\n");
		return;
	}

	/* set the media context used format */
	mEI.oc->oformat = fmt;

	/* add the audio and video streams using the default format codecs and initialize the codecs */
	if (fmt->video_codec != CODEC_ID_NONE)
		vStream = add_video_stream(mEI, mEI.width, mEI.height);

	// no audio stream for now
	fmt->audio_codec = CODEC_ID_NONE;

	// allocate the video frame
	mEI.frame = av_frame_alloc();
	if (!mEI.frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		return;
	}

	// set frame parameters
	mEI.frame->pts = 0;
	mEI.frame->format = vStream->codec->pix_fmt;
	mEI.frame->width = vStream->codec->width;
	mEI.frame->height = vStream->codec->height;

	// allocate the video frame data
	int ret = av_image_alloc(mEI.frame->data, mEI.frame->linesize, mEI.width, mEI.height, vStream->codec->pix_fmt, 32);
	if (ret < 0) {
		fprintf(stderr, "Could not allocate raw picture buffer\n");
		return;
	}

	// dump codec info
	av_dump_format(mEI.oc, 0, filename, 1);

	// open the output file
	if (!(fmt->flags & AVFMT_NOFILE)) {
		if (avio_open(&mEI.oc->pb, filename, AVIO_FLAG_WRITE) < 0) {
			KIGS_ERROR("Could not open mp4 file output\n", 0);
		}
	}

	/* write the stream header, if any */
	avformat_write_header(mEI.oc, NULL);

	mEI.pkt = new AVPacket();
}

void CameraWUP::EncodeFrame(double Time, void* data, int dataLen)
{
	AVStream * vStream = mEI.oc->streams[0];

	// init packet
	av_init_packet(mEI.pkt);
	memcpy(mEI.frame->data[0], data, dataLen);
	mEI.pkt->size = dataLen;

	// encode the packet
	int got_output;
	int ret = avcodec_encode_video2(vStream->codec, mEI.pkt, mEI.frame, &got_output);
	mEI.frame->pts += av_rescale_q(1, vStream->codec->time_base, vStream->time_base);
	if (ret < 0) {
		fprintf(stderr, "Error encoding frame\n");
		return;
	}

	if (got_output) {
		printf(".");
		//av_interleaved_write_frame(mEI.oc, mEI.pkt);
		av_write_frame(mEI.oc, mEI.pkt);
	}

	av_free_packet(mEI.pkt);
}

void CameraWUP::CloseEncoder()
{
	AVStream * vStream = mEI.oc->streams[0];

	/* get the delayed frames */
	int got_output = 1;
	if ((vStream->codec->codec->capabilities&AV_CODEC_CAP_DELAY) != 0)
	{
		for (; got_output > 0;)
		{
			av_init_packet(mEI.pkt);
			mEI.pkt->data = NULL;    // packet data will be allocated by the encoder
			mEI.pkt->size = 0;

			int ret = avcodec_encode_video2(vStream->codec, mEI.pkt, NULL, &got_output);
			mEI.frame->pts += av_rescale_q(1, vStream->codec->time_base, vStream->time_base);
			if (ret < 0) {
				fprintf(stderr, "Error encoding frame\n");
				return;
			}

			if (got_output) {
				printf(".");
				//av_interleaved_write_frame(mEI.oc, mEI.pkt);
				av_write_frame(mEI.oc, mEI.pkt);
			}
		}
	}

	printf("\nwrite %d images\n", (int)vStream->nb_frames);

	av_free_packet(mEI.pkt);

	av_write_trailer(mEI.oc);

	if (!(mEI.oc->oformat->flags & AVFMT_NOFILE)) {
		/* close the output file */
		avio_close(mEI.oc->pb);
	}


	/* free the streams */
	for (unsigned int i = 0; i < mEI.oc->nb_streams; i++) {
		av_freep(mEI.oc->streams[i]->codec);
		av_freep(mEI.oc->streams[i]);
	}

	/* free the stream */
	av_free(mEI.oc);

	delete mEI.pkt;
}



bool CameraWUP::StartRecording(kstl::string & name)
{

	SmartPointer<FileHandle> file = Platform_FindFullName(name);
	const char * filename = file != 0 ? file->mFullFileName.c_str() : name.c_str();


	/* initialize libavcodec, and register all codecs and formats */
	av_register_all();


	mEI.width = mCaptureX;
	mEI.height = mCaptureY;
	switch (mFormat)
	{
	case 0: // RGB555
		mEI.PixFormat = AV_PIX_FMT_RGB555;
		break;
	case 1: // YUV422
		mEI.PixFormat = AV_PIX_FMT_YUV420P;
		break;
	case 2: // RGB24
		mEI.PixFormat = AV_PIX_FMT_RGB24;
		break;
	case 3: // YUV24
		mEI.PixFormat = AV_PIX_FMT_YUV420P;
		break;
	default:
		printf("Recording failed : BAD FORMAT\n");
		isRecording = false;
		return false;
	}

	InitEncoder(filename);

	isRecording = true;

	return true;
}

void CameraWUP::StopRecording()
{
	isRecording = false;
	needCloseEncoder = true;
}
#endif

void CameraWUP::ResetAllBuffers()
{
	ParentClassType::ResetAllBuffers();

	std::unique_lock lock(mMtx);
	mLatestFrame = nullptr;
}


MrcVideoEffectDefinition::MrcVideoEffectDefinition()
{
	StreamType(DefaultStreamType);
	HologramCompositionEnabled(DefaultHologramCompositionEnabled);
	RecordingIndicatorEnabled(DefaultRecordingIndicatorEnabled);
	VideoStabilizationEnabled(DefaultVideoStabilizationEnabled);
	VideoStabilizationBufferLength(DefaultVideoStabilizationBufferLength);
	//VideoStabilizationBufferLength(0);
	GlobalOpacityCoefficient(DefaultGlobalOpacityCoefficient);
}