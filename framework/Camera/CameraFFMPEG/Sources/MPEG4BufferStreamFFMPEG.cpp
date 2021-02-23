#include "PrecompiledHeaders.h"
#include "MPEG4BufferStreamFFMPEG.h"
#include <algorithm>
#include <ModuleFileManager.h>


extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include "libavformat/avio.h"
#ifdef WIN32
#include "libavcodec/dxva2.h"
#endif
//#include "libavutil/hwcontext.h"
}



IMPLEMENT_CLASS_INFO(MPEG4BufferStreamFFMPEG)

struct AVPacket;
struct AVFrame;
struct AVFormatContext;
struct AVStream;
struct AVCodec;
struct SwsContext;

struct ReadInfo
{
	AVStream		*	vStream;
	AVCodec			*	codec;
	AVFrame			*	frame;
	AVFrame			*	RGBframe;
	AVFormatContext *	fmtContext;
	SwsContext		*	swsContext;
	AVPacket			avpkt;

	AVIOContext		*	ioContext;
};


//! constructor
MPEG4BufferStreamFFMPEG::MPEG4BufferStreamFFMPEG(const kstl::string& name, CLASS_NAME_TREE_ARG) : MPEG4BufferStream(name, PASS_CLASS_NAME_TREE_ARG)
, RI{new ReadInfo()}
{
	RI->fmtContext = 0;
	RI->vStream = 0;
	RI->RGBframe = 0;
	RI->codec = 0;
	RI->frame = 0;
	RI->swsContext = 0;
	mLastTime = -1.0;
	_timer_pts = 0;
	_stream_pts = 0;
	
	
}

//! destructor
MPEG4BufferStreamFFMPEG::~MPEG4BufferStreamFFMPEG()
{
	sws_freeContext(RI->swsContext);

	if (RI->frame)
		av_freep(&RI->frame->data);
	
	if (RI->RGBframe)
		av_freep(&RI->RGBframe->data);

	av_frame_free(&RI->frame);
	av_frame_free(&RI->RGBframe);

	avformat_close_input(&RI->fmtContext);
	delete RI->fmtContext;

	if (mIsAllocated)
		FreeFrameBuffers();
}

bool MPEG4BufferStreamFFMPEG::ReadNextFrame(AVPacket* avpkt)
{
	int frame_finished = 0;
	while (frame_finished == 0)
	{
		if (av_read_frame(RI->fmtContext, avpkt) < 0) break;
		avcodec_decode_video2(RI->vStream->codec, RI->frame, &frame_finished, avpkt);
	}
	// decode delayed frame
	if (frame_finished == 0 && (RI->codec->capabilities&AV_CODEC_CAP_DELAY) != 0)
	{
		avpkt->data = NULL;
		avpkt->size = 0;
		avcodec_decode_video2(RI->vStream->codec, RI->frame, &frame_finished, avpkt);
	}
	return frame_finished;
}
#include "Remotery.h"

void MPEG4BufferStreamFFMPEG::Process()
{
	if (mState == Stopped || !RI->fmtContext || !mIsAllocated) return;
	rmt_BeginCPUSample(Process, 0);
	kdouble dt = 0.0;
	kdouble now = mTimer->GetTime();

	if (mLastTime < 0.0) mLastTime = now;
	if (mState == Running)
	{
		mCurrentTime = now - mStartTime;
		dt = now - mLastTime;
		mLastTime = now;
		_timer_pts = av_rescale_q((int64_t)(mCurrentTime*AV_TIME_BASE), { 1, AV_TIME_BASE }, RI->vStream->time_base);

		if (_timer_pts >= RI->vStream->duration)
			mHasReachEnd = true;
	}
	else
	{
		mCurrentTime = mPauseTime - mStartTime;
	}
	
	SyncStream();
	rmt_EndCPUSample();

}

void MPEG4BufferStreamFFMPEG::SyncStream()
{
	
	int count = 0;
	rmt_BeginCPUSample(ReadFrames, 0);
	while (_stream_pts < _timer_pts)
	{
		if (!ReadNextFrame(&RI->avpkt))
		{
			Stop();
			break;
		}
		count++;
		if(RI->avpkt.dts != AV_NOPTS_VALUE)
			_stream_pts = av_frame_get_best_effort_timestamp(RI->frame);

		double frame_delay = av_q2d(RI->vStream->time_base);
		frame_delay +=  RI->frame->repeat_pict * (frame_delay * 0.5);
		_stream_pts += frame_delay;
	}
	rmt_EndCPUSample();

	if(count)
	{
		// printf("read %d frames\n", count);
		int buffertoset = GetFreeBuffer();
		if (buffertoset != -1)
		{
			SetBufferState(buffertoset, ProcessingBuffer);
			rmt_BeginCPUSample(Convert1, 0);
			sws_scale(RI->swsContext, RI->frame->data, RI->frame->linesize, 0, mHeight, RI->RGBframe->data, RI->RGBframe->linesize);
			rmt_EndCPUSample();
			rmt_BeginCPUSample(Convert2, 0);

			// fill the buffer
			unsigned char* dest = GetBuffer(buffertoset);

			if (dest)
			{
				memcpy(dest, RI->RGBframe->data[0], mFrameSize);
				// ConvertFormat_RGB24ToYUV24(src, dest, mWidth, mHeight);
				
				/*for(int y=0;y<mHeight;++y)
				{
					for(int x=0; x<mWidth; ++x)
					{
						dest[3 * x + 0] = srcY[x];
						dest[3 * x + 1] = srcU[x];
						dest[3 * x + 2] = srcV[x];
					}
					srcY += RI->RGBframe->linesize[0];
					srcU += RI->RGBframe->linesize[1];
					srcV += RI->RGBframe->linesize[2];
					dest += mWidth * 3;
				}*/
				SetBufferState(buffertoset, ReadyBuffer);
			}
			else
			{
				SetBufferState(buffertoset, FreeBuffer);
			}
			rmt_EndCPUSample();

		}
	}
}



int MPEG4BufferStreamFFMPEG::ReadFunc(unsigned char* buf, int buf_size)
{
	int read = Platform_fread(buf, 1, buf_size,  _file_handle.get());
	if(read == -1)
		return AVERROR_EOF;
	return read; 

	// if (_stream_buffer.pos == _stream_buffer.size)
	// {
	// return AVERROR_EOF;
	// }

	// if (_stream_buffer.pos + buf_size > _stream_buffer.size)
	// 	buf_size = _stream_buffer.size - _stream_buffer.pos;

	// memcpy(buf, _stream_buffer.buffer->buffer() + _stream_buffer.pos, buf_size);
	// _stream_buffer.pos += buf_size;
	// return buf_size;
}

int64_t MPEG4BufferStreamFFMPEG::SeekFunc(int64_t pos, int whence)
{
	if(whence == AVSEEK_SIZE)
	{
		auto old_pos = Platform_ftell(_file_handle.get());
		Platform_fseek(_file_handle.get(), 0, SEEK_END);
		auto size = Platform_ftell(_file_handle.get());
		Platform_fseek(_file_handle.get(), old_pos, SEEK_SET);
		return size;
		// return _stream_buffer.size;
	}
	
	// if (whence == SEEK_SET)
	// 	_stream_buffer.pos = pos;
	// else if (whence == SEEK_CUR)
	// 	_stream_buffer.pos += pos;
	// else if (whence == SEEK_END)
	// 	_stream_buffer.pos = _stream_buffer.size;
	// else
	// 	return -1;

	// return 0;
	
	return Platform_fseek(_file_handle.get(), pos, whence);
}

static int ffmpeg_read_func(void* ptr, uint8_t* buf, int buf_size)
{
	return static_cast<MPEG4BufferStreamFFMPEG*>(ptr)->ReadFunc(buf, buf_size);
}

static int64_t ffmpeg_seek_func(void* ptr, int64_t pos, int whence)
{
	return static_cast<MPEG4BufferStreamFFMPEG*>(ptr)->SeekFunc(pos, whence);
}

static AVHWAccel *ff_find_hwaccel(AVCodecID codec_id, AVPixelFormat pix_fmt)
{
	AVHWAccel *hwaccel = NULL;

	while ((hwaccel = av_hwaccel_next(hwaccel)))
	{
		if (hwaccel->id == codec_id
			&& hwaccel->pix_fmt == pix_fmt)
			return hwaccel;
	}
	return NULL;
}

void MPEG4BufferStreamFFMPEG::InitModifiable()
{
	MPEG4BufferStream::InitModifiable();
	
	auto& pathManager = KigsCore::Singleton<FilePathManager>();
	_file_handle = pathManager->FindFullName(mFileName.const_ref());

	if ((_file_handle->mStatus & FileHandle::Exist) == 0)
	{
		UninitModifiable();
		return;
	}
	Platform_fopen(_file_handle.get(), "rb");
	// _stream_buffer.buffer = ModuleFileManager::LoadFile(_file_handle, _stream_buffer.size, 0);
	// _stream_buffer.pos = 0;

	av_log_set_level(AV_LOG_QUIET);
	av_register_all();

	// 32 kb buffer for ffmpegm
	_internal_buffer.resize(32 * 1024); 

	RI->ioContext = avio_alloc_context(_internal_buffer.data(), _internal_buffer.size(), 0, this, ffmpeg_read_func, 0, ffmpeg_seek_func);
	RI->fmtContext = avformat_alloc_context();
	RI->fmtContext->pb = RI->ioContext;
	RI->fmtContext->flags = AVFMT_FLAG_CUSTOM_IO;

	AVProbeData probe_data;
	probe_data.buf = _internal_buffer.data();
	probe_data.buf_size = _internal_buffer.size();
	probe_data.filename = "";
	probe_data.mime_type = "";

	ReadFunc(_internal_buffer.data(), _internal_buffer.size());
	SeekFunc(0, SEEK_SET);
	RI->fmtContext->iformat = av_probe_input_format(&probe_data, 1);
	
	auto error = avformat_open_input(&RI->fmtContext, "", NULL, NULL);
	if (error != 0)
	{
		char str[256];
		av_strerror(error, str, 256);

		return;
	}
	if (avformat_find_stream_info(RI->fmtContext, NULL) < 0)
		return;

	// av_dump_format(RI->fmtContext, 0, mFileName.c_str(), 0);

	// retreive video stream
	RI->vStream = NULL;
	for (unsigned int i = 0; i < RI->fmtContext->nb_streams; i++)
	{
		if (RI->fmtContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			RI->vStream = RI->fmtContext->streams[i];
			break;
		}
	}

	// retreive the used codec and open it
	RI->codec = avcodec_find_decoder(RI->vStream->codec->codec_id);

	if (RI->codec == NULL) return;
	
	//PIX_FMT_DXVA2_VLD
	//RI->vStream->codec->pix_fmt = AVPixelFormat::AV_PIX_FMT_DXVA2_VLD;

#if CONFIG_MPEG2_DXVA2_HWACCEL
	RI->vStream->codec->pix_fmt = AVPixelFormat::AV_PIX_FMT_DXVA2_VLD; // AVPixelFormat::AV_PIX_FMT_NV12;
	RI->vStream->codec->hwaccel = ff_find_hwaccel(RI->vStream->codec->codec_id, RI->vStream->codec->pix_fmt);
#endif


	if (avcodec_open2(RI->vStream->codec, RI->codec, NULL) < 0)return;


	av_init_packet(&RI->avpkt);


	// allocate the video frame
	RI->frame = av_frame_alloc();
	if (!RI->frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		return;
	}

	mWidth = RI->vStream->codec->width;
	mHeight = RI->vStream->codec->height;

	// allocate the video frame data
	int ret = av_image_alloc(RI->frame->data, RI->frame->linesize, mWidth, mHeight, /*AVPixelFormat::AV_PIX_FMT_NV12*/RI->vStream->codec->pix_fmt, 32);
	if (ret < 0) {
		char str[1024];
		av_make_error_string(str, 1024, ret);
		fprintf(stderr, "Could not allocate raw picture buffer: %s\n", str);
		return;
	}
	

	// set frame parameters
	RI->frame->pts = 0;
	RI->frame->format = RI->vStream->codec->pix_fmt;
	RI->frame->width = mWidth;
	RI->frame->height = mHeight;


	// allocate the RGB frame
	RI->RGBframe = av_frame_alloc();
	if (!RI->RGBframe) {
		fprintf(stderr, "Could not allocate video frame\n");
		return;
	}
	// allocate the RGB video frame data
	ret = av_image_alloc(RI->RGBframe->data, RI->RGBframe->linesize, mWidth, mHeight, AV_PIX_FMT_RGBA, 32);
	if (ret < 0) {
		fprintf(stderr, "Could not allocate raw picture buffer\n");
		return;
	}

	// set frame parameters
	RI->RGBframe->format = AV_PIX_FMT_RGBA;
	RI->RGBframe->width = mWidth;
	RI->RGBframe->height = mHeight;


	RI->swsContext = sws_getContext(	mWidth, mHeight, /*AVPixelFormat::AV_PIX_FMT_NV12*/RI->vStream->codec->pix_fmt,	// in parameters
									mWidth, mHeight, AV_PIX_FMT_RGBA,				// out parameters
									SWS_BICUBIC, NULL, NULL, NULL);					// other parameters
	if (RI->swsContext == NULL)
	{
		fprintf(stderr, "Cannot initialize the conversion context!\n");
		return;
	}
	
	_stream_duration = RI->vStream->duration; // av_rescale_q(RI->vStream->duration, RI->vStream->time_base, { 1, AV_TIME_BASE });

	mDuration = (kdouble)av_rescale_q(_stream_duration, RI->vStream->time_base, { 1, AV_TIME_BASE }) / AV_TIME_BASE;
	// retreive FPS
	mFps = (float)RI->vStream->r_frame_rate.num / (float)RI->vStream->r_frame_rate.den;
	
	// myFrametime = av_rescale_q((int64_t)((1.0 / myFps)*AV_TIME_BASE), { 1, AV_TIME_BASE }, RI->vStream->time_base);

	mLineSize = RI->RGBframe->linesize[0];
	mFrameSize = mLineSize * mHeight;// avpicture_get_size(AV_PIX_FMT_RGBA, mWidth, mHeight);
	

	//mFrameSize = avpicture_get_size(RI->vStream->codec->pix_fmt, mWidth, mHeight);

	AllocateFrameBuffers();
	mIsAllocated = true;
	

}

void MPEG4BufferStreamFFMPEG::SeekPts(int64_t seek_pts)
{
	if (seek_pts != _stream_pts){
		avcodec_flush_buffers(RI->vStream->codec);
		int flags = seek_pts < _stream_pts ? AVSEEK_FLAG_BACKWARD : 0;
		if (avformat_seek_file(RI->fmtContext, RI->vStream->index, INT64_MIN, seek_pts, seek_pts, flags) >= 0)
		{
			_stream_pts = 0;
			_timer_pts = seek_pts;
			SyncStream();
		}
	}
}

void MPEG4BufferStreamFFMPEG::SetTime(kdouble t)
{
	int64_t seek_pts = av_rescale_q((int64_t)(t*AV_TIME_BASE), { 1, AV_TIME_BASE }, RI->vStream->time_base);
	seek_pts = std::max(0ll, std::min(seek_pts, _stream_duration));
	MPEG4BufferStream::SetTime(t);
	SeekPts(seek_pts);
}

void MPEG4BufferStreamFFMPEG::NextFrame()
{
	_timer_pts = _stream_pts + 1;
	SyncStream();
	_timer_pts = _stream_pts;
	MPEG4BufferStream::SetTime(mTimer->GetTime());
}

