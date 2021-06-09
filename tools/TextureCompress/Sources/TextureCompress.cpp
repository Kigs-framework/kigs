#include "TextureCompress.h"
#include "CoreIncludes.h"
#include "TimerIncludes.h"
#include "FilePathManager.h"

#include "MinimalXML.h"
#include "ModuleFileManager.h"

#include "TinyImage.h"
#include "TinyImageLoaderContext.h"

#include "rg_etc1.h"
#include "squish.h"

#include "zlib.h"
#include "ZipUtils.h"
#include "zstd.h"
#include "Etc.h"
#include "EtcColorFloatRGBA.h"

// utility:
unsigned long getNextPOT(unsigned long x)
{
	x = x - 1;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >>16);
	return x + 1;
}


TinyImageLoaderContext	Context2432= {	TinyImage::NO_TEXTURE,								//NO_TEXTURE,
											TinyImage::A3I5_TRANSLUCENT,						//A3I5_TRANSLUCENT	
											TinyImage::RGB_24_888,						//PALETTE16_4_COLOR			(ABGR 1555)
											TinyImage::RGB_24_888,						//PALETTE16_16_COLOR		(ABGR 1555)
											TinyImage::RGB_24_888,						//PALETTE16_256_COLOR		(ABGR 1555)
											TinyImage::COMPRESSED_4X4_TEXEL,					//COMPRESSED_4X4_TEXEL
											TinyImage::A5I3_TRANSLUCENT,						//A5I3_TRANSLUCENT	
											TinyImage::RGBA_32_8888,							//ABGR_16_1555_DIRECT_COLOR
											TinyImage::RGBA_32_8888,							//RGBA_32_8888
											TinyImage::RGB_24_888,								//RGB_24_888
											TinyImage::RGBA_32_8888,							//RGBA_16_4444
											TinyImage::RGBA_32_8888,							//BGRA_16_5551
											TinyImage::RGBA_32_8888,							//RGBA_16_5551
											TinyImage::RGB_24_888,								//RGB_16_565
											TinyImage::COMPRESSED_PVRTC,						//COMPRESSED_PVRTC	
											TinyImage::RGB_24_888,						//PALETTE32_4_COLOR
											TinyImage::RGB_24_888,						//PALETTE32_16_COLOR
											TinyImage::RGB_24_888,						//PALETTE32_256_COLOR
											TinyImage::RGB_24_888,						//PALETTE24_4_COLOR	
											TinyImage::RGB_24_888,						//PALETTE24_16_COLOR
											TinyImage::RGB_24_888,						//PALETTE24_256_COLOR
											TinyImage::GREYSCALE									//GREYSCALE
};

// for android input 
TinyImageLoaderContext	Context32= {	TinyImage::NO_TEXTURE,								//NO_TEXTURE,
											TinyImage::A3I5_TRANSLUCENT,						//A3I5_TRANSLUCENT	
											TinyImage::RGBA_32_8888,						//PALETTE16_4_COLOR			(ABGR 1555)
											TinyImage::RGBA_32_8888,						//PALETTE16_16_COLOR		(ABGR 1555)
											TinyImage::RGBA_32_8888,						//PALETTE16_256_COLOR		(ABGR 1555)
											TinyImage::COMPRESSED_4X4_TEXEL,					//COMPRESSED_4X4_TEXEL
											TinyImage::A5I3_TRANSLUCENT,						//A5I3_TRANSLUCENT	
											TinyImage::RGBA_32_8888,							//ABGR_16_1555_DIRECT_COLOR
											TinyImage::RGBA_32_8888,							//RGBA_32_8888
											TinyImage::RGBA_32_8888,								//RGB_24_888
											TinyImage::RGBA_32_8888,							//RGBA_16_4444
											TinyImage::RGBA_32_8888,							//BGRA_16_5551
											TinyImage::RGBA_32_8888,							//RGBA_16_5551
											TinyImage::RGBA_32_8888,								//RGB_16_565
											TinyImage::COMPRESSED_PVRTC,						//COMPRESSED_PVRTC	
											TinyImage::RGBA_32_8888,						//PALETTE32_4_COLOR
											TinyImage::RGBA_32_8888,						//PALETTE32_16_COLOR
											TinyImage::RGBA_32_8888,						//PALETTE32_256_COLOR
											TinyImage::RGBA_32_8888,						//PALETTE24_4_COLOR	
											TinyImage::RGBA_32_8888,						//PALETTE24_16_COLOR
											TinyImage::RGBA_32_8888,						//PALETTE24_256_COLOR
											TinyImage::GREYSCALE									//GREYSCALE
};

void	usage()
{
	printf("Usage : \n");
	printf("TextureCompress -i inputImagePath [ -o outputImagePath ] [ -c {ETC1|ETC1A4|ETC1A8|DDS|ETC2|ETC2A8|AUTO} ] [ -q {0|1|2} ] \n [ -p {0|1} ] [ -d {ANDROID|PC} ] [ -r {p2|#} ] [ -a ] [ -s ] [ -z ] [ -x ] \n\n");

	printf("-c : ETC1 = RGB compression \n");
	printf("     ETC1A4 = RGBA compression (alpha in 4bpp separate image on Android) \n");
	printf("     ETC1A8 = ETC1 RGB + 8 bit alpha channel image \n");
	printf("     ETC2 = ETC2 RGB image \n");
	printf("     ETC2A8 = ETC2_EAC RGBA image \n");
	printf("     DDS = only available for PC device. Auto choose DXT1, DXT3 or DXT5\n");
	printf("     AUTO = check for alpha channel and output ETC1 / ETC1A4 (default) or DDS if device is PC\n");

	printf("-q : 0 = low-quality(high speed)   (default)\n");
	printf("     1 = middle-quality \n");
	printf("     2 = high-quality \n\n");
	
	printf("-p : \"perceptual mode\"\n");
	printf("     0 = physical noise reduction (default)\n");
	printf("     1 = perceptual noise reduction \n\n");

	printf("-d : \"device\"\n");
	printf("     ANDROID = export classic ETC1 or ETC1A8\n");
	printf("     PC = export DDS or fake ETC1A8 (RGB24 + Alpha 8) for testing \n\n");

	printf("-r : \"resize\"\n");
	printf("     p2 = poweroftwo (default)\n");
	printf("     # = resize on multiple of power of 2 >= # \n");

	printf("-a : for ETC1A4 format, un-premultiply alpha, do floyd steinberg dithering on alpha and premultiply again \n\n");
	printf("-s : activate size optimisation (prefer ETC1 or DXTC1 when possible) \n\n");
	printf("-z : compress (libzip) image and export DDZ (for zipped DDS) or ETZ (for zipped ETC) \n\n");
	printf("-x : compress (zstd) image and export DDX (for zstd DDS) or ETX (for zstd ETC) \n\n");

}

int lastProgressPercent=0;
double progressMultiplicateur=10.0;

void progression(double a)
{
	int progressPercent=(int)(a*progressMultiplicateur);

	if(progressPercent != lastProgressPercent)
	{
		if(progressMultiplicateur>100)
		{
			printf("%f %% compression finished\n", (a * 100));
		}
		else
		{
			printf("%i %% compression finished\n", (int)(a * 100));
		}
		lastProgressPercent=progressPercent;
	}
}

IMPLEMENT_CLASS_INFO(ETC1TextureCompress)

ETC1TextureCompress::ETC1TextureCompress(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreBaseApplication(name, PASS_CLASS_NAME_TREE_ARG)
, m_platform(-1)
, m_SizeOptimize(false)
, m_ZipExport(false)
, m_ZstdExport(false)
, m_ManagedPreAlpha(false) 
, m_resizePolicy(-1)
{ ; }


// return number of bit per alpha pixel (0, 1 or 4)
int		ETC1TextureCompress::checkBestMatchingAlpha(SP<TinyImage> toCheck)
{

	if (toCheck->GetFormat() == TinyImage::RGB_24_888)
	{
		return 0;
	}

	int alphaCount[16];

	memset(alphaCount,0,16*sizeof(int));

	int width,height;
	width=toCheck->GetWidth();
	height=toCheck->GetHeight();

	// create a mask where alpha in [1-14] interval will be put
	// so we will be able to check if the intermediate alpha is just due to scaling or real transparency

	unsigned char*	alphamask=new unsigned char[width*height];

	memset(alphamask,0,width*height*sizeof(unsigned char));

	unsigned int* RGBAread=(unsigned int*)toCheck->GetPixelData();

	unsigned char*	alphawrite=alphamask;

	int pixelcount=width*height;

	int i,j;
	for(j=0;j<height;j++)
	{
		for(i=0;i<width;i++)
		{
			int alpha=((*RGBAread)&0xff000000)>>(4+24);
			alphaCount[alpha]++;

			if((alpha>0)&&(alpha<15))
			{
				*alphawrite=1;
			}
			++alphawrite;
			++RGBAread;
		}
	}

	int result=0;

	if(alphaCount[15] == pixelcount) // all pixels are opaque, no alpha channel
	{
		// nothing more, return result
	}
	else if((alphaCount[15]+alphaCount[0])>((pixelcount*80)/100)) //if mostly opaque and fully transparent pixel, check if 1 bit is OK 
	{
		bool isOnly1Bit=true;
		
		unsigned int notOnly1Bit=0;

		// more than 2% semi transparent not on an edge
		unsigned int limit=((pixelcount*2)/100);
		
		unsigned char* alpharead=alphamask;
		// jump one line
		alpharead+=width;
		for(j=1;j<height-1;j++)
		{
			// jump first pixel on line
			++alpharead;
			for(i=1;i<width-1;i++)
			{
				if(*alpharead) // semi transparent pixel
				{
					// check neighbours
					int test=	alpharead[-1-width]+	alpharead[-width]+		alpharead[-width+1]+
								alpharead[-1]+									alpharead[+1]+
								alpharead[-1+width]+	alpharead[+width]+		alpharead[+width+1];

					if(test>6) 
					{
						notOnly1Bit++;
						if(notOnly1Bit>limit)
						{
							isOnly1Bit=false;
							break;
						}
					}
				}
				++alpharead;
			}
			// jump last pixel on line
			++alpharead;
		}

		if(isOnly1Bit)
		{
			result=1;
		}
		else
		{
			result=4;
		}
	}
	else
	{
		result=4;
	}


	delete[] alphamask;

	return result;
}

// check only if alpha is needed or not
int	ETC1TextureCompress::checkIfNeedsAlpha(SP<TinyImage> toCheck)
{
	int alphaCount[16];

	memset(alphaCount,0,16*sizeof(int));

	int width,height;
	width=toCheck->GetWidth();
	height=toCheck->GetHeight();
	unsigned int* RGBAread=(unsigned int*)toCheck->GetPixelData();

	int pixelcount=width*height;
	int i,j;
	for(j=0;j<height;j++)
	{
		for(i=0;i<width;i++)
		{
			int alpha=((*RGBAread)&0xff000000)>>(4+24);
			alphaCount[alpha]++;
			++RGBAread;
		}
	}

	int result=0;

	if(alphaCount[15] == pixelcount) // all pixels are opaque, no alpha channel
	{
		// nothing more, return result
	}
	else
	{
		result=4;
	}

	return result;
}

unsigned char*	ETC1TextureCompress::CopyToPot(SP<TinyImage> tocopy,int sizex,int sizey)
{
	int width,height;
	width=tocopy->GetWidth();
	height=tocopy->GetHeight();

	int pixelSize=0;

	if(TinyImage::RGBA_32_8888 == tocopy->GetFormat())
	{
		pixelSize=4;
	}
	else if(TinyImage::RGB_24_888 == tocopy->GetFormat())
	{
		pixelSize=3;
	}

	if(pixelSize)
	{
		unsigned char* resultBuffer=new unsigned char[sizex*sizey*pixelSize];
		memset(resultBuffer,0,sizex*sizey*pixelSize*sizeof(unsigned char));

		unsigned char* PixelRead=tocopy->GetPixelData();
		unsigned char* PixelWrite=resultBuffer;

		unsigned int lineSize=width*pixelSize;
		unsigned int potLineSize=sizex*pixelSize;

		int line;
		for(line=0;line<height;line++)
		{
			memcpy(PixelWrite,PixelRead,lineSize);
			for (int endline = width; endline < sizex; endline++)
			{
				for (int pixelcomponent = 0; pixelcomponent < pixelSize; pixelcomponent++)
				{
					PixelWrite[endline*pixelSize+ pixelcomponent] = PixelWrite[(width - 1)*pixelSize+ pixelcomponent];
				}
			}
			PixelWrite+=potLineSize;
			PixelRead+=lineSize;
		}

		for (line = height; line < sizey; line++)
		{
			memcpy(PixelWrite, &resultBuffer[(height-1)*potLineSize], potLineSize);
			PixelWrite += potLineSize;
		}

		return resultBuffer;
	}

	return 0;
}
	
void	ETC1TextureCompress::GetRGB24(unsigned char* pixelData,unsigned char* outpixels,unsigned int width,unsigned int height)
{
	int i,j;

	unsigned char* readRGBA=pixelData;
	unsigned char* writeRGB=outpixels;

	for(j=0;j<height;j++)
	{
		for(i=0;i<width;i++)
		{
			// copy RGB
			writeRGB[0]=readRGBA[0];
			writeRGB[1]=readRGBA[1];
			writeRGB[2]=readRGBA[2];
			readRGBA+=4;
			writeRGB+=3;
		}
	}
}

static const int errorMultCoef[6][4] = { {7,3,5,1}, {5,2,3,1}, {6,3,6,0}, {4,1,4,1},{5,3,2,2},{4,2,1,2}   };

void			ETC1TextureCompress::FloydSteinberg(unsigned char* data, int width,int height)
{
	int i,j;
	unsigned int* RGBAread=(unsigned int*)data;

	// first pass undo-premult
	for(j=0;j<height;j++)
	{
		for(i=0;i<width;i++)
		{
			
			unsigned int alpha=(((*RGBAread)>>24)&0xff)+1;
			unsigned int R=((*RGBAread)>>16)&0xff;
			unsigned int G=((*RGBAread)>>8)&0xff;
			unsigned int B=((*RGBAread))&0xff;
			
			// undo-premult

			R=(R<<8)/alpha;
			if(R>0xFF)
			{
				R=0xFF;
			}
			G=(G<<8)/alpha;
			if(G>0xFF)
			{
				G=0xFF;
			}
			B=(B<<8)/alpha;
			if(B>0xFF)
			{
				B=0xFF;
			}

			// write back
			unsigned int writeback=(((alpha-1)<<24) | (R<<16) | (G<<8) | (B) );

			*RGBAread=writeback;
			++RGBAread;
		}
	}

	RGBAread=(unsigned int*)data;

	for(j=0;j<height;j++)
	{
		for(i=0;i<width;i++)
		{
			int randomArray=rand()%6;
			unsigned int alpha=((*RGBAread)>>24)&0xff;
			unsigned int R=((*RGBAread)>>16)&0xff;
			unsigned int G=((*RGBAread)>>8)&0xff;
			unsigned int B=((*RGBAread))&0xff;

			unsigned int alpha4bit=(alpha&0xf0);
			unsigned int alpha4bitPremult=alpha4bit | 0x0f;
			unsigned int error=alpha&0xf;

			// premult again

			R=(R*alpha4bitPremult)>>8;
			if(R>0xFF)
			{
				R=0xFF;
			}

			G=(G*alpha4bitPremult)>>8;
			if(G>0xFF)
			{
				G=0xFF;
			}
			B=(B*alpha4bitPremult)>>8;
			if(B>0xFF)
			{
				B=0xFF;
			}

			// write back

			unsigned int writeback=((alpha4bit<<24) | (R<<16) | (G<<8) | (B) );

			*RGBAread=writeback;

			// add error on following alpha

			if(i<(width-1))
			{
				//pixel[x+1][y  ] := pixel[x+1][y  ] + 7/16 * erreur_quantification
				unsigned int nextalpha=((*(RGBAread+1))>>24)&0xff;
				nextalpha+=(error*errorMultCoef[randomArray][0])>>4;
				if(nextalpha>0xFF)
				{
					nextalpha=0xFF;
				}
				*(RGBAread+1) = ((*(RGBAread+1))&0x00FFFFFF) | (nextalpha<<24);
			}

			if((i>0)&&(j<(height-1)))
			{
				//pixel[x-1][y+1] := pixel[x-1][y+1] + 3/16 * erreur_quantification
				unsigned int nextalpha=((*(RGBAread-1+width))>>24)&0xff;
				nextalpha+=(error*errorMultCoef[randomArray][1])>>4;
				if(nextalpha>0xFF)
				{
					nextalpha=0xFF;
				}
				*(RGBAread-1+width) = ((*(RGBAread-1+width))&0x00FFFFFF) | (nextalpha<<24);
			}

			if(j<(height-1))
			{
				//pixel[x  ][y+1] := pixel[x  ][y+1] + 5/16 * erreur_quantification
				unsigned int nextalpha=((*(RGBAread+width))>>24)&0xff;
				nextalpha+=(error*errorMultCoef[randomArray][2])>>4;
				if(nextalpha>0xFF)
				{
					nextalpha=0xFF;
				}
				*(RGBAread+width) = ((*(RGBAread+width))&0x00FFFFFF) | (nextalpha<<24);

				if(i<(width-1))
				{
					//pixel[x+1][y+1] := pixel[x+1][y+1] + 1/16 * erreur_quantification
					nextalpha=((*(RGBAread+width+1))>>24)&0xff;
					nextalpha+=(error*errorMultCoef[randomArray][3])>>4;
					if(nextalpha>0xFF)
					{
						nextalpha=0xFF;
					}
					*(RGBAread+width+1) = ((*(RGBAread+width+1))&0x00FFFFFF) | (nextalpha<<24);
				}
			}

			++RGBAread;
		}
	}
}

unsigned char*	ETC1TextureCompress::export8BitAlpha(unsigned char* data,int width,int height)
{

	unsigned char*	alphamask=new unsigned char[width*height];

	memset(alphamask,0,width*height*sizeof(unsigned char));

	unsigned int* RGBAread=(unsigned int*)data;

	unsigned char*	alphawrite=alphamask;

	int i,j;

	for(j=0;j<height;j++)
	{
		for(i=0;i<width;i++)
		{
			unsigned char alpha=((*RGBAread)>>24)&0xff;
			*alphawrite=alpha;
			++alphawrite;
			++RGBAread;
		}
	}
	return alphamask;
}

unsigned char*	ETC1TextureCompress::export4BitAlpha(unsigned char* data,int width,int height)
{

	unsigned char*	alphamask=new unsigned char[width*height/2];

	memset(alphamask,0,(width*height*sizeof(unsigned char))/2);

	unsigned int* RGBAread=(unsigned int*)data;

	unsigned char*	alphawrite=alphamask;

	int i,j;
	int bit=0;
	for(j=0;j<height;j++)
	{
		for(i=0;i<width;i++)
		{
			if(bit)
				*alphawrite|=((*RGBAread)>>28)&0x0F;
			else
				*alphawrite|=((*RGBAread)>>24)&0xF0;
						
			bit=1-bit;
			if(0==bit)
			{
				++alphawrite;
			}
			++RGBAread;
		}
	}
	return alphamask;
}

#define PACKARGBTORGBF(a)	(0xFF000000|a)

void ETC1TextureCompress::packInBlockKeepAlpha(unsigned int* rgbaread,unsigned int* inBlock,unsigned int width)
{
		// rearrange blocks for ETC1 packer

	unsigned int offset=0;

	inBlock[0]=(rgbaread[0+offset]);
	inBlock[1]=(rgbaread[1+offset]);	
	inBlock[2]=(rgbaread[2+offset]);
	inBlock[3]=(rgbaread[3+offset]);
	offset+=width;

	inBlock[4]=(rgbaread[0+offset]);
	inBlock[5]=(rgbaread[1+offset]);	
	inBlock[6]=(rgbaread[2+offset]);
	inBlock[7]=(rgbaread[3+offset]);
	offset+=width;

	inBlock[8]=(rgbaread[0+offset]);
	inBlock[9]=(rgbaread[1+offset]);	
	inBlock[10]=(rgbaread[2+offset]);
	inBlock[11]=(rgbaread[3+offset]);
	offset+=width;

	inBlock[12]=(rgbaread[0+offset]);
	inBlock[13]=(rgbaread[1+offset]);	
	inBlock[14]=(rgbaread[2+offset]);
	inBlock[15]=(rgbaread[3+offset]);
}

void ETC1TextureCompress::packInBlock(unsigned int* rgbaread,unsigned int* inBlock,unsigned int width)
{
	// rearrange blocks for ETC1 packer

	unsigned int offset=0;

	inBlock[0]=PACKARGBTORGBF(rgbaread[0+offset]);
	inBlock[1]=PACKARGBTORGBF(rgbaread[1+offset]);	
	inBlock[2]=PACKARGBTORGBF(rgbaread[2+offset]);
	inBlock[3]=PACKARGBTORGBF(rgbaread[3+offset]);
	offset+=width;

	inBlock[4]=PACKARGBTORGBF(rgbaread[0+offset]);
	inBlock[5]=PACKARGBTORGBF(rgbaread[1+offset]);	
	inBlock[6]=PACKARGBTORGBF(rgbaread[2+offset]);
	inBlock[7]=PACKARGBTORGBF(rgbaread[3+offset]);
	offset+=width;

	inBlock[8]=PACKARGBTORGBF(rgbaread[0+offset]);
	inBlock[9]=PACKARGBTORGBF(rgbaread[1+offset]);	
	inBlock[10]=PACKARGBTORGBF(rgbaread[2+offset]);
	inBlock[11]=PACKARGBTORGBF(rgbaread[3+offset]);
	offset+=width;

	inBlock[12]=PACKARGBTORGBF(rgbaread[0+offset]);
	inBlock[13]=PACKARGBTORGBF(rgbaread[1+offset]);	
	inBlock[14]=PACKARGBTORGBF(rgbaread[2+offset]);
	inBlock[15]=PACKARGBTORGBF(rgbaread[3+offset]);

}

void	ETC1TextureCompress::RetreiveShortNameAndExt(const kstl::string& filename,kstl::string& shortname,kstl::string& fileext)
{
	int pos=(int)filename.rfind("/")+1;
	int pos1=(int)filename.rfind("\\")+1;

	if(pos1 > pos)
	{
		pos=pos1;
	}

	bool	filenameIsShortfilename=false;

	if(pos==0)
	{
		shortname=filename;
		filenameIsShortfilename=true;
	}
	else
	{
		shortname=filename.substr((unsigned int)pos,filename.length()-pos);
	}

	fileext="";
	pos=(int)shortname.rfind(".")+1;
	if(pos)
	{
		fileext.append(shortname,(unsigned int)pos,shortname.length()-pos);
		shortname=shortname.substr(0,pos-1);
	}
}

void	ETC1TextureCompress::ProtectedInit()
{
	// retreive args
	kstl::vector<kstl::string>::iterator itArgs=mArgs.begin();
	// skip app name
	itArgs++;

	int totalArgsCount=mArgs.size()-1;

	if((totalArgsCount<2))
	{
		usage();
		exit(-1);
	}

	int recognizeArgCount=0;

	kstl::string fileNameIn="";
	kstl::string fileNameOut="";

	int	compress=-1;

	
	m_platform=PLATFORM_ANDROID;

	// set compress quality
	int quality = 0;		// 0: low-quality, 1: middle-quality, 2: high-quality
	int perceptual = 0;		// 0: phisical noise reduction, 1: perceptual noise		

	for(;itArgs!=mArgs.end();itArgs++)
	{
		kstl::string& current=(*itArgs);

		if(current[0] == '-')
		{
			char argtype=current[1];
			switch(argtype)
			{
				case 'q':
				{
					itArgs++;
					kstl::string askedq=(*itArgs);
					
					quality=atoi(askedq.c_str());

					recognizeArgCount++;
				}
				break;
				case 'p':
				{
					itArgs++;
					kstl::string askedp=(*itArgs);
					
					perceptual=atoi(askedp.c_str());

					recognizeArgCount++;
				}
				break;
			case 'i':
				{
					// in already there
					if(fileNameIn != "")
					{
						usage();
						exit(-1);
					}
					itArgs++;
					fileNameIn=(*itArgs);
					recognizeArgCount++;
				}
				break;
			case 'o':
				{
					// out already there
					if(fileNameOut != "")
					{
						usage();
						exit(-1);
					}
					itArgs++;
					fileNameOut=(*itArgs);
					recognizeArgCount++;
				}
				break;
			case 'c':
				{
					itArgs++;
					kstl::string askedcompress=(*itArgs);
					if(askedcompress == "ETC1")
					{
						compress=(int)ETC1;
					}
					else if(askedcompress == "ETC1A4")
					{
						compress=(int)ETC1A4;
					}
					else if(askedcompress == "ETC1A8")
					{
						compress=(int)ETC1A8;
					}
					else if(askedcompress == "DDS")
					{
						compress=(int)DDS;
					}
					else if (askedcompress == "ETC2")
					{
						compress = (int)ETC2;
					}
					else if (askedcompress == "ETC2A8")
					{
						compress = (int)ETC2A8;
					}
					else if(askedcompress == "AUTO")
					{
						compress=-1;
					}
					else
					{
						usage();
						exit(-1);
					}
					recognizeArgCount++;
				}
				break;
			case 'd':
				{
					itArgs++;
					kstl::string askedcompress=(*itArgs);
					if(askedcompress == "ANDROID")
					{
						m_platform=PLATFORM_ANDROID;
					}
					else if(askedcompress == "PC")
					{
						m_platform=PLATFORM_PC;
					}
					else
					{
						usage();
						exit(-1);
					}
					recognizeArgCount++;
				}
				break;
			case 'r': // resize policy
			{
				itArgs++;
				kstl::string askedresizeP = (*itArgs);
				if (askedresizeP == "p2")
				{
					m_resizePolicy = -1;
				}
				else
				{
					int resizemultiple = atoi(askedresizeP.c_str());
					
					if (resizemultiple < 4)
					{
						resizemultiple = 4;
					}
					if (resizemultiple > 64)
					{
						resizemultiple = 64;
					}
					m_resizePolicy = getNextPOT(resizemultiple);

				}
				
			}
			break;
			case 's':
				{
					m_SizeOptimize=true;
				}
				break;
			case 'z':
				{
					if (m_ZstdExport == true)
					{
						printf("zip and zstd can not be activated together\n");
						usage();
						exit(-1);
					}
					m_ZipExport=true;
				}
				break;
			case 'x':
			{
				if (m_ZipExport == true)
				{
					printf("zip and zstd can not be activated together\n");
					usage();
					exit(-1);
				}
				m_ZstdExport = true;
			}
			break;
			case 'a':
				{
					m_ManagedPreAlpha=true;
				}
				break;
			}
		}
	}

	// if DDS, platform must be PC
	if( ((int)DDS) == compress) 
	{
		if(m_platform != PLATFORM_PC)
		{
			usage();
			exit(-1);
		}
	}

	// high quality update rate
	if(quality == 1)
	{
		progressMultiplicateur=10.0;
	}
	else if(quality == 2)
	{
		progressMultiplicateur=10.0f;
	}

	kstl::string shortname,ext;
	if(fileNameOut == "")
	{
		RetreiveShortNameAndExt(fileNameIn,shortname,ext);
		fileNameOut=shortname+".etc";
	}
	else // check if fileNameOut is only a path
	{
		RetreiveShortNameAndExt(fileNameOut,shortname,ext);

		if(shortname=="")
		{
			RetreiveShortNameAndExt(fileNameIn,shortname,ext);
			fileNameOut=fileNameOut + shortname+".etc";
		}
	}

	if( ( ((int)DDS) == compress) || ((compress == -1)&& (m_platform==PLATFORM_PC) ) )
	{
		// change extension if needed 
		size_t pos = 0;
		if( (pos = fileNameOut.find(".etc", pos)) != std::string::npos)
		{
			 fileNameOut.replace(pos, 4, ".dds");
		}
	}

	// replace .etc with .etz and .dds with .ddz
	if(m_ZipExport)
	{
		size_t pos = 0;
		if( (pos = fileNameOut.find(".etc", 0)) != std::string::npos)
		{
			 fileNameOut.replace(pos, 4, ".etz");
		}
		else if( (pos = fileNameOut.find(".dds", 0)) != std::string::npos)
		{
			 fileNameOut.replace(pos, 4, ".ddz");
		}
	}

	if (m_ZstdExport)
	{
		size_t pos = 0;
		if ((pos = fileNameOut.find(".etc", 0)) != std::string::npos)
		{
			fileNameOut.replace(pos, 4, ".etx");
		}
		else if ((pos = fileNameOut.find(".dds", 0)) != std::string::npos)
		{
			fileNameOut.replace(pos, 4, ".ddx");
		}
	}

	TinyImage::PushContext(Context32);
	
	SP<TinyImage>	toConvert=TinyImage::CreateImage(fileNameIn.c_str());
	TinyImage::PopContext();
	if(toConvert)
	{
		if(toConvert->IsOK())
		{
#define ETCCOMPRESS_FORMAT_RGBA8 1
#define ETCCOMPRESS_FORMAT_RGB8 2
			int srcFormat=-1;

			int	alphaSortFlag=0;

			// check if src is RGBA_8888 or RGB_888
			if(TinyImage::RGBA_32_8888 == toConvert->GetFormat())
			{
				srcFormat=ETCCOMPRESS_FORMAT_RGBA8;
			}
			else if(TinyImage::RGB_24_888 == toConvert->GetFormat())
			{
				srcFormat=ETCCOMPRESS_FORMAT_RGB8;
				alphaSortFlag=1;
			}

			if(srcFormat != -1)
			{
				int width,height,levels,dstFormat;
				levels=1;
				width=toConvert->GetWidth();
				height=toConvert->GetHeight();

				// get POT sized
				int potSizeX=getNextPOT(width);
				int potSizeY=getNextPOT(height);

				if (m_resizePolicy>=4)
				{
					potSizeX = (width + m_resizePolicy - 1) / m_resizePolicy;
					potSizeY = (height + m_resizePolicy - 1) / m_resizePolicy;
					potSizeX *= m_resizePolicy;
					potSizeY *= m_resizePolicy;
				}

				int	ContentSizeX=0;
				int	ContentSizeY=0;

				if(compress == -1)	// auto format
				{
					if(m_platform == PLATFORM_PC) 
					{
						compress=(int)DDS;
					}
					else if(srcFormat==ETCCOMPRESS_FORMAT_RGB8)
					{
						compress=(int)ETC1;
					}
					else // RGBA32
					{
						// check how much bit is needed for alpha channel
						int needs=checkBestMatchingAlpha(toConvert);
						if(needs==0)
						{
							compress=(int)ETC1;
							alphaSortFlag=1;
						}
						else
						{
							if(needs==1)
							{
								alphaSortFlag=1;
							}

							if(m_platform == PLATFORM_ANDROID) //only ETC1A8 on android at the moment
							{
								compress=(int)ETC1A8;
							}
							else
							{
								compress=(int)ETC1A4;
							}
						}
					}
				}

				// go to DDS export
				if( ((int)DDS) == compress)
				{
					int potSizeX=width;
					int potSizeY=height;

					if(!exportDDS(toConvert,fileNameOut,quality,perceptual))
					{
						printf("Error exporting DDS image\n");
						usage();
					}
				}
				else if( (((int)ETC1A8)==compress) &&  (m_platform == PLATFORM_PC) )// fake ETC1A8 for PC
				{
					int		etcLength;
					unsigned char*	pixelData=toConvert->GetPixelData();
					ContentSizeX=0;
					ContentSizeY=0;

					etcLength=(width*height)*3;
					unsigned char* outpixels = new unsigned char[etcLength];

					GetRGB24(pixelData,outpixels,width,height);

					ETC_Header	header;
					header.sizex=(width&0xFFFF);
					header.sizey=(height&0xFFFF);
					header.format=(compress&0xFFFF) | (alphaSortFlag<<16);
					header.dataSize=etcLength;

						
					// add alpha data size
					header.dataSize+=width*height;

					auto fp = Platform_fopen(fileNameOut.c_str(), "wb");
					Platform_fwrite(&header, sizeof(ETC_Header), 1, fp.get());

					if(m_ZipExport)
					{
						// create a buffer for the whole package
						unsigned char* tozip=new unsigned char[etcLength + width*height];
						memcpy(tozip,outpixels,etcLength);
						unsigned char*	toexport=export8BitAlpha(pixelData,width,height);
						memcpy(tozip+etcLength,toexport, width*height);
						delete[] toexport;

						unsigned char * outbuffer;
						unsigned int	outLength;
						ZipUtils::DeflateMemory(tozip,etcLength + width*height,&outbuffer,outLength);
						Platform_fwrite(outbuffer, outLength, 1, fp.get());
						Platform_fflush(fp.get());
						delete[] outbuffer;

						delete[] tozip;
					}
					else if (m_ZstdExport)
					{
						unsigned char* tozip = new unsigned char[etcLength + width * height];
						memcpy(tozip, outpixels, etcLength);
						unsigned char*	toexport = export8BitAlpha(pixelData, width, height);
						memcpy(tozip + etcLength, toexport, width*height);
						delete[] toexport;

						size_t	good_size = ZSTD_compressBound(etcLength + width * height);

						std::vector<u8> result;

						result.resize(good_size);

						size_t compressedSize = ZSTD_compress(result.data(), good_size, tozip, etcLength + width * height, 80);
						result.resize(compressedSize);

						if (!ZSTD_isError(compressedSize))
						{
							Platform_fwrite(result.data(), compressedSize, 1, fp.get());
							Platform_fflush(fp.get());
						}

						delete[] tozip;
					}
					else
					{
						Platform_fwrite(outpixels, etcLength, 1, fp.get());
						unsigned char*	toexport=export8BitAlpha(pixelData,width,height);
						Platform_fwrite(toexport, width*height, 1, fp.get());
						delete[] toexport;
					}

					Platform_fclose(fp.get());

					delete[] outpixels;
				}
				else	// export ETC
				{
					unsigned char*	pixelData=toConvert->GetPixelData();
					bool			needsPixelDataDelete=false;

					if((potSizeX != width) || (potSizeY != height))
					{
						//Create a POT sized Buffer and copy data 
						ContentSizeX=width;
						ContentSizeY=height;
						width=potSizeX;
						height=potSizeY;

						pixelData=CopyToPot(toConvert,potSizeX,potSizeY);

						needsPixelDataDelete=true;
					}

					// output
					int		psize, plevels,etcLength,ret;
					char*	outpixels=0;

					
					// Android
					{
						if ((compress == ETC2) || (compress == ETC2A8))
						{

							int bits = checkBestMatchingAlpha(toConvert);

							float* convertedToFloat = convertToFloatRGBA(pixelData, width, height, toConvert->GetFormat() == TinyImage::RGB_24_888, bits > 0);

							Etc::ErrorMetric errormetric = Etc::ErrorMetric::NUMERIC;

							if (perceptual != 0)
							{
								errormetric = Etc::ErrorMetric::REC709;
							}

							Etc::Image image(convertedToFloat,	width, height, errormetric);
							image.m_bVerboseOutput = true;

							Etc::Image::Format fmt = Etc::Image::Format::RGB8;
							int blockSize = 8;

							if (m_SizeOptimize) // auto choose ETC2 / ETC2A8
							{
								if (bits > 0)
								{
									fmt = Etc::Image::Format::RGBA8;
									blockSize = 16;
									compress = ETC2A8;
								}
								else
								{
									compress = ETC2;
								}
							}
							else if(compress == ETC2A8)
							{
								fmt = Etc::Image::Format::RGBA8;
								blockSize = 16;
							}

							float effort = ETCCOMP_DEFAULT_EFFORT_LEVEL;

							if (quality == 0)
							{
								effort = ETCCOMP_MIN_EFFORT_LEVEL;
							}
							else if (quality == 2)
							{
								effort = ETCCOMP_MAX_EFFORT_LEVEL;
							}

							
							Etc::Image::EncodingStatus encStatus = Etc::Image::EncodingStatus::SUCCESS;

							encStatus = image.Encode(fmt, errormetric, effort, 2, 1024);
							
							ret = 1;
							plevels = 1;

							psize = (width / 4) * (height / 4) * blockSize;

							etcLength = image.GetEncodingBitsBytes();

							outpixels = new char[etcLength];

							memcpy(outpixels, image.GetEncodingBits(),image.GetEncodingBitsBytes());

							delete[] convertedToFloat;
						}
						else // ETC1
						{
							int bits = checkBestMatchingAlpha(toConvert);

							float* convertedToFloat = convertToFloatRGBA(pixelData, width, height, toConvert->GetFormat() == TinyImage::RGB_24_888, bits > 0);

							Etc::ErrorMetric errormetric = Etc::ErrorMetric::NUMERIC;

							if (perceptual != 0)
							{
								errormetric = Etc::ErrorMetric::REC709;
							}

							Etc::Image image(convertedToFloat, width, height, errormetric);
							image.m_bVerboseOutput = true;

							Etc::Image::Format fmt = Etc::Image::Format::ETC1;
							int blockSize = 8;

							if (m_SizeOptimize) // auto choose ETC1 / ETC1A8
							{
								if (bits > 0)
								{
									compress = ETC1A8;
								}
								else
								{
									compress = ETC1;
								}
							}

							float effort = ETCCOMP_DEFAULT_EFFORT_LEVEL;

							if (quality == 0)
							{
								effort = ETCCOMP_MIN_EFFORT_LEVEL;
							}
							else if (quality == 2)
							{
								effort = ETCCOMP_MAX_EFFORT_LEVEL;
							}


							Etc::Image::EncodingStatus encStatus = Etc::Image::EncodingStatus::SUCCESS;

							encStatus = image.Encode(fmt, errormetric, effort, 8, 1024);

							ret = 1;
							plevels = 1;

							psize = (width / 4) * (height / 4) * blockSize;

							etcLength = image.GetEncodingBitsBytes();

							outpixels = new char[etcLength];

							memcpy(outpixels, image.GetEncodingBits(), image.GetEncodingBitsBytes());

							delete[] convertedToFloat;

						}

					}

					if (ret != 0 && plevels == levels && psize == etcLength)
					{
						ETC_Header	header;
						header.sizex=((ContentSizeX&0xFFFF)<<16)|(width&0xFFFF);
						header.sizey=((ContentSizeY&0xFFFF)<<16)|(height&0xFFFF);
						header.format=(compress&0xFFFF) | (alphaSortFlag<<16);
						header.dataSize=etcLength;

						if((int)ETC1A8 == compress)
						{
							// add alpha data size
							header.dataSize+=width*height;
						}
						else if(((int)ETC1A4 == compress)) // android has no ETC1A4 internaly
						{
							header.dataSize+=width*height/2;
						}

						auto fp = Platform_fopen(fileNameOut.c_str(), "wb");
						Platform_fwrite(&header, sizeof(ETC_Header), 1, fp.get());

						if(m_ZipExport)
						{
							// create a buffer for the whole package
							int extrachannelsize=0;
							if((int)ETC1A8 == compress)
							{
								extrachannelsize=width*height;
							}
							else if(((int)ETC1A4 == compress)) // android has no ETC1A4 internaly
							{
								extrachannelsize=width*height/2;
							}

							unsigned char* tozip=new unsigned char[psize + extrachannelsize];
							memcpy(tozip,outpixels,psize);

							if((int)ETC1A8 == compress)
							{
								unsigned char*	toexport=export8BitAlpha(pixelData,width,height);
								memcpy(tozip+psize,toexport,extrachannelsize);
								delete[] toexport;
							}
							else if(((int)ETC1A4 == compress) ) // android has no ETC1A4 internaly
							{
								unsigned char*	toexport=export4BitAlpha(pixelData,width,height);
								memcpy(tozip+psize,toexport,extrachannelsize);
								delete[] toexport;
							}

							unsigned char * outbuffer;
							unsigned int	outLength;
							ZipUtils::DeflateMemory(tozip,psize + extrachannelsize,&outbuffer,outLength);
							Platform_fwrite(outbuffer, outLength, 1, fp.get());
							Platform_fflush(fp.get());
							delete[] outbuffer;
							delete[] tozip;
						}
						else if (m_ZstdExport) 
						{
							// create a buffer for the whole package
							int extrachannelsize = 0;
							if ((int)ETC1A8 == compress)
							{
								extrachannelsize = width * height;
							}
							else if (((int)ETC1A4 == compress)) // android has no ETC1A4 internaly
							{
								extrachannelsize = width * height / 2;
							}

							unsigned char* tozip = new unsigned char[psize + extrachannelsize];
							memcpy(tozip, outpixels, psize);

							if ((int)ETC1A8 == compress)
							{
								unsigned char*	toexport = export8BitAlpha(pixelData, width, height);
								memcpy(tozip + psize, toexport, extrachannelsize);
								delete[] toexport;
							}
							else if (((int)ETC1A4 == compress)) // android has no ETC1A4 internaly
							{
								unsigned char*	toexport = export4BitAlpha(pixelData, width, height);
								memcpy(tozip + psize, toexport, extrachannelsize);
								delete[] toexport;
							}


							size_t	good_size = ZSTD_compressBound(psize + extrachannelsize);

							std::vector<u8> result;

							result.resize(good_size);

							size_t compressedSize = ZSTD_compress(result.data(), good_size, tozip, psize + extrachannelsize, 80);
							result.resize(compressedSize);

							if (!ZSTD_isError(compressedSize))
							{
								Platform_fwrite(result.data(), compressedSize, 1, fp.get());
								Platform_fflush(fp.get());
							}
							delete[] tozip;
						}
						else
						{

							Platform_fwrite(outpixels, psize, 1, fp.get());

							if((int)ETC1A8 == compress)
							{
								unsigned char*	toexport=export8BitAlpha(pixelData,width,height);
								Platform_fwrite(toexport, width*height, 1, fp.get());
								delete[] toexport;
							}
							else if(((int)ETC1A4 == compress) ) // android has no ETC1A4 internaly
							{
								unsigned char*	toexport=export4BitAlpha(pixelData,width,height);
								Platform_fwrite(toexport, width*height/2, 1, fp.get());
								delete[] toexport;
							}
						}

						Platform_fclose(fp.get());

					}
					else
					{
						printf("error during compression\n");
						usage();
						if(needsPixelDataDelete)
						{
							delete[] pixelData;
						}
						if(outpixels)
							delete[] outpixels;
						exit(-1);
					}

					if(needsPixelDataDelete)
					{
						delete[] pixelData;
					}
					if(outpixels)
						delete[] outpixels;
				}
			}
			else
			{
				printf("input must be RGBA_8888 or RGB_888\n");
				usage();
				exit(-1);
			}
		}
	}

}

const unsigned int FCC_DXT1=*(unsigned int*)"DXT1";
const unsigned int FCC_DXT3=*(unsigned int*)"DXT3";
const unsigned int FCC_DXT5=*(unsigned int*)"DXT5";

bool	ETC1TextureCompress::exportDDS(SP<TinyImage> toexport,const kstl::string& outfile,int quality,int perceptual)
{
	int width,height,dstFormat;
				
	width=toexport->GetWidth();
	height=toexport->GetHeight();

	int bits = 0;

	if (toexport->GetFormat() == TinyImage::RGB_24_888) // change to RGBA_32_8888
	{
		int sx = toexport->GetWidth();
		int sy = toexport->GetHeight();

		unsigned int*	newrgbaBuffer = new unsigned int[sx*sy];
		unsigned char*  inBuffer = toexport->GetPixelData();
		unsigned int*   outBuffer = newrgbaBuffer;
		for (int py = 0; py < sy; py++)
		{
			for (int px = 0; px < sx; px++)
			{
				int R = inBuffer[0];
				int G = inBuffer[1];
				int B = inBuffer[2];

				outBuffer[0] = R | (G << 8) | (B << 16) | (0xFF<<24);

				inBuffer += 3;
				outBuffer++;

			}
		}
		toexport = TinyImage::CreateImage(newrgbaBuffer, sx, sy, TinyImage::RGBA_32_8888);
		delete[] newrgbaBuffer;

	}
	else
	{
		bits = checkBestMatchingAlpha(toexport);
	}

	

	int Length=(width*height);
	int blkSize=16;
	dstFormat=squish::kDxt5;
	if((bits == 0) || ((bits<4) && m_SizeOptimize))
	{
		dstFormat=squish::kDxt1;
		Length=(width*height)/2;
		blkSize=8;
	}
	else if(bits<4)
	{
		dstFormat=squish::kDxt3;
	}

	unsigned int*	inBlock=new unsigned int[16]; // 4x4 32bits

	unsigned char* outpixels = new unsigned char[Length];
	unsigned char* outblock=outpixels;

	unsigned int*	rgbaBuffer=(unsigned int*)toexport->GetPixelData();

	float oneOnBlockCount=1.0f/((float)((width/4)*(height/4)));
	float currentBlockIndex=0;

	// set quality
	switch(quality)
	{
	case 1:
		{
			dstFormat|=squish::kColourClusterFit;
		}
		break;
	case 2:
		{
			dstFormat|=squish::kColourIterativeClusterFit;
		}
		break;
	case 0:
	default:
		{
			dstFormat|=squish::kColourRangeFit;
		}
		break;
	}

	// set perceptual
	if(perceptual == 0)
	{
		dstFormat|=squish::kColourMetricUniform;
	}
	else
	{
		dstFormat|=squish::kColourMetricPerceptual;
	}

	int blocki,blockj;
	for(blockj=0;blockj<(height/4);blockj++)
	{
		unsigned int*	rgbaread=rgbaBuffer+(blockj*4)*width;

		for(blocki=0;blocki<(width/4);blocki++)
		{
			packInBlockKeepAlpha(rgbaread,inBlock,width);					
			squish::Compress((unsigned char*) inBlock, outblock, dstFormat );
			outblock+=blkSize;
			rgbaread+=4; // jump 4 rgba 32bpp pixels

			++currentBlockIndex;
			progression(currentBlockIndex*oneOnBlockCount);
		}
	}

	delete[] inBlock;

	auto fp = Platform_fopen(outfile.c_str(), "wb");
	if(fp)
	{
		DDS_HEADER header;

		memset(&header,0,sizeof(DDS_HEADER));

		header.dwSize=124;
		header.dwFlags=0x81007; // mandatory flags
		header.dwHeight=height;
		header.dwWidth=width;
		header.dwPitchOrLinearSize = Length; // for compressed texture, the full texture size
		header.dwCaps = 0x1000;	// means texture
		header.ddspf.dwSize = 32;
		header.ddspf.dwFlags = 0x4; // contains compressed data with format in fourCC

		if(dstFormat & squish::kDxt1)
		{
			header.ddspf.dwFourCC =FCC_DXT1;
		}
		else if(dstFormat & squish::kDxt3)
		{
			header.ddspf.dwFourCC =FCC_DXT3;
		}
		else
		{
			header.ddspf.dwFourCC =FCC_DXT5;
		}

		unsigned int prehead=*(unsigned int*)"DDS ";
		// pre header ?
		Platform_fwrite(&prehead, sizeof(unsigned int), 1, fp.get());

		Platform_fwrite(&header, sizeof(DDS_HEADER), 1, fp.get());

		// compress only data
		if(m_ZipExport)
		{
			unsigned char * outbuffer;
			unsigned int	outLength;
			ZipUtils::DeflateMemory(outpixels,Length,&outbuffer,outLength);
			Platform_fwrite(outbuffer, outLength, 1, fp.get());
			Platform_fflush(fp.get());
			delete[] outbuffer;
		}
		else if (m_ZstdExport) 
		{
			size_t	good_size = ZSTD_compressBound(Length);

			std::vector<u8> result;

			result.resize(good_size);

			size_t compressedSize = ZSTD_compress(result.data(), good_size, outpixels, Length, 80);
			result.resize(compressedSize);

			if (!ZSTD_isError(compressedSize))
			{
				Platform_fwrite(result.data(), compressedSize, 1, fp.get());
				Platform_fflush(fp.get());
			}
		}
		else
		{
			Platform_fwrite(outpixels, Length, 1, fp.get());
		}
		Platform_fclose(fp.get());
	}

	delete[] outpixels;

	return true;
}


void	ETC1TextureCompress::ProtectedUpdate()
{
	mNeedExit=true;
}

void	ETC1TextureCompress::ProtectedClose()
{

}
float* ETC1TextureCompress::convertToFloatRGBA(unsigned char* input, int sizex, int sizey, bool isRGB, bool useAlpha)
{
	Etc::ColorFloatRGBA* m_pafrgbaPixels = new Etc::ColorFloatRGBA[sizex * sizey];
	

	int iBytesPerPixel = 4;

	if (isRGB)
	{
		iBytesPerPixel = 3;
	}

	unsigned char *pucPixel;	// = &paucPixels[(iBlockY * iWidth + iBlockX) * iBytesPerPixel];
	Etc::ColorFloatRGBA *pfrgbaPixel = m_pafrgbaPixels;

	// convert pixels from RGBA* to ColorFloatRGBA
	for (unsigned int uiV = 0; uiV < sizey; ++uiV)
	{
		// reset coordinate for each row
		pucPixel = &input[(uiV * sizex ) * iBytesPerPixel];

		// read each row
		for (unsigned int uiH = 0; uiH <  sizex; ++uiH)
		{
			
			unsigned char alpha;
			if (isRGB || (!useAlpha))
			{
				alpha = 255;
			}
			else
			{
				alpha = pucPixel[3];
			}

			*pfrgbaPixel++ = Etc::ColorFloatRGBA::ConvertFromRGBA8(pucPixel[0], pucPixel[1],pucPixel[2], alpha);
			

			pucPixel += iBytesPerPixel;
		}
	}

	return (float*)m_pafrgbaPixels;
}