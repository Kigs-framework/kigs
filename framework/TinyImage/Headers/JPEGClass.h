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
		// * JPEGClass class
		// * --------------------------------------
		/**
		* \file	JPEGClass.h
		* \class	JPEGClass
		* \ingroup TinyImageModule
		* \brief TinyImage specialized for JPEG management.
		*
		*/
		// ****************************************
		class JPEGClass : public TinyImage
		{
			friend class TinyImage;
		public:
			/// Destructor
			virtual ~JPEGClass();

			/**
			* Constructor
			* \param fileName TGA file to load
			*/
			JPEGClass(File::FileHandle* fileName);
			JPEGClass(CoreRawBuffer* rawbuffer);

			/**
			* Main loading method
			* \param fileName TGA file to load
			*/
			virtual bool Load(File::FileHandle* fileName);
			bool Load(CoreRawBuffer* fileName);


			virtual void	Export(const char* filename)
			{
				// TODO
			};


		protected:

		};

		std::vector<u8> RGB24ToJpeg(unsigned char* image_buffer, int image_width, int image_height, int quality);
	}
}
