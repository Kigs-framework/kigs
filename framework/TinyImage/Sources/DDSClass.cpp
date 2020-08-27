#include "PrecompiledHeaders.h"
#include "DDSClass.h"
#include "ModuleFileManager.h"
#include "TinyImageLoaderContext.h"
#include "Core.h"

DDSClass::DDSClass(FileHandle* fileName) :TinyImage()
, mMipmapCount(0)
{
	mIsVFlipped = false;
	mInitIsOK = Load(fileName);
	
}

DDSClass::~DDSClass()
{
}

const unsigned int FCC_DXT1 = *(unsigned int*)"DXT1";
const unsigned int FCC_DXT2 = *(unsigned int*)"DXT2";
const unsigned int FCC_DXT3 = *(unsigned int*)"DXT3";
const unsigned int FCC_DXT4 = *(unsigned int*)"DXT4";
const unsigned int FCC_DXT5 = *(unsigned int*)"DXT5";

bool	DDSClass::Load(FileHandle* fileName)
{
	bool result = false;
	u64 filelength;

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

	CoreRawBuffer* rawbuffer = ModuleFileManager::LoadFile(fileName, filelength);
	if (rawbuffer)
	{
		u8* memfile = (u8*)rawbuffer->buffer();

		DDS_HEADER*	imageheader = (DDS_HEADER*)(memfile + 4);
		size_t dataSize = size_t(filelength - sizeof(DDS_HEADER) - 4);

		unsigned char* imgdata = (unsigned char*)imageheader;
		imgdata += sizeof(DDS_HEADER);

		mWidth = imageheader->dwWidth;
		mHeight = imageheader->dwHeight;
		mMipmapCount = imageheader->dwMipMapCount;

		mPixelLineSize = mWidth;

		if (imageheader->ddspf.dwFourCC == FCC_DXT1)
		{
			mFormat = BC1;
			mPixelLineSize = mWidth / 2;
		}
		else if ((imageheader->ddspf.dwFourCC == FCC_DXT2) || (imageheader->ddspf.dwFourCC == FCC_DXT3))
		{
			mFormat = BC2;
		}
		else if ((imageheader->ddspf.dwFourCC == FCC_DXT4) || (imageheader->ddspf.dwFourCC == FCC_DXT5))
		{
			mFormat = BC3;
		}

		mPixelDataSize = mPixelLineSize*mHeight;

		// if mimmap count, then put all mipmap in buffer
		if (mMipmapCount)
		{
			mPixels = new u8[dataSize];
			memcpy(mPixels, imgdata, dataSize);
		}
		else
		{
			mPixels = new u8[mPixelDataSize];
			memcpy(mPixels, imgdata, (size_t)mPixelDataSize);
		}
		result = true;

		rawbuffer->Destroy();
	}

	if (result == false)
	{
		KIGS_ERROR("Error reading DDS file", 1);
	}

	return result;
}
