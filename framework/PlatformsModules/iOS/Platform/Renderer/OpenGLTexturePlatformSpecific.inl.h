
#include "Platform/Core/PlatformCore.h"


bool 	OpenGLTexture::initWithETCImage(FileHandle * aFile)
{
	bool L_Result = false;	
	unsigned long L_filelength = 0;
	CoreRawBuffer* L_RawBuffer = ModuleFileManager::LoadFile(aFile, L_filelength);
	if(L_RawBuffer)
	{
		myTextureType = TEXTURE_2D;
		struct ETC_Header
		{
			unsigned int	sizex;	// if sizex&0xFFFF0000	=> content size
			unsigned int	sizey;  // if sizey&0xFFFF0000	=> content size
			unsigned int	format;
			unsigned int	datasize;
		};
	
		//Read Header
		ETC_Header*	imageheader=(ETC_Header*)L_RawBuffer->buffer();

		//int dataSize=L_filelength-sizeof(ETC_Header);
		int L_DataLen=imageheader->datasize;
		
		//bool L_bPreMulti = true;
		unsigned char* imgdata=(unsigned char*)L_RawBuffer->buffer();
		imgdata+=sizeof(ETC_Header);
		unsigned char* L_pData = new unsigned char[L_DataLen];
		memcpy(L_pData, imgdata, L_DataLen);
		myPow2Width   = imageheader->sizex&0xFFFF;
		myPow2Height  = imageheader->sizey&0xFFFF;

		myWidth = (short)myPow2Width;
		myHeight = (short)myPow2Height;
		
		if((imageheader->sizex & 0xFFFF0000) || (imageheader->sizey & 0xFFFF0000))
		{
			myWidth = ((imageheader->sizex >> 16)& 0xFFFF);
			myHeight = ((imageheader->sizey >> 16)& 0xFFFF);
		}

		// compressed
		//int L_nBitsPerComponent = 0;

		bool L_bHasAlpha = false;
		unsigned int L_Format = (imageheader->format & 0xFFFF);
		if (2 == L_Format) // ETC1A8
			L_bHasAlpha = true;
		
		if(L_bHasAlpha)
			glGenTextures(1, &myIDETCAlphaTexture);
						/*Set texture Param*/
		glBindTexture( GL_TEXTURE_2D, myTextureGLIndex );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

		if(myIDETCAlphaTexture) // if alpha texture, apply same params
		{
			glBindTexture( GL_TEXTURE_2D, myIDETCAlphaTexture );
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		}
		

		unsigned int L_TotalSize = (myPow2Width*myPow2Height)/2;
		if(!L_bHasAlpha) // ETC1
		{
			glBindTexture(GL_TEXTURE_2D, myTextureGLIndex);
            #ifndef __APPLE__
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, (GLsizei)myPow2Width, (GLsizei)myPow2Height, 0,L_TotalSize, L_pData);
            #endif
			myTransparencyType = 0;
		}
		else//ETC1A8
		{
			glBindTexture(GL_TEXTURE_2D, myTextureGLIndex);
            #ifndef __APPLE__
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, (GLsizei)myPow2Width, (GLsizei)myPow2Height, 0,L_TotalSize, L_pData);
            #endif
			glBindTexture(GL_TEXTURE_2D, myIDETCAlphaTexture);
			unsigned char* alphastart=(unsigned char*)L_pData;
			alphastart += L_TotalSize;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, (GLsizei)myPow2Width, (GLsizei)myPow2Height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, alphastart);
			myTransparencyType = 2;
		}

		L_Result = true;
	}
#ifdef _DEBUG
	else
	{
		printf( "LoadFile %s Failed\n", aFile->myFullFileName.c_str());
	}
#endif
	return L_Result;
}
	
// special case for cube map
// generate file names from base name
bool	OpenGLTexture::CubeMapGeneration()
{
	bool result=true;
	FilePathManager*	pathManager=(FilePathManager*)KigsCore::GetSingleton(_S_2_ID("FilePathManager"));
	kstl::string fullfilename;
	char asciiCount[16];
	asciiCount[0]=0;

	// get extension
	kstl::string	extension=myFileName;
	extension=extension.substr(extension.rfind("."));
	// remove extension
	kstl::string basefilename=myFileName;
	basefilename=basefilename.substr(0,basefilename.length()-extension.length());

	// check if all 6 textures are ok
	int index;
	for(index=0;index<6;index++)
	{
		kstl::string filename=basefilename;
		filename+="_";
		sprintf(asciiCount,"%d",(index+1));
		filename+=asciiCount;
		filename+=extension;

        SmartPointer<FileHandle> fullfilenamehandle = pathManager->FindFullName(filename);
		if(fullfilenamehandle != 0)
		{
            fullfilename = fullfilenamehandle->myFullFileName;
        }
        else
        {
			result=false;
			break;
		}
	}

	if(result) // ok, the 6 textures are here
	{
		// first load them all

		//kfloat minwidth=1024;
		//kfloat minheight=1024;

		// TODO
		
		return true;
	
	}

	return result;
}

bool OpenGLTexture::CreateFromImage(TinyImage* image) 
{
	if(image == 0)
		return false;

			int pow2sizex = 1;
	while (pow2sizex < image->GetWidth())
	{
		pow2sizex = pow2sizex << 1;
	}

	int pow2sizey = 1;
	while (pow2sizey < image->GetHeight())
	{
		pow2sizey = pow2sizey << 1;
	}


	// check if we need to resize the image
	
	if (((pow2sizex != image->GetWidth()) || (pow2sizey != image->GetHeight())) )
	{
		if ((image->GetFormat() != TinyImage::BC1) && (image->GetFormat() != TinyImage::BC2) && (image->GetFormat() != TinyImage::BC3) )
			image->FastResize(pow2sizex, pow2sizey);
	}
	myPow2Width=pow2sizex;
	myPow2Height=pow2sizey;
	
	// no palette image can be converted directly
	switch(image->GetFormat())
	{
	case TinyImage::ABGR_16_1555_DIRECT_COLOR:
	case TinyImage::RGB_16_565 :
	case TinyImage::RGBA_16_5551 :	
	case TinyImage::RGBA_32_8888 :
	case TinyImage::RGB_24_888 :
		return CreateFromBuffer(image->GetPixelData(),image->GetWidth(),image->GetHeight(),image->GetFormat());
		break;
	default:
		break;
	}

	// and here for palette conversion
	unsigned int	L_TextureFormat;
	unsigned int  L_TextureLevel;
	unsigned int  L_TextureTypes;

	myWidth				= image->GetWidth();
	myHeight			= image->GetHeight();
	L_TextureTypes		= TEXTURE_2D;

	// compute power of two textures
	/*int pow2size=1;
	while(pow2size < image->GetWidth())
	{
		pow2size=pow2size<<1;
	}
	myPow2Width=pow2size;

	pow2size=1;
	while(pow2size < image->GetHeight())
	{
		pow2size=pow2size<<1;
	}
	myPow2Height=pow2size;*/

	unsigned char*	imageData	= image->GetPixelData();

	unsigned int	dataSize	= image->GetPixelDataSize();

	unsigned int	pixCount	= image->GetHeight()*image->GetWidth();

	unsigned short* data		= new unsigned short[pixCount];

	KIGS_MESSAGE("Create From image\n");
	char message[512];
	sprintf(message,"DataSize = %i\n", dataSize);
	KIGS_MESSAGE(message);

	sprintf(message,"pixCount = %i\n", pixCount);
	KIGS_MESSAGE(message);

	sprintf(message,"myWidth = %i\n", (int)myWidth);
	KIGS_MESSAGE(message);

	sprintf(message,"myHeight = %i\n", (int)myHeight);
	KIGS_MESSAGE(message);

	switch (image->GetFormat()) 
	{

		case TinyImage::PALETTE16_256_COLOR : 
		{
			L_TextureLevel  = GL_RGBA;
			L_TextureFormat = GL_RGBA;
			L_TextureTypes  = GL_UNSIGNED_SHORT_5_5_5_1;
			unsigned short* palette		= (unsigned short*)image->GetPaletteData();
			unsigned int	colorCount	= image->GetPaletteDataSize()/2; // 1 unsigned short by color
			
			// set from ABGR to BGRA
			// alpha = 0 for index 0
			palette[0] = (palette[0]<<1);
			// alpha = 1 for the others
			for(int index=1; index < colorCount; index++)
				palette[index] = (palette[index]<<1)|0x1;
			
			// replace index by associate color
			for(int index=0; index < dataSize; index++)
				data[index] = palette[imageData[index]];
			//glGenTextures(1,&myTextureGLIndex);					
			glBindTexture (GL_TEXTURE_2D, myTextureGLIndex);					
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			
			glTexImage2D (GL_TEXTURE_2D, 0, L_TextureLevel, myWidth, myHeight, 0, L_TextureFormat, L_TextureTypes, data);
			myTransparencyType	= 1;
			KIGS_ASSERT(glGetError()==GL_NO_ERROR);
			glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);				
			glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		}
			break;
		case TinyImage::PALETTE16_16_COLOR : 
		{
			L_TextureLevel  = GL_RGBA;
			L_TextureFormat = GL_RGBA;
			L_TextureTypes  = GL_UNSIGNED_SHORT_5_5_5_1;
			unsigned short* palette		= (unsigned short*)image->GetPaletteData();
			unsigned int	colorCount	= image->GetPaletteDataSize()/2; // 1 unsigned short by color
			int				index;
			
			// set from ABGR to BGRA
			// alpha = 0 for index 0
			palette[0] = (palette[0]<<1);
			// alpha = 1 for the others
			for(index=1; index < colorCount; index++)
				palette[index] = (palette[index]<<1)|0x1;
			
			// replace index by associate color
			for(index=0; index < dataSize; index++) {
				unsigned char p1 = imageData[index]&0x0f;
				unsigned char p2 = imageData[index]>>4;
				
				// 2 pixel per char
				data[2*index] = palette[p1];
				data[2*index+1] = palette[p2];
			}
			//glGenTextures(1,&myTextureGLIndex);					
			glBindTexture (GL_TEXTURE_2D, myTextureGLIndex);					
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			
			glTexImage2D (GL_TEXTURE_2D, 0, L_TextureLevel, myWidth, myHeight, 0, L_TextureFormat, L_TextureTypes, data);
			myTransparencyType	= 1;
			KIGS_ASSERT(glGetError()==GL_NO_ERROR);
			glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);				
			glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		}
		break;
		
		default:
			KIGS_ASSERT(0 && "not supported format");
			break;
	
	}
	ComputeRatio();
	delete[] data;
	
	return true;
	
}

bool	OpenGLTexture::UpdateBufferZone(unsigned char* bitmapbuffer, const BBox2DI& zone, const Point2DI& bitmapSize)
{
	return false;
}

///// DYNAMIC TEXTURE

bool			OpenGLTexture::CanUseDynamicTexture(TinyImage::ImageFormat format)
{
	return false;
}
bool			OpenGLTexture::UseDynamicTexture(unsigned char* buffer, unsigned int width, unsigned int height, TinyImage::ImageFormat format, bool needRealloc)
{
	return true;
}