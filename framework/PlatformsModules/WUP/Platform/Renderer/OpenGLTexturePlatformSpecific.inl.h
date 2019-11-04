



bool	OpenGLTexture::CreateFromImage(TinyImage* image)
{
	RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	if(image==0)
		return false;

	/*int pow2sizex = 1;
	while (pow2sizex < image->GetWidth())
	{
		pow2sizex = pow2sizex << 1;
	}

	int pow2sizey = 1;
	while (pow2sizey < image->GetHeight())
	{
		pow2sizey = pow2sizey << 1;
	}

	int openGLFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

	// check if we need to resize the image
	
	if (((pow2sizex != image->GetWidth()) || (pow2sizey != image->GetHeight())))// && myForcePow2)
	{
		if ((image->GetFormat() != TinyImage::BC1) && (image->GetFormat() != TinyImage::BC2) && (image->GetFormat() != TinyImage::BC3) )
			image->FastResize(pow2sizex, pow2sizey);
	}*/

	int openGLFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

	// no palette image can be converted directly
	switch(image->GetFormat())
	{
	case TinyImage::ABGR_16_1555_DIRECT_COLOR:
	case TinyImage::RGB_16_565 :
	case TinyImage::RGBA_16_5551 :	
	case TinyImage::RGBA_32_8888 :
	case TinyImage::RGB_24_888 :
		return CreateFromBuffer(image->GetPixelData(), image->GetWidth(), image->GetHeight(), image->GetFormat());
		break;
	case TinyImage::BC1:
	case TinyImage::BC2:
	case TinyImage::BC3:
		myWidth = myPow2Width = image->GetWidth();
		myHeight = myPow2Height = image->GetHeight();
		myTextureType = TEXTURE_2D;

		myTransparencyType = 0;

		if (image->GetFormat() == TinyImage::BC1)
		{
			openGLFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		}
		else if (image->GetFormat() == TinyImage::BC2)
		{
			openGLFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			myTransparencyType = 1;
		}
		else if (image->GetFormat() == TinyImage::BC3)
		{
			openGLFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			myTransparencyType = 1;
		}
		renderer->BindTexture(RENDERER_TEXTURE_2D, myTextureGLIndex);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_WRAP_S, RENDERER_REPEAT);
		//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_WRAP_T, RENDERER_REPEAT);
		//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MAG_FILTER, RENDERER_LINEAR);

		//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		if (((bool)myHasMipmap) && image->getMipMapCount())
			//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MIN_FILTER, RENDERER_LINEAR_MIPMAP_LINEAR);
		else
		{
			// force no mipmap
			myHasMipmap = false;

			//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MIN_FILTER, RENDERER_LINEAR);
		}
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		if (((bool)myHasMipmap) && image->getMipMapCount())
		{
			int mipsizex = myWidth;
			int mipsizey = myHeight;
			int mipsize = image->GetPixelDataSize();

			unsigned char* mipdatastart = image->GetPixelData();

			int mipmapC;
			for (mipmapC = 0; mipmapC < image->getMipMapCount(); mipmapC++)
			{
				glCompressedTexImage2D(GL_TEXTURE_2D, mipmapC, openGLFormat, (GLsizei)mipsizex, (GLsizei)mipsizey, 0, mipsize, mipdatastart);
			
				mipdatastart += mipsize;
				mipsizex = mipsizex >> 1;
				if (mipsizex < 1)
				{
					mipsizex = 1;
				}
				mipsizey = mipsizey >> 1;
				if (mipsizey < 1)
				{
					mipsizey = 1;
				}

				mipsize = mipsizex*mipsizey;
				if (image->GetFormat() == TinyImage::BC1)
				{
					mipsize /= 2;
					if (mipsize < 8)
					{
						mipsize = 8;
					}
				}
				else
				{
					if (mipsize < 16)
					{
						mipsize = 16;
					}
				}
			}
		}
		else
		{
			
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, openGLFormat, (GLsizei)myWidth, (GLsizei)myHeight, 0, image->GetPixelDataSize(), image->GetPixelData());
		}
		ComputeRatio();

		return true;
		break;
	}

	// for paletted format :
	myWidth				= image->GetWidth();
	myHeight			= image->GetHeight();
	myTextureType		= TEXTURE_2D;

	unsigned char*	imageData	= image->GetPixelData();
	unsigned int	dataSize	= image->GetPixelDataSize();

	unsigned int	pixCount	= image->GetHeight()*image->GetWidth();

	//glBindTexture (GL_TEXTURE_2D, myTextureGLIndex);
	renderer->BindTexture (RENDERER_TEXTURE_2D, myTextureGLIndex);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	renderer->TextureParameteri (RENDERER_TEXTURE_2D, RENDERER_TEXTURE_WRAP_S, RENDERER_REPEAT);
	//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	renderer->TextureParameteri (RENDERER_TEXTURE_2D, RENDERER_TEXTURE_WRAP_T, RENDERER_REPEAT);
	//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	renderer->TextureParameteri (RENDERER_TEXTURE_2D,RENDERER_TEXTURE_MAG_FILTER, RENDERER_LINEAR);

	if(myHasMipmap)
		//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		renderer->TextureParameteri (RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MIN_FILTER, RENDERER_LINEAR_MIPMAP_LINEAR);
	else
		//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		renderer->TextureParameteri (RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MIN_FILTER, RENDERER_LINEAR);

	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	switch(image->GetFormat())
	{
	case TinyImage::PALETTE16_256_COLOR :
		{
			unsigned short*	data		= new unsigned short[pixCount];
			unsigned short* palette		= (unsigned short*)image->GetPaletteData();
			unsigned int	colorCount	= image->GetPaletteDataSize()/2; // 1 unsigned short by color

			// set from ABGR to BGRA
			// alpha = 0 for index 0
			palette[0] = (palette[0]<<1);
			// alpha = 1 for the others
			for(unsigned int index=1; index < colorCount; index++)
				palette[index] = (palette[index]<<1)|0x1;

			// replace index by associate color
			for(unsigned int index=0; index < dataSize; index++)
				data[index] = palette[imageData[index]];
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, myWidth, myHeight, 0, GL_BGRA, GL_UNSIGNED_SHORT_5_5_5_1, data);
			myTransparencyType	= 1;
			delete[] data;

		}
		break;
	case TinyImage::PALETTE16_16_COLOR :
		{
			unsigned short*	data		= new unsigned short[pixCount];
			unsigned short* palette		= (unsigned short*)image->GetPaletteData();
			unsigned int	colorCount	= image->GetPaletteDataSize()/2; // 1 unsigned short by color
			unsigned int	index;

			// set from ABGR to BGRA
			// alpha = 0 for index 0
			palette[0] = (palette[0]<<1);
			// alpha = 1 for the others
			for(index=1; index < colorCount; index++)
				palette[index] = (palette[index]<<1)|0x1;

			// replace index by associate color
			for(index=0; index < dataSize; index++)
			{
				unsigned char p1 = imageData[index]&0x0f;
				unsigned char p2 = imageData[index]>>4;

				// 2 pixel per char
				data[2*index] = palette[p1];
				data[2*index+1] = palette[p2];
			}
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, myWidth, myHeight, 0, GL_BGRA, GL_UNSIGNED_SHORT_5_5_5_1, data);
			myTransparencyType	= 1;
			delete[] data;
		}
		break;

	default :
		KIGS_ERROR("Texture::CreateFromImage -> color mode unknown",1);
		return false;
	}

	ComputeRatio();

	return true;
}

// special case for cube map
// generate file names from base name
bool	OpenGLTexture::CubeMapGeneration()
{
	RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->FlushState();
	bool result=true;
	FilePathManager*	pathManager=(FilePathManager*)Core::GetSingleton(_S_2_ID("FilePathManager"));
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
		if (fullfilenamehandle != 0)
		{
			fullfilename = fullfilenamehandle->myFullFileName;
		}
		else
		{
			result = false;
			break;
		}

	}

	if(result) // ok, the 6 textures are here
	{
		// first load them all

		unsigned int minwidth=1024;
		unsigned int minheight=1024;
		TinyImage* Img[6];
		unsigned Format;
		int components=3;
		for(index=0;index<6;index++)
		{
			kstl::string filename=basefilename;
			filename+="_";
			sprintf(asciiCount,"%d",(index+1));
			filename+=asciiCount;
			filename+=extension;

			SmartPointer<FileHandle> fullfilenamehandle = pathManager->FindFullName(filename);
			
			fullfilename = fullfilenamehandle->myFullFileName;


			Img[index] = TinyImage::CreateImage(fullfilename.c_str());
			myWidth = Img[index]->GetWidth();
			myHeight = Img[index]->GetHeight();

			/*BITMAPINFO Info;
			ilGenImages(1,&(Img[index]));
			ilBindImage(Img[index]);
			ilLoadImage((char* const)fullfilename.c_str());
			ilutGetBmpInfo(&Info);

			myWidth = (int)Info.bmiHeader.biWidth;
			myHeight = (int)Info.bmiHeader.biHeight;*/

			// resize to powerof 2
			if(myWidth>=1024)
			{
				myWidth=1024;
			}
			else if(myWidth>=512)
			{
				myWidth=512;
			}
			else if(myWidth>=256)
			{
				myWidth=256;
			}
			else if(myWidth>=128)
			{
				myWidth=128;
			}
			else if(myWidth>=64)
			{
				myWidth=64;
			}
			else if(myWidth>=32)
			{
				myWidth=32;
			}
			else if(myWidth>=16)
			{
				myWidth=16;
			}
			else
			{
				myWidth=8;
			}

			if(myHeight>=1024)
			{
				myHeight=1024;
			}
			else if(myHeight>=512)
			{
				myHeight=512;
			}
			else if(myHeight>=256)
			{
				myHeight=256;
			}
			else if(myHeight>=128)
			{
				myHeight=128;
			}
			else if(myHeight>=64)
			{
				myHeight=64;
			}
			else if(myHeight>=32)
			{
				myHeight=32;
			}
			else if(myHeight>=16)
			{
				myHeight=16;
			}
			else
			{
				myHeight=8;
			}

			if(myWidth<minwidth)
			{
				minwidth=myWidth;
			}
			if(myHeight<minheight)
			{
				minheight=myHeight;
			}
		}
		myWidth = minwidth;
		myHeight = minheight;
		Format = GL_RGB;
		myTransparencyType = 0;

		// only one texture index
		//glGenTextures(1,&myTextureGLIndex);
		//glBindTexture (GL_TEXTURE_CUBE_MAP, myTextureGLIndex);
		renderer->BindTexture (RENDERER_TEXTURE_CUBE_MAP, myTextureGLIndex);
		glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
		//glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
		renderer->TextureParameteri(RENDERER_TEXTURE_CUBE_MAP, RENDERER_TEXTURE_WRAP_S, RENDERER_REPEAT);
		//glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
		renderer->TextureParameteri(RENDERER_TEXTURE_CUBE_MAP, RENDERER_TEXTURE_WRAP_T, RENDERER_REPEAT);
		//glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		renderer->TextureParameteri(RENDERER_TEXTURE_CUBE_MAP, RENDERER_TEXTURE_MAG_FILTER, RENDERER_LINEAR);
		if(myHasMipmap)
		{
			//glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			renderer->TextureParameteri(RENDERER_TEXTURE_CUBE_MAP, RENDERER_TEXTURE_MIN_FILTER, RENDERER_LINEAR_MIPMAP_LINEAR);
		}
		else
		{
			//glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			renderer->TextureParameteri(RENDERER_TEXTURE_CUBE_MAP, RENDERER_TEXTURE_MIN_FILTER, RENDERER_LINEAR);
		}
		glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// convert them all to the same size and format
		for(index=0;index<6;index++)
		{
			/*BITMAPINFO Info;
			ilBindImage(Img[index]);
			ilutGetBmpInfo(&Info);
			iluScale(myWidth,myHeight,(int)Info.bmiHeader.biPlanes);
			ilConvertImage(IL_RGB,IL_UNSIGNED_BYTE);*/
			unsigned char *T = Img[index]->GetPixelData(); //ilGetData();

			if(myHasMipmap)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, 0, Format, myWidth, myHeight, 0, Format, GL_UNSIGNED_BYTE, T);
			delete Img[index];
		}

		return true;

	}

	return result;
}

bool	OpenGLTexture::UpdateBufferZone(unsigned char* bitmapbuffer, const BBox2DI& zone, const Point2DI& bitmapSize)
{
	RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->BindTexture(RENDERER_TEXTURE_2D, myTextureGLIndex);
	renderer->FlushState();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, bitmapSize.x);

	Point2DI	zonesize = zone.Size();

	glTexSubImage2D(GL_TEXTURE_2D, 0, zone.m_Min.x, zone.m_Min.y, zonesize.x, zonesize.y, GL_RGBA, GL_UNSIGNED_BYTE, bitmapbuffer);

	// reset default
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	return true;
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