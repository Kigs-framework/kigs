#pragma once

#include "FrameBufferStream.h"

namespace Kigs
{
	namespace Camera
	{
		// ****************************************
		// * FixedImageBufferStream class
		// * --------------------------------------
		/**
		* \file	FixedImageBufferStream.h
		* \class	FixedImageBufferStream
		* \ingroup Camera
		* \brief Utility class to use a fixed image as a buffer stream
		*/
		// ****************************************

		class FixedImageBufferStream : public FrameBufferStream
		{
		public:

			DECLARE_CLASS_INFO(FixedImageBufferStream, FrameBufferStream, CameraModule)

				//! constructor
				FixedImageBufferStream(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);


			void	Process() override;

			void	InitModifiable() override;

			virtual ~FixedImageBufferStream();

		protected:


			void AllocateFrameBuffers() override;
			void FreeFrameBuffers() override;

			maString	mFilename;
			maBool		mNoConvert;
			unsigned char* mImageData;

		};

	}
}