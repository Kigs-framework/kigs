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
		// * PNGClass class
		// * --------------------------------------
		/**
		* \file	PNGClass.h
		* \class	PNGClass
		* \ingroup TinyImageModule
		* \brief TinyImage specialized for PNG management.
		*
		*/
		// ****************************************
		class PNGClass : public TinyImage
		{
			friend class TinyImage;
		public:
			/// Destructor
			virtual ~PNGClass();

			/**
			* Constructor
			* \param fileName TGA file to load
			*/
			PNGClass(File::FileHandle* fileName);
			PNGClass(void* data, int sx, int sy, TinyImage::ImageFormat internalfmt);
			PNGClass(Core::CoreRawBuffer* rawbuffer);

			/**
			* Main loading method
			* \param fileName TGA file to load
			*/
			virtual bool Load(File::FileHandle* fileName);

			bool Load(Core::CoreRawBuffer* buffer);

			virtual void	Export(const char* filename);


		protected:

		};

	}
}
