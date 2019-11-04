

struct OpenGLTexturePlatformImpl
{
	// Nothing for windows
};



// special case for cube map
// generate file names from base name
bool	OpenGLTexture::CubeMapGeneration()
{
	RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->FlushState();
	bool result=true;
	FilePathManager*	pathManager=(FilePathManager*)KigsCore::GetSingleton("FilePathManager");
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
		if (fullfilenamehandle)
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

void	OpenGLTexture::GetPixels(unsigned int* _array)
{
	RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->FlushState();
	//glBindTexture(GL_TEXTURE_2D,myTextureGLIndex);
	renderer->ActiveTextureChannel(0);
	renderer->BindTexture(RENDERER_TEXTURE_2D, myTextureGLIndex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, _array);
}

void	OpenGLTexture::SetPixels(unsigned int* _array, int Width, int Height)
{
	RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	renderer->FlushState();
	renderer->ActiveTextureChannel(0);
	//glBindTexture(GL_TEXTURE_2D,myTextureGLIndex);
	renderer->BindTexture(RENDERER_TEXTURE_2D, myTextureGLIndex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, _array);
}