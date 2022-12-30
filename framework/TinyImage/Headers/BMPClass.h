#pragma once

// base class
#include "TinyImage.h"

namespace Kigs
{

	namespace Pict
	{

		// ****************************************
		// * BMPClass class
		// * --------------------------------------
		/**
		* \file	BMPClass.h
		* \class	BMPClass
		* \ingroup TinyImageModule
		* \brief TinyImage specialized for BMP management.
		*
		*/
		// ****************************************
		class BMPClass : public TinyImage
		{
		public:
			/**
			* constructor
			* \param filename  the name of the file to load
			*/
			BMPClass(File::FileHandle* fileName);
			BMPClass(void* data, int sx, int sy, TinyImage::ImageFormat internalfmt);

			/// destructor
			virtual ~BMPClass();

		protected:
			virtual void	Export(const char* filename);
			/**
			* load image from file
			* \param filename the name of the file to load
			*/
			virtual bool	Load(File::FileHandle* fileName);

			/**
			* decompress the image using RLE 8bits decompression
			* \param bytes the bytes of the image to decompress
			*/
			void	UncompressRLE8bits(u8* bytes, ReadColorFunc func, unsigned int outPixelSize);

			/**
			* decompress the image using RLE 4bits decompression
			* \param bytes the bytes of the image to decompress
			*/
			void	UncompressRLE4bits(u8* bytes, ReadColorFunc func, unsigned int outPixelSize);

			/**
			* \struct bmpHeader
			* \brief header without 2Bytes ID (for alignement purpose)
			*/
			struct bmpHeader
			{
				unsigned int	mFileSize;            /// the size of the file
				unsigned int	mReserved;
				unsigned int	mBitmapDataOffset;    /// position of the data from the start of file
				unsigned int	mBitmapHeaderSize;    /// size of the header

				unsigned int	mSizeX;               /// width of the image in pixels
				unsigned int	mSizeY;               /// height of the image in pixels
				unsigned short	mBitplaneCount;
				unsigned short	mBitPerPixel;         /// color depth of the image

				unsigned int	mCompression;         /// mode of compression used
				unsigned int	mBitmapDataSize;
				unsigned int	mHRes;
				unsigned int	mVRes;
				unsigned int	mColorCount;          /// number of colors in the palette
				unsigned int	mImportantColorCount;
			};

			/// enumeration of RLE compression modes
			enum
			{
				NO_COMPRESSION = 0, /// no compression used
				RLE_8BITS = 1,      /// RLE 8bits compression
				RLE_4BITS = 2       /// RLE 4bits compression
			};

		};


	}
}

