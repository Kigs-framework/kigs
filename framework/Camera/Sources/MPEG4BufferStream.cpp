#include "PrecompiledHeaders.h"
#include "MPEG4BufferStream.h"
#include "Core.h"



using namespace Kigs::Camera;

IMPLEMENT_CLASS_INFO(MPEG4BufferStream)

//! constructor
MPEG4BufferStream::MPEG4BufferStream(const std::string& name, CLASS_NAME_TREE_ARG) : FrameBufferStream(name, PASS_CLASS_NAME_TREE_ARG)
, mFileName(*this, false, "FileName", "")
, mBuffers(NULL)
, mIsAllocated(false)
{
	mHasReachEnd = false;
	mState = Stopped;
	mCurrentTime = -1;
	mFrameSize = 0;
	mFormat.setValue("YUV24");
}     



void MPEG4BufferStream::Start()
{
	mHasReachEnd = false;
	auto now = mTimer->GetTime();
	if (mState == Paused)
	{
		mStartTime += now - mPauseTime;
	} else
	{
		mStartTime = now;
	}
	mState = Running;
	mLastTime = now;
}

void MPEG4BufferStream::Pause()
{
	if (mState == Running){
		mState = Paused;
		mPauseTime = mTimer->GetTime();
	}

}

void MPEG4BufferStream::Stop()
{
	mHasReachEnd = true;
	mState = Stopped;
	SetTime(0);
	mLastTime = -1.0;
}



void MPEG4BufferStream::SetTime(double t)
{
	double current_time = mTimer->GetTime();
	mStartTime = current_time - t; 
	mCurrentTime = current_time;
	mPauseTime = current_time;
}



void MPEG4BufferStream::AllocateFrameBuffers()
{
	mFrameBuffers[0] = std::make_shared<AlignedCoreRawBuffer<16, unsigned char>>(mFrameSize, false); 
	mFrameBuffers[1] = std::make_shared<AlignedCoreRawBuffer<16, unsigned char>>(mFrameSize, false);
	mFrameBuffers[2] = std::make_shared<AlignedCoreRawBuffer<16, unsigned char>>(mFrameSize, false);
}

void MPEG4BufferStream::FreeFrameBuffers()
{
	mFrameBuffers[0] = mFrameBuffers[1] = mFrameBuffers[2] = nullptr;
}

DEFINE_METHOD(MPEG4BufferStream, Play)
{
	Start();
	return false;
}

DEFINE_METHOD(MPEG4BufferStream, Pause)
{
	Pause();
	return false;
}

DEFINE_METHOD(MPEG4BufferStream, Stop)
{
	Stop();
	return false;
}

DEFINE_METHOD(MPEG4BufferStream, NextFrame)
{
	NextFrame();
	return false;
}

DEFINE_METHOD(MPEG4BufferStream, HasReachEnd)
{
	*((bool*)privateParams) = mHasReachEnd;
	return false;
}