namespace Kigs
{
	namespace Draw
	{

		struct OpenGLTexturePlatformImpl
		{
			// Nothing for windows
		};



		// special case for cube map
		// generate file names from base name
		bool	OpenGLTexture::CubeMapGeneration()
		{
			RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
			renderer->FlushState();
			bool result = true;
			auto pathManager = KigsCore::Singleton<File::FilePathManager>();
			std::string fullfilename;
			char asciiCount[16];
			asciiCount[0] = 0;

			// get extension
			std::string	extension = mFileName;
			extension = extension.substr(extension.rfind("."));
			// remove extension
			std::string basefilename = mFileName;
			basefilename = basefilename.substr(0, basefilename.length() - extension.length());

			// check if all 6 textures are ok
			int index;
			for (index = 0; index < 6; index++)
			{
				std::string filename = basefilename;
				filename += "_";
				sprintf(asciiCount, "%d", (index + 1));
				filename += asciiCount;
				filename += extension;

				SmartPointer<File::FileHandle> fullfilenamehandle = pathManager->FindFullName(filename);
				if (fullfilenamehandle)
				{
					fullfilename = fullfilenamehandle->mFullFileName;
				}
				else
				{
					result = false;
					break;
				}

			}

			if (result) // ok, the 6 textures are here
			{
				// first load them all

				unsigned int minwidth = 1024;
				unsigned int minheight = 1024;
				SP<Pict::TinyImage> Img[6];
				unsigned Format;
				int components = 3;
				for (index = 0; index < 6; index++)
				{
					std::string filename = basefilename;
					filename += "_";
					sprintf(asciiCount, "%d", (index + 1));
					filename += asciiCount;
					filename += extension;

					SmartPointer<File::FileHandle> fullfilenamehandle = pathManager->FindFullName(filename);

					fullfilename = fullfilenamehandle->mFullFileName;


					Img[index] = Pict::TinyImage::CreateImage(fullfilename.c_str());
					mWidth = Img[index]->GetWidth();
					mHeight = Img[index]->GetHeight();

					/*BITMAPINFO Info;
					ilGenImages(1,&(Img[index]));
					ilBindImage(Img[index]);
					ilLoadImage((char* const)fullfilename.c_str());
					ilutGetBmpInfo(&Info);

					mWidth = (int)Info.bmiHeader.biWidth;
					mHeight = (int)Info.bmiHeader.biHeight;*/

					// resize to powerof 2
					if (mWidth >= 1024)
					{
						mWidth = 1024;
					}
					else if (mWidth >= 512)
					{
						mWidth = 512;
					}
					else if (mWidth >= 256)
					{
						mWidth = 256;
					}
					else if (mWidth >= 128)
					{
						mWidth = 128;
					}
					else if (mWidth >= 64)
					{
						mWidth = 64;
					}
					else if (mWidth >= 32)
					{
						mWidth = 32;
					}
					else if (mWidth >= 16)
					{
						mWidth = 16;
					}
					else
					{
						mWidth = 8;
					}

					if (mHeight >= 1024)
					{
						mHeight = 1024;
					}
					else if (mHeight >= 512)
					{
						mHeight = 512;
					}
					else if (mHeight >= 256)
					{
						mHeight = 256;
					}
					else if (mHeight >= 128)
					{
						mHeight = 128;
					}
					else if (mHeight >= 64)
					{
						mHeight = 64;
					}
					else if (mHeight >= 32)
					{
						mHeight = 32;
					}
					else if (mHeight >= 16)
					{
						mHeight = 16;
					}
					else
					{
						mHeight = 8;
					}

					if (mWidth < minwidth)
					{
						minwidth = mWidth;
					}
					if (mHeight < minheight)
					{
						minheight = mHeight;
					}
				}
				mWidth = minwidth;
				mHeight = minheight;
				Format = GL_RGB;
				mTransparencyType = 0;

				// only one texture index
				//glGenTextures(1,&mTextureGLIndex);
				//glBindTexture (GL_TEXTURE_CUBE_MAP, mTextureGLIndex);
				renderer->BindTexture(RENDERER_TEXTURE_CUBE_MAP, mTextureGLIndex);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				//glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
				renderer->TextureParameteri(RENDERER_TEXTURE_CUBE_MAP, RENDERER_TEXTURE_WRAP_S, RENDERER_REPEAT);
				//glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
				renderer->TextureParameteri(RENDERER_TEXTURE_CUBE_MAP, RENDERER_TEXTURE_WRAP_T, RENDERER_REPEAT);
				//glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				renderer->TextureParameteri(RENDERER_TEXTURE_CUBE_MAP, RENDERER_TEXTURE_MAG_FILTER, RENDERER_LINEAR);
				if (mHasMipmap)
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
				for (index = 0; index < 6; index++)
				{
					/*BITMAPINFO Info;
					ilBindImage(Img[index]);
					ilutGetBmpInfo(&Info);
					iluScale(mWidth,mHeight,(int)Info.bmiHeader.biPlanes);
					ilConvertImage(IL_RGB,IL_UNSIGNED_BYTE);*/
					unsigned char* T = Img[index]->GetPixelData(); //ilGetData();

					if (mHasMipmap)
					{
						glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
					}
					else
					{
						glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
					}

					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, 0, Format, mWidth, mHeight, 0, Format, GL_UNSIGNED_BYTE, T);
				}
				return true;

			}

			return result;
		}

		bool	OpenGLTexture::UpdateBufferZone(unsigned char* bitmapbuffer, const BBox2DI& zone, const v2i& bitmapSize)
		{
			RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
			renderer->BindTexture(RENDERER_TEXTURE_2D, mTextureGLIndex);
			renderer->FlushState();
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glPixelStorei(GL_UNPACK_ROW_LENGTH, bitmapSize.x);

			v2i	zonesize = zone.Size();

			glTexSubImage2D(GL_TEXTURE_2D, 0, zone.m_Min.x, zone.m_Min.y, zonesize.x, zonesize.y, GL_RGBA, GL_UNSIGNED_BYTE, bitmapbuffer);

			// reset default
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

			return true;
		}

		///// DYNAMIC TEXTURE

		bool			OpenGLTexture::CanUseDynamicTexture(Pict::TinyImage::ImageFormat format)
		{
			return false;
		}
		bool			OpenGLTexture::UseDynamicTexture(unsigned char* buffer, unsigned int width, unsigned int height, Pict::TinyImage::ImageFormat format, bool needRealloc)
		{
			return true;
		}

		void	OpenGLTexture::GetPixels(unsigned int* _array)
		{
			RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
			renderer->FlushState();
			//glBindTexture(GL_TEXTURE_2D,mTextureGLIndex);
			renderer->ActiveTextureChannel(0);
			renderer->BindTexture(RENDERER_TEXTURE_2D, mTextureGLIndex);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, _array);
		}

		void	OpenGLTexture::SetPixels(unsigned int* _array, int Width, int Height)
		{
			RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
			renderer->FlushState();
			renderer->ActiveTextureChannel(0);
			//glBindTexture(GL_TEXTURE_2D,mTextureGLIndex);
			renderer->BindTexture(RENDERER_TEXTURE_2D, mTextureGLIndex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, _array);
		}
	}
}