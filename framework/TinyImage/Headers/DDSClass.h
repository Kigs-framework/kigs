#ifndef _DDSCLASS_H_
#define _DDSCLASS_H_

// base class
#include "TinyImage.h"

/**
* \class DDSClass
* \brief specific DDS class
*/
class DDSClass : public TinyImage
{
public:

	friend class TinyImage;

	/// destructor
	virtual ~DDSClass();

	virtual int	getMipMapCount()
	{
		return myMipmapCount;
	}

protected:
	/**
	* constructor
	* \param filename  the name of the file to load
	*/
	DDSClass(FileHandle* fileName);


	virtual void	Export(const char* filename)
	{
		printf("can not export to DDS\n");
	}


	/**
	* load image from file
	* \param filename the name of the file to load
	*/
	virtual bool	Load(FileHandle* fileName);

	int		myMipmapCount;

	struct DDS_PIXELFORMAT {
		unsigned int dwSize;
		unsigned int dwFlags;
		unsigned int dwFourCC;
		unsigned int dwRGBBitCount;
		unsigned int dwRBitMask;
		unsigned int dwGBitMask;
		unsigned int dwBBitMask;
		unsigned int dwABitMask;
	};

	typedef struct {
		unsigned int             dxgiFormat;
		unsigned int			   resourceDimension;
		unsigned int             miscFlag;
		unsigned int             arraySize;
		unsigned int             reserved;
	} DDS_HEADER_DXT10;

	typedef struct {
		unsigned int           dwSize;
		unsigned int           dwFlags;
		unsigned int           dwHeight;
		unsigned int           dwWidth;
		unsigned int           dwPitchOrLinearSize;
		unsigned int           dwDepth;
		unsigned int           dwMipMapCount;
		unsigned int           dwReserved1[11];
		DDS_PIXELFORMAT		 ddspf;
		unsigned int           dwCaps;
		unsigned int           dwCaps2;
		unsigned int           dwCaps3;
		unsigned int           dwCaps4;
		unsigned int           dwReserved2;
	} DDS_HEADER;

};


#endif //_DDSCLASS_H_
