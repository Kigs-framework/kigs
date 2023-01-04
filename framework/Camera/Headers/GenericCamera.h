#pragma once
#include "FrameBufferStream.h"

namespace Kigs
{
	namespace Camera
	{
		// ****************************************
		// * GenericCamera class
		// * --------------------------------------
		/**
		* \file	GenericCamera.h
		* \class	GenericCamera
		* \ingroup Camera
		* \brief Base class to manage webcamera / video capture devices.
		*/
		// ****************************************


		class GenericCamera : public FrameBufferStream
		{
		public:
			enum CamState
			{
				isStopped,
				isPaused,
				isRunning,
				isInit,
				isInInit
			};

			DECLARE_ABSTRACT_CLASS_INFO(GenericCamera, FrameBufferStream, CameraModule)
				DECLARE_INLINE_CONSTRUCTOR(GenericCamera) {};

			virtual bool StartRecording(std::string&) { return false; };
			virtual void StopRecording() {};

			virtual void SetWhiteBalance(int i) {};
			virtual int GetSupportedWhiteBalanceCount() { return 0; };

			bool NeedCrop() { return mNeedCrop; }

			virtual void StartPreview() = 0;
			virtual void StopPreview() = 0;

		protected:
			void InitModifiable() override;
			void NotifyUpdate(const unsigned int /* labelid */) override;

			CamState mState;

			// called when one of the width / height / offset value is changed
			void RecomputeAllParams();


			// As Camera inherit FrameBufferStream, so use mWidth / mHeight to determine wanted output size
			// if  mWidth and/or mHeight are 0, then use mInitResX & mInitResY to determine capture size
			// if mInitResX and/or mInitResY are 0, then use camera default resolution 

			// init resolution, if not set, use Capture size
			maInt mInitResX = BASE_ATTRIBUTE(InitResX, 0);
			maInt mInitResY = BASE_ATTRIBUTE(InitResY, 0);

			// if init res is larger than width / height, offset is use
			maInt mOffsetX = BASE_ATTRIBUTE(OffsetX, 0);
			maInt mOffsetY = BASE_ATTRIBUTE(OffsetY, 0);

			maFloat mFOV = BASE_ATTRIBUTE(FOV, 1.0f);

			maInt mSkippedFrameCount = BASE_ATTRIBUTE(SkippedFrameCount, 0);
			int mFrameCounter = 0;
			bool mNeedCrop = false;

			DECLARE_METHOD(Start);
			COREMODIFIABLE_METHODS(Start);
		};

	}
}