#pragma once

#include "Core.h"

#include "TinyImage.h"

namespace Kigs
{
	namespace Core
	{
		class CoreRawBuffer;

	}
	namespace Pict
	{
		// ****************************************
		// * GIFClass class
		// * --------------------------------------
		/**
		* \file	GIFClass.h
		* \class	GIFClass
		* \ingroup TinyImageModule
		* \brief TinyImage specialized for PNG management.
		*
		*/
		// ****************************************
		class GIFClass : public TinyImage
		{
			friend class TinyImage;
		public:
			/// Destructor
			virtual ~GIFClass();

			/**
			* Constructor
			* \param fileName TGA file to load
			*/
			GIFClass(File::FileHandle* fileName);
			GIFClass(void* data, int sx, int sy, TinyImage::ImageFormat internalfmt);
			GIFClass(CoreRawBuffer* rawbuffer);

			/**
			* Main loading method
			* \param fileName TGA file to load
			*/
			virtual bool Load(File::FileHandle* fileName);

			bool Load(CoreRawBuffer* buffer);

			virtual void	Export(const char* filename);


		protected:

		};
	}
}
