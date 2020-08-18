#include "PrecompiledHeaders.h"
#include "ETCClass.h"
#include "ModuleFileManager.h"
#include "TinyImageLoaderContext.h"
#include "Core.h"
#include "zstd.h"
#ifndef JAVASCRIPT
#include "zlib.h"
#endif

ETCClass::ETCClass(FileHandle* fileName) :TinyImage()
{
	mIsVFlipped = false;
	myInitIsOK = Load(fileName);

}

ETCClass::~ETCClass()
{
}

bool	ETCClass::Load(FileHandle* fileName)
{
	bool result = false;
	u64 filelength;

	// free previous image if any
	if (myInitIsOK)
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

		//Read Header
		ETC_Header*	imageheader = (ETC_Header*)rawbuffer->buffer();

		int L_DataLen = imageheader->datasize;

		//bool L_bPreMulti = true;
		unsigned char* imgdata = (unsigned char*)rawbuffer->buffer();
		imgdata += sizeof(ETC_Header);

		myWidth = imageheader->sizex & 0xFFFF;
		myHeight = imageheader->sizey & 0xFFFF;

		mPixelLineSize = myWidth;

		if ((imageheader->sizex & 0xFFFF0000) || (imageheader->sizey & 0xFFFF0000))
		{
			myUsedWidth = ((imageheader->sizex >> 16) & 0xFFFF);
			myUsedHeight = ((imageheader->sizey >> 16) & 0xFFFF);
		}

		mFormat = ETC1;

		unsigned int L_Format = (imageheader->format & 0xFFFF);
		switch (L_Format)
		{
			case 0: // ETC1
			{
				mFormat = ETC1;
				break;
			}
			case 1: // ETC1A4
			{
				mFormat = ETC1A4;
				break;
			}
			case 2: // ETC1A8
			{
				mFormat = ETC1A8;
				break;
			}
			case 4: // ETC2
			{
				mFormat = ETC2;
				break;
			}
			case 5: // ETC2A8
			{
				mFormat = ETC2A8;
				break;
			}
		}

		mPixelDataSize = L_DataLen;

		kstl::string upExtension = ToUpperCase(fileName->mExtension);
		mPixels = new u8[mPixelDataSize];

		if (upExtension == ".ETX")// use zstd
		{

			unsigned int outLenght = ZSTD_decompress(mPixels, mPixelDataSize, imgdata, filelength- sizeof(ETC_Header));
			if (mPixelDataSize != outLenght)
			{
				KIGS_ERROR("Error decompressing ETX image", 2);
			}
		}
		else if(upExtension == ".ETZ") // zlib
		{
#ifndef JAVASCRIPT
			uLongf size = uLongf(mPixelDataSize);
			if (uncompress(mPixels, &size, (const u8*)imgdata, filelength - sizeof(ETC_Header)) != Z_OK)
			{
				KIGS_ERROR("Error decompressing ETZ image", 2);
			}
#else
			KIGS_ERROR("Error decompressing with zlib unsupported on javascript", 2);
#endif
		}
		else
		{
			memcpy(mPixels, imgdata, mPixelDataSize);

		}
	
		result = true;

		rawbuffer->Destroy();
	}

	if (result == false)
	{
		KIGS_ERROR("Error reading ETC file", 1);
	}

	return result;
}
