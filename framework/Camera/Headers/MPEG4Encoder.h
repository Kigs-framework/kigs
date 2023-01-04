#pragma once

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"

namespace Kigs
{
	namespace Camera
	{
		using namespace Kigs::Core;
		// ****************************************
		// * MPEG4Encoder class
		// * --------------------------------------
		/**
		* \file	MPEG4Encoder.h
		* \class	MPEG4Encoder
		* \ingroup Camera
		* \brief   Base class for MP4 video stream encoding
		*/
		// ****************************************

		class MPEG4Encoder : public CoreModifiable
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(MPEG4Encoder, CoreModifiable, CameraModule);
			DECLARE_CONSTRUCTOR(MPEG4Encoder);

			virtual bool InitEncoder(const char* filename, bool flipInput) = 0;
			virtual void CloseEncoder() = 0;

			virtual void EncodeFrame(double Time, void* data, int dataLen, int stride = 0) = 0;
			virtual bool StartRecording() = 0;
			virtual void StopRecording() = 0;

		protected:
			virtual ~MPEG4Encoder();
			virtual void InitModifiable() override;

			maUInt mSourceWidth;
			maUInt mSourceHeight;
			maUInt mTargetWidth;
			maUInt mTargetHeight;
			maString mFileName;

			maBool mVFlip;

			maEnum<3> mSourceFormat;
			maEnum<3> mTargetFormat;
		};
	}
}