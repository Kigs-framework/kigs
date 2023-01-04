#pragma once

#include "FrameBufferStream.h"

namespace Kigs
{
	namespace Camera
	{

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
				MPEG4BufferStream(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			virtual void Start();
			virtual void Pause();
			virtual void Stop();
			virtual void NextFrame() = 0;
			virtual void SetTime(double t) = 0;

			MPEG4State GetState() { return mState; }

			double GetDuration() { return mDuration; }
			double GetTime() { return mCurrentTime; }
			double GetFps() { return mFps; }

		protected:

			MPEG4State mState;


			void AllocateFrameBuffers() override;
			void FreeFrameBuffers() override;


			maString mFileName;

			bool mHasReachEnd;

			double mStartTime;
			double mPauseTime;
			double mLastTime;
			double mDuration;

			unsigned char** mBuffers;

			int mFrameSize;


			double mFps;

			bool mIsAllocated;

			double mCurrentTime;

			DECLARE_METHOD(Play);
			DECLARE_METHOD(Pause);
			DECLARE_METHOD(Stop);
			DECLARE_METHOD(NextFrame);
			DECLARE_METHOD(HasReachEnd);
			COREMODIFIABLE_METHODS(Play, Pause, Stop, NextFrame, HasReachEnd);
		};

	}
}
