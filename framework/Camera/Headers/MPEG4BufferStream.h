#ifndef _MPEG4BufferStream_H_
#define _MPEG4BufferStream_H_

#include "FrameBufferStream.h"


// ****************************************
// * MPEG4BufferStream class
// * --------------------------------------
/**
* \file	MPEG4BufferStream.h
* \class	MPEG4BufferStream
* \ingroup Camera
* \brief   Base class for MP4 video stream playing
*/
// ****************************************

class MPEG4BufferStream : public FrameBufferStream
{
public:

	enum MPEG4State
	{
		Running,
		Paused,
		Stopped
	};

	DECLARE_ABSTRACT_CLASS_INFO(MPEG4BufferStream, FrameBufferStream, CameraModule)

	//! constructor
	MPEG4BufferStream(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	virtual void Start();
	virtual void Pause();
	virtual void Stop();
	virtual void NextFrame() = 0;
	virtual void SetTime(kdouble t) = 0;

	MPEG4State GetState() { return mState; }

	kdouble GetDuration() { return mDuration;  }
	kdouble GetTime() { return mCurrentTime; }
	kdouble GetFps() { return mFps; }

protected:

	MPEG4State mState;

	
	void AllocateFrameBuffers() override;
	void FreeFrameBuffers() override;


	maString mFileName;

	bool mHasReachEnd;

	kdouble mStartTime;
	kdouble mPauseTime;
	kdouble mLastTime;
	kdouble mDuration;

	unsigned char** mBuffers;

	int mFrameSize;


	kdouble mFps;

	bool mIsAllocated;

	kdouble mCurrentTime;

	DECLARE_METHOD(Play);
	DECLARE_METHOD(Pause);
	DECLARE_METHOD(Stop);
	DECLARE_METHOD(NextFrame);
	DECLARE_METHOD(HasReachEnd);
	COREMODIFIABLE_METHODS(Play, Pause, Stop, NextFrame, HasReachEnd);
};

#endif //_MPEG4BufferStream_H_
