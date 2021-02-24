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
						unsigned char* readPixel = ((unsigned char*)currentImage.RasterBits)+ y* readLineLen;
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