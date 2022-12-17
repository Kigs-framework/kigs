#ifndef _TINYIMAGE_H_
#define _TINYIMAGE_H_

#include "TinyImageUtils.h"
#include "Core.h"

/*! \defgroup TinyImageModule 
 *  Picture file management.
 */


class TinyImageLoaderContext;
class FileHandle;

typedef void (*ReadColorFunc)(unsigned char* src, unsigned char* dst);

// ****************************************
// * TinyImage class
// * --------------------------------------
/**
* \file	TinyImage.h
* \class	TinyImage
* \ingroup TinyImageModule
* \brief Base class for picture file loading.
*
*/
// ****************************************

class TinyImage : public GenericRefCountedBaseClass
{
public:	
	

	/**
	* \enum ImageFormat
	* \brief enumeration of supported texture format
	*/
	enum ImageFormat
	{
		NO_TEXTURE	=0,
		A3I5_TRANSLUCENT,		// NDS specific
		PALETTE16_4_COLOR,	
		PALETTE16_16_COLOR,
		PALETTE16_256_COLOR,
		COMPRESSED_4X4_TEXEL,	// NDS specific
		A5I3_TRANSLUCENT,		// NDS specific
		ABGR_16_1555_DIRECT_COLOR,
		RGBA_32_8888,
		RGB_24_888,
		RGBA_16_4444,
		BGRA_16_5551,
		RGBA_16_5551,
		RGB_16_565,
		COMPRESSED_PVRTC,		//IPHONE specific
		PALETTE32_4_COLOR,	
		PALETTE32_16_COLOR,
		PALETTE32_256_COLOR,
		PALETTE24_4_COLOR,	
		PALETTE24_16_COLOR,
		PALETTE24_256_COLOR,
		GREYSCALE,
		ALPHA_8,
		AI88,
		// DDS format
		//
		BC1,
		BC2,
		BC3,
		// ETC format
		ETC1,
		ETC1A8,
		ETC1A4,
		ETC2,
		ETC2A8,
		SUPPORTED_FORMAT_COUNT
	};

	enum ImageFileFormat
	{
		BMP_IMAGE = 0,
		PNG_IMAGE,
		TGA_IMAGE,
		DDS_IMAGE,
		JPG_IMAGE,
		ETC_IMAGE
	};


	TinyImage();
	TinyImage(void* data, int sx, int sy, TinyImage::ImageFormat internalfmt, int linesize = -1);
	virtual ~TinyImage();

	// negative valeur is for division : -1 =1/2 -2=1/4
	// 0 is invalid
	static	inline unsigned int	GetPixelValueSize(ImageFormat fmt)
	{
		switch((int)fmt)
		{
		case ((int)NO_TEXTURE):
			{
				return 0;
			}

		case ((int)A3I5_TRANSLUCENT):
			{
				return 2;
			}
		case ((int)PALETTE16_4_COLOR):
			{
				return (unsigned int)-2;
			}
		case ((int)PALETTE16_16_COLOR):
			{
				return (unsigned int)-1;
			}
		case ((int)PALETTE16_256_COLOR):
			{
				return 1;
			}
		case ((int)COMPRESSED_4X4_TEXEL):
			{
				return (unsigned int)-2;
			}
		case ((int)A5I3_TRANSLUCENT):
			{
				return 2;
			}
		case ((int)ABGR_16_1555_DIRECT_COLOR):
			{
				return 2;
			}
		case ((int)RGBA_32_8888):
			{
				return 4;
			}
		case ((int)RGB_24_888):
			{
				return 3;
			}
		case ((int)RGBA_16_4444):
			{
				return 2;
			}
		case ((int)BGRA_16_5551):
			{
				return 2;
			}
		case ((int)RGBA_16_5551):
			{
				return 2;
			}
		case ((int)RGB_16_565):
			{
				return 2;
			}
		case ((int)COMPRESSED_PVRTC): // TODO ?
			{
				return (unsigned int)-2;
			}
		case ((int)PALETTE32_4_COLOR):
			{
				return (unsigned int)-2;
			}
		case ((int)PALETTE32_16_COLOR):
			{
				return (unsigned int)-1;
			}
		case ((int)PALETTE32_256_COLOR):
			{
				return 1;
			}
		case ((int)PALETTE24_4_COLOR):
			{
				return (unsigned int)-2;
			}
		case ((int)PALETTE24_16_COLOR):
			{
				return (unsigned int)-1;
			}
		case ((int)PALETTE24_256_COLOR):
			{
				return 1;
			}
		case ((int)GREYSCALE):
			{
				return 1;
			}
		case ((int)ALPHA_8):
		{
			return 1;
		}
		case ((int)AI88):
			{
				return 2;
			}
		}
		return 0;
	}

	static	inline unsigned int	GetPaletteValueSize(ImageFormat fmt)
	{
		switch((int)fmt)
		{
		case ((int)NO_TEXTURE):
			{
				return 0;
			}

		case ((int)A3I5_TRANSLUCENT):
			{
				return 0;
			}
		case ((int)PALETTE16_4_COLOR):
			{
				return 2;
			}
		case ((int)PALETTE16_16_COLOR):
			{
				return 2;
			}
		case ((int)PALETTE16_256_COLOR):
			{
				return 2;
			}
		case ((int)COMPRESSED_4X4_TEXEL):
			{
				return 2;
			}
		case ((int)A5I3_TRANSLUCENT):
			{
				return 0;
			}
		case ((int)ABGR_16_1555_DIRECT_COLOR):
			{
				return 0;
			}
		case ((int)RGBA_32_8888):
			{
				return 0;
			}
		case ((int)RGB_24_888):
			{
				return 0;
			}
		case ((int)RGBA_16_4444):
			{
				return 0;
			}
		case ((int)BGRA_16_5551):
			{
				return 0;
			}
		case ((int)RGBA_16_5551):
			{
				return 0;
			}
		case ((int)RGB_16_565):
			{
				return 0;
			}
		case ((int)COMPRESSED_PVRTC): // TODO ?
			{
				return 0;
			}
		case ((int)PALETTE32_4_COLOR):
			{
				return 4;
			}
		case ((int)PALETTE32_16_COLOR):
			{
				return 4;
			}
		case ((int)PALETTE32_256_COLOR):
			{
				return 4;
			}
		case ((int)PALETTE24_4_COLOR):
			{
				return 3;
			}
		case ((int)PALETTE24_16_COLOR):
			{
				return 3;
			}
		case ((int)PALETTE24_256_COLOR):
			{
				return 3;
			}
		case ((int)GREYSCALE):
			{
				return 0;
			}
		case ((int)ALPHA_8):
			{
				return 0;
			}
		}
		return 0;
	}

	virtual int	getMipMapCount()
	{
		return 0;
	}
	
	static	SP<TinyImage>	CreateImage(const char* filename);
	static	SP<TinyImage>	CreateImage(FileHandle* filename);
	static	SP<TinyImage>	CreateImage(void* data, int sx, int sy, TinyImage::ImageFormat internalfmt, int linesize = -1);
	static void		ExportImage(const char* filename, void* data, int width, int height, ImageFormat internalfmt, ImageFileFormat file_format, bool flip = true);

	bool	IsOK()
	{
		return mInitIsOK;
	}
	
	int		GetWidth() const {return mWidth;}
	int		GetHeight() const  {return mHeight;}
	int		GetUsedWidth() const { return (mUsedWidth==-1)?mWidth: mUsedWidth; }
	int		GetUsedHeight() const { return (mUsedHeight == -1) ? mHeight : mUsedHeight; }

	unsigned int GetPixelLineSize()
	{
		return mPixelLineSize;
	}

	unsigned int GetPixelDataSize()
	{
		return mPixelDataSize;
	}

	unsigned int GetPaletteDataSize()
	{
		return mPaletteDataSize;
	}

	unsigned char* GetPixelData()
	{
		return (unsigned char*)mPixels;
	}
	
	unsigned char* GetPaletteData()
	{
		return (unsigned char*)mPalette;
	}
	
	unsigned char* GetPaletteIndexData()
	{
		return (unsigned char*)mPaletteIndex;
	}
	
	void	VFlip();
	
	bool	isVFlipped()
	{
		return mIsVFlipped;
	}
	
	ImageFormat GetFormat() 
	{
		return mFormat;
	}
	
	static void	PushContext(const TinyImageLoaderContext& toPush);
	static void PopContext();
	
	bool	FastResize(int newsx, int newsy);
	virtual void	Export(const char* filename)
	{
		// can not be called on base
		KIGS_ASSERT(0);
	}
protected:	
	

	virtual bool	Load(FileHandle* fileName)
	{
		// can not be called on base
		KIGS_ASSERT(0);

		return false;
	}
	
	bool			mInitIsOK;
	
	int				mWidth, mHeight;
	int				mUsedWidth, mUsedHeight;
	unsigned char*	mPalette;
	unsigned int	mPaletteDataSize;
	unsigned char*	mPixels;
	unsigned int	mPixelDataSize;
	
	// only for 4x4 compressed textures
	unsigned short* mPaletteIndex;
	
	bool			mIsVFlipped;
	
	unsigned int	mPixelLineSize;
	
	TinyImage::ImageFormat		mFormat;

	static			TinyImageLoaderContext* mLoaderContext;

	static std::vector<TinyImageLoaderContext>*	mContextStack;
	
};


#endif //_TINYIMAGE_H_

