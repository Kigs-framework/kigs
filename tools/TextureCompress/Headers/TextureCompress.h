#ifndef _TEXTURECOMPRESS_H_
#define _TEXTURECOMPRESS_H_

#include "CoreBaseApplication.h"

class TinyImage;

// DDS structs
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



class ETC1TextureCompress : public CoreBaseApplication
{
public:
	DECLARE_CLASS_INFO(ETC1TextureCompress, CoreBaseApplication, Core)

	ETC1TextureCompress(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~ETC1TextureCompress()
	{
		// Nothing to do here
	}
protected:

	enum	PlatformType
	{
		PLATFORM_ANDROID		=	1,
		PLATFORM_PC				=	2,
	};

	int		m_platform;

	bool	m_SizeOptimize;
	bool	m_ZipExport;
	bool	m_ZstdExport;
	bool	m_ManagedPreAlpha;
	int		m_resizePolicy;

	enum	CompressType
	{
		ETC1		=	0,
		ETC1A4		=	1,
		ETC1A8		=	2,
		DDS			=	3,
		ETC2		=	4,
		ETC2A8		=	5,
	};

	struct ETC_Header
	{
		unsigned int sizex;		// 2 words, if 0xFFFF0000 part is not null then the "contentsizex" is encoded here 
		unsigned int sizey;		// 2 words, if 0xFFFF0000 part is not null then the "contentsizex" is encoded here 
		unsigned int format;	// 2 words, if 0xFFFF0000 part is not null, indicates no alpha sorting (alpha channel fully opaque or fully transparent)
		unsigned int dataSize;
	};

	virtual void	ProtectedInit();
	virtual void	ProtectedUpdate();
	virtual void	ProtectedClose();

	void	RetreiveShortNameAndExt(const kstl::string& filename,kstl::string& shortname,kstl::string& fileext);

	// check only if alpha is needed or not
	int	checkIfNeedsAlpha(TinyImage*);
	int	checkBestMatchingAlpha(TinyImage* toCheck);

	bool	exportDDS(TinyImage*,const kstl::string& outfile,int quality,int perceptual);

	void packInBlock(unsigned int* rgbaread,unsigned int* inBlock,unsigned int width);
	void packInBlockKeepAlpha(unsigned int* rgbaread,unsigned int* inBlock,unsigned int width);
	unsigned char*	export8BitAlpha(unsigned char* data,int sizex,int sizey);
	unsigned char*	export4BitAlpha(unsigned char* data,int sizex,int sizey);
	void			GetRGB24(unsigned char* pixelData,unsigned char* outpixels,unsigned int width,unsigned int height);

	unsigned char*	CopyToPot(TinyImage*,int sizex,int sizey);
	void			FloydSteinberg(unsigned char* data, int sizex,int sizey);

	float* convertToFloatRGBA(unsigned char* input,int sizex,int sizey, bool isRGB,bool useAlpha);
};


#endif //_TEXTURECOMPRESS_H_