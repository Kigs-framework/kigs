#include "PrecompiledHeaders.h"
#include "GIFClass.h"
#include "TinyImageLoaderContext.h"
#include "ModuleFileManager.h"



GIFClass::GIFClass(FileHandle* fileName) :TinyImage()
{
	mInitIsOK = Load(fileName);
}

GIFClass::GIFClass(CoreRawBuffer* rawbuffer) : TinyImage()
{
	mInitIsOK = Load(rawbuffer);
}

GIFClass::GIFClass(void* data, int sx, int sy, TinyImage::ImageFormat internalfmt) : TinyImage(data, sx, sy, internalfmt)
{
}

GIFClass::~GIFClass()
{
}



#ifdef USE_LIB_PNG // TODO specific define here ?
#include "gif_lib.h"

struct memoryGifRead
{
	char *			mBuffer;
	int				mCurrentReadPos;
	int				mTotalSize;
};

int GifInputFunc(GifFileType* head, GifByteType* dest, int readsize)
{
	memoryGifRead& kigsGIFStruct = *(memoryGifRead*)head->UserData;
	if ((readsize + kigsGIFStruct.mCurrentReadPos) > kigsGIFStruct.mTotalSize)
	{
		readsize = kigsGIFStruct.mTotalSize - kigsGIFStruct.mCurrentReadPos;
	}

	memcpy(dest, kigsGIFStruct.mBuffer + kigsGIFStruct.mCurrentReadPos, readsize);

	kigsGIFStruct.mCurrentReadPos += readsize;

	return readsize;
}

bool GIFClass::Load(CoreRawBuffer* rawbuffer)
{
	bool result = false;
	if (rawbuffer)
	{
		int Error=0;
		memoryGifRead	kigsGIFStruct;
		kigsGIFStruct.mBuffer = rawbuffer->buffer();
		kigsGIFStruct.mCurrentReadPos = 0;
		kigsGIFStruct.mTotalSize = rawbuffer->length();


		GifFileType* resultGif= DGifOpen(&kigsGIFStruct, GifInputFunc, &Error);    /* new one (TVT) */
		if (Error == 0)
		{
			if (DGifSlurp(resultGif) == GIF_OK)
			{
				switch (resultGif->SColorResolution)
				{
				case 8:
				{
					
					mFormat = RGB_24_888;
					SavedImage& currentImage = resultGif->SavedImages[0];
					GifImageDesc& imgDesc = currentImage.ImageDesc;
					mPixelLineSize = imgDesc.Width * 3;
					mPixelDataSize = mPixelLineSize * imgDesc.Height;
					mPixels = new unsigned char[mPixelDataSize];
					mWidth = imgDesc.Width;
					mHeight = imgDesc.Height;
					unsigned char* pixels = mPixels;
					// not sure
					int readLineLen = resultGif->SWidth;
					int x, y;
					for (y = 0; y < resultGif->Image.Height; y++)
					{
						unsigned char* readPixel = ((unsigned char*)currentImage.RasterBits)+ imgDesc.Top* readLineLen;
						for (x = 0; x < resultGif->Image.Width; x++)
						{
							pixels[0] = resultGif->SColorMap->Colors[*readPixel].Red;
							pixels[1] = resultGif->SColorMap->Colors[*readPixel].Green;
							pixels[2] = resultGif->SColorMap->Colors[*readPixel].Blue;
							readPixel++;
							pixels += 3;
						}
					}
					result = true;
				}
					break;
				case 16:
					// TODO ?
					break;
				case 24:
					mFormat = RGB_24_888;
					break;
				case 32:
					mFormat = RGBA_32_8888;
					break;
				}


			}
		}
		DGifCloseFile(resultGif, &Error);
	}

/*
		u8* pData = (u8*)rawbuffer->buffer();
		u32 nDatalen = rawbuffer->size();

		png_byte        header[8] = { 0 };
		png_structp     png_ptr = 0;
		png_infop       info_ptr = 0;

		do
		{
			// png header len is 8 bytes
			if (nDatalen < 8)
				break;

			// check the data is png or not
			memcpy(header, pData, 8);
			if (png_sig_cmp(header, 0, 8))
				break;

			// init png_struct
			png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
			if (!png_ptr)
				break;

			// init png_info
			info_ptr = png_create_info_struct(png_ptr);
			if (!info_ptr || setjmp(png_jmpbuf(png_ptr)))
				break;

			// set the read call back function
			tImageSource imageSource;
			imageSource.data = (unsigned char*)pData;
			imageSource.size = (int)nDatalen;
			imageSource.offset = 0;
			png_set_read_fn(png_ptr, &imageSource, pngReadCallback);

			// read png
			// PNG_TRANSFORM_EXPAND: perform set_expand()
			// PNG_TRANSFORM_PACKING: expand 1, 2 and 4-bit samples to bytes
			// PNG_TRANSFORM_STRIP_16: strip 16-bit samples to 8 bits
			// PNG_TRANSFORM_GRAY_TO_RGB: expand grayscale samples to RGB (or GA to RGBA)
			png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_PACKING
				, 0);

			int         color_type = 0;
			png_uint_32 nWidth = 0;
			png_uint_32 nHeight = 0;
			int         nBitsPerComponent = 0;
			png_get_IHDR(png_ptr, info_ptr, &nWidth, &nHeight, &nBitsPerComponent, &color_type, 0, 0, 0);

			mWidth = nWidth;
			mHeight = nHeight;
			mPaletteDataSize = 0;
			// init image info

			int componentCount;
			switch (color_type)
			{
			case 0:
			case 4:
				componentCount = 1;
				break;
			case 2:
			case 6:
				componentCount = 3;
				break;
			default:
				return false;
			}

			// add alpha component
			if ((color_type & PNG_COLOR_MASK_ALPHA))
				componentCount++;

			// calc data size and alloc buffer
			mPixelLineSize = nWidth * componentCount;
			mPixelDataSize = mPixelLineSize * nHeight;
			mPixels = new unsigned char[mPixelDataSize];

			png_bytep * rowPointers = png_get_rows(png_ptr, info_ptr);
			for (unsigned int j = 0; j < nHeight; ++j)
			{
				memcpy(mPixels + j * mPixelLineSize, rowPointers[j], mPixelLineSize);
			}

			switch (componentCount)
			{
			case 4:
				mFormat = RGBA_32_8888;
				break;
			case 3:
				mFormat = RGB_24_888;
				break;
			case 2:
				mFormat = AI88;
				break;
			case 1:
				mFormat = GREYSCALE;
			default:
				break;
			}

			result = true;
		} while (0);

		if (png_ptr)
		{
			png_destroy_read_struct(&png_ptr, (info_ptr) ? &info_ptr : 0, 0);
		}
	}*/
	return result;
}

bool GIFClass::Load(FileHandle* fileName)
{
	bool result = false;

	// free previous image if any
	if (mInitIsOK)
	{
		if (mPixels)
		{
			delete[] mPixels;
			mPixels = 0;
		}
		if (mPalette)
		{
			delete[] mPalette;
			mPalette = 0;
		}
	}

	u64 nDatalen;
	CoreRawBuffer* rawbuffer = ModuleFileManager::LoadFile(fileName, nDatalen);
	if (rawbuffer)
	{
		result = Load(rawbuffer);
		rawbuffer->Destroy();
	}
	return result;
}


void	GIFClass::Export(const char* filename)
{

}

#else

#include "Platform/TinyImage/GIFClassLoader.inl.h"

#endif // USE_LIB_PNG