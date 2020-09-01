#include "PrecompiledHeaders.h"
#include "TinyImage.h"
#include "TinyImageLoaderContext.h"
#include "BMPClass.h"
#include "TGAClass.h"
#include "JPEGClass.h"
#include "PNGClass.h"
#include "DDSClass.h"
#include "ETCClass.h"

#include "Core.h"
#include "FilePathManager.h"




TinyImageLoaderContext* TinyImage::mLoaderContext=&TinyImageLoaderContext::GetDefault();

kstl::vector<TinyImageLoaderContext>*	TinyImage::mContextStack=0;

TinyImage::TinyImage(): GenericRefCountedBaseClass()
,mInitIsOK(false)
,mWidth(0)
,mHeight(0)
, mUsedWidth(-1)
, mUsedHeight(-1)
,mPalette(0)
,mPaletteDataSize(0)
,mPixels(0)
,mPixelDataSize(0)
,mPaletteIndex(0)
,mIsVFlipped(false)
,mPixelLineSize(0)
,mFormat(ABGR_16_1555_DIRECT_COLOR)
{

}

TinyImage::~TinyImage()
{
	if(mInitIsOK)	
	{
		mInitIsOK=false;
	}
	if(mPixels)
	{
		delete[] mPixels;
	}
	if(mPalette)
	{
		delete[] mPalette;
	}
	if(mPaletteIndex)
	{
		delete[] mPaletteIndex;
	}
}

void		TinyImage::VFlip()
{
	// some image format does not support vflip
	if(	(mFormat==COMPRESSED_4X4_TEXEL) || 
		(mFormat==COMPRESSED_PVRTC) ||
		(mFormat == BC1) ||
		(mFormat == BC2) ||
		(mFormat == BC3) ||
		(mFormat == ETC1) ||
		(mFormat == ETC1A8)
		)
	{
		return;
	}

	mIsVFlipped=!mIsVFlipped;
	
	unsigned char* buffer=new unsigned char[mPixelLineSize];
	
	unsigned char* l1,*l2;
	
	int i;
	for(i=0;i<(mHeight/2);i++)
	{
		l1=mPixels+i*mPixelLineSize;
		l2=mPixels+(mHeight-i-1)*mPixelLineSize;
					
					
		// copy l1 to buffer
		memcpy(buffer,l1,(int)mPixelLineSize);
		// copy l2 to l1
		memcpy(l1,l2,(int)mPixelLineSize);
		// copy buffer to l2
		memcpy(l2,buffer,(int)mPixelLineSize);
	}

	delete[] buffer;

}

//#ifdef WIN32

void	TinyImage::ExportImage(const char* filename,void* data,int width,int height,ImageFormat internalfmt, ImageFileFormat file_format, bool flip)
{
	TinyImage* newpic=NULL;
	switch (file_format)
	{
		case BMP_IMAGE:
			newpic = new BMPClass(data, width, height, internalfmt);
			break;
		case PNG_IMAGE:
			newpic = new PNGClass(data, width, height, internalfmt);
			break;
		case TGA_IMAGE:
			newpic = new TGAClass(data, width, height, internalfmt);
			break;
		default:
			KIGS_ERROR("Unsupported image export format", 0);
	}
	
	if(flip)
		newpic->VFlip();

	newpic->Export(filename);
	delete newpic;
}

// create images from data, to export them
TinyImage::TinyImage(void* data, int sx,int sy,TinyImage::ImageFormat internalfmt, int linesize) : GenericRefCountedBaseClass()
, mUsedWidth(-1)
, mUsedHeight(-1)
{
	mInitIsOK=false;
	// only a few supported formats
	switch(internalfmt)
	{
		case RGBA_32_8888:	
		{
			mInitIsOK = true;
			mWidth = sx;
			mHeight = sy;
			mFormat = RGBA_32_8888;
			mPalette = 0;
			mPaletteDataSize = 0;
			mPixelLineSize = linesize == -1 ? sx * 4 : linesize;
			mPixelDataSize = sy * mPixelLineSize;
			mPixels = new unsigned char[mPixelDataSize];
			memcpy(mPixels, data, mPixelDataSize);
			mPaletteIndex = 0;
			
		}
		break;
		case AI88:
		{
			mInitIsOK = true;
			mWidth = sx;
			mHeight = sy;
			mFormat = AI88;
			mPalette = 0;
			mPaletteDataSize = 0;
			mPixelLineSize = linesize == -1 ? sx * 2 : linesize;
			mPixelDataSize = sy * mPixelLineSize;
			mPixels = new unsigned char[mPixelDataSize];
			memcpy(mPixels, data, mPixelDataSize);
			mPaletteIndex = 0;
		}
		break;
		case RGB_24_888:	
		{
			mInitIsOK = true;
			mWidth = sx;
			mHeight = sy;
			mFormat = RGB_24_888;
			mPalette = 0;
			mPaletteDataSize = 0;
			mPixelLineSize = linesize == -1 ? sx * 3 : linesize;
			mPixelDataSize = sy * mPixelLineSize;
			mPixels = new unsigned char[mPixelDataSize];
			memcpy(mPixels, data, mPixelDataSize);
			mPaletteIndex = 0;
		}
		break;
		case ABGR_16_1555_DIRECT_COLOR:	
		{
			mInitIsOK = true;
			mWidth = sx;
			mHeight = sy;
			mFormat = ABGR_16_1555_DIRECT_COLOR;
			mPalette = 0;
			mPaletteDataSize = 0;
			mPixelLineSize = linesize == -1 ? sx * 2 : linesize;
			mPixelDataSize = sy * mPixelLineSize;
			mPixels = new unsigned char[mPixelDataSize];
			memcpy(mPixels, data, mPixelDataSize);
			mPaletteIndex = 0;
		}
		break;
		case GREYSCALE:	
		{
			mInitIsOK = true;
			mWidth = sx;
			mHeight = sy;
			mFormat = RGB_24_888;
			mPalette = 0;
			mPaletteDataSize = 0;
			mPixels = new unsigned char[sx*sy * 3];
			mPixelDataSize = sx*sy * 3;
			mPixelLineSize = sx * 3;

			// unsigned int greyscale to rgb
			unsigned char* readgrey = (unsigned char*)data;
			unsigned char* writegrey = mPixels;
			int i, j;
			for (j = 0; j < sy; j++)
			{
				for (i = 0; i < sx; i++)
				{
					unsigned char greyval = *readgrey++;
					*writegrey++ = greyval;
					*writegrey++ = greyval;
					*writegrey++ = greyval;
				}
			}


			mPaletteIndex = 0;
		}
		break;
		default:
		{
			KIGS_ERROR("TinyImage with unknown format", 2);
		}
	}
}


//#endif
	
TinyImage*	TinyImage::CreateImage(const char* filename)
{
	auto& pathManager = KigsCore::Singleton<FilePathManager>();
	auto hdl = pathManager->FindFullName(filename);
	return CreateImage(hdl.get());
}


TinyImage*	TinyImage::CreateImage(FileHandle* aFile)
{
	TinyImage* img= NULL;
	
	kstl::string upExtension = ToUpperCase(aFile->mExtension);

	if(upExtension == ".BMP")		img= new BMPClass(aFile);
	else if (upExtension == ".TGA")	img= new TGAClass(aFile);
	else if (upExtension == ".PNG")	img= new PNGClass(aFile);
	else if (upExtension == ".JPG")	img = new JPEGClass(aFile);
	else if (upExtension == ".DDS" || upExtension == ".DDZ" || upExtension == ".DDX")	img = new DDSClass(aFile);
	else if (upExtension == ".ETC" || upExtension == ".ETZ" || upExtension == ".ETX")	img = new ETCClass(aFile);

	if(img && img->mInitIsOK) 
	{
		if(img->isVFlipped())
		{
			img->VFlip();
		}

	}

	
	return img;
}

TinyImage*	TinyImage::CreateImage(void* data, int sx, int sy, TinyImage::ImageFormat internalfmt, int linesize)
{
	return new TinyImage(data, sx, sy, internalfmt, linesize);
}

void	TinyImage::PushContext(const TinyImageLoaderContext& toPush)
{
	if(!mContextStack)
	{
		mContextStack=new kstl::vector<TinyImageLoaderContext>;
	}

	kstl::vector<TinyImageLoaderContext>& currentStack=*mContextStack;
	currentStack.push_back(toPush);
	mLoaderContext=&(currentStack[currentStack.size()-1]);

}

void	TinyImage::PopContext()
{
	if(!mContextStack)
	{
		KIGS_ERROR("TinyImage : Pop context with empty stack",1);
		return;
	}

	kstl::vector<TinyImageLoaderContext>& currentStack=*mContextStack;
	currentStack.pop_back();

	if(currentStack.size())
	{
		mLoaderContext=&(currentStack[currentStack.size()-1]);
	}
	else
	{
		delete mContextStack;
		mContextStack=0;
		mLoaderContext=&TinyImageLoaderContext::GetDefault();
	}
}
	
// nearest pixel resize
bool	TinyImage::FastResize(int newsx, int newsy)
{
	// keep same image format (even if palette)
	// as it's a nearest pixel algorithm, we don't need to interpolate colors or to know the real image format
	
	int pixSize = GetPixelValueSize(mFormat);
	if (pixSize == 0)
	{
		return false;
	}
	if (pixSize < 0)
	{
		pixSize = -pixSize;
	}

	float ratiox = ((float)mWidth) / ((float)newsx);
	float ratioy = ((float)mHeight) / ((float)newsy);

	unsigned char* newdata = new unsigned char[newsx*newsy*pixSize];

	int i, j;

	unsigned char* writeData = newdata;

	for (j = 0; j < newsy; j++)
	{
		int posy =(int) (((float)j)*ratioy+0.49f);

		// line read start
		unsigned char* readData = mPixels + posy*mWidth*pixSize;

		for (i = 0; i < newsx; i++)
		{
			int k;
			int posx = (int)(((float)i)*ratiox+0.49f);
			unsigned char* readPixel = readData + posx*pixSize;
			for (k = 0; k < pixSize; k++)
			{
				*(writeData + k) = *(readPixel + k);
			}
			writeData += pixSize;
		}
	}
	

	mWidth = newsx;
	mHeight = newsy;

	mPixelLineSize = newsx*pixSize;

	mPixelDataSize = mPixelLineSize*mHeight;

	delete[] mPixels;

	mPixels = newdata;

	return true;
}