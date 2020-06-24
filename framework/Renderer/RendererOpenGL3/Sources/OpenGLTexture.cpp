#include "PrecompiledHeaders.h"

#include "math.h"
#include "FilePathManager.h"
#include "Core.h"
#include "ModuleRenderer.h"
#include "TravState.h"

#include "OpenGLTexture.h"
#include "RendererDefines.h"

#include "TinyImage.h"
//#ifndef JAVASCRIPT
#include "FreeType_TextDrawer.h"
//#endif
#include "RendererOpenGL.h"
#include "ModuleFileManager.h"
#include "FilePathManager.h"
#include "GLSLShader.h"
// include after all other kigs include (for iOS)
#include "Platform/Renderer/OpenGLInclude.h"


#include "Platform/Renderer/OpenGL3TexturePlatformSpecific.inl.h"

#include "ModuleSceneGraph.h"


/*#ifdef JAVASCRIPT
extern "C" void		JSDrawMultilineTextUTF16(const unsigned short* TextToWrite, int maxLineNumber, int maxWidth, u32 FontSize, const char* FontName, u32 a_Alignment, u32 jumpedLines, int R, int G, int B, int A, int& width, int& height);
extern "C" void		JSInitImageData(void* pixels, int width, int height);
#endif*/

#define FREETYPE_SIZE_COEFF	0.65f

int	GetFirstLeftBit(u32 n);

int	GetFirstLeftBit(u32 n)
{
	static	unsigned char addlast16[16] = { 0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3 };

	int result = 0;

	if (n & 0xffff0000)
	{
		result = 16;
		n >>= 16;
	}

	if (n & 0xff00)
	{
		result += 8;
		n >>= 8;
	}

	if (n & 0xf0)
	{
		result += 4;
		n >>= 4;
	}

	result += (int)addlast16[n];

	return result;
};

IMPLEMENT_CLASS_INFO(OpenGLTexture)

OpenGLTexture::OpenGLTexture(const kstl::string& name, CLASS_NAME_TREE_ARG)
	: Texture(name, PASS_CLASS_NAME_TREE_ARG)
	, myTextureGLIndex((u32)-1)
	, myCanReuseBuffer(false)
	, myPow2BufferSize(0)
	, myPow2Buffer(NULL)
	, privatePointer(NULL)
#ifdef SUPPORT_ETC_TEXTURE
	, myIDETCAlphaTexture(0)
#endif
{
}

OpenGLTexture::~OpenGLTexture()
{

	if (myTextureGLIndex != (u32)-1)
	{
		ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
		scenegraph->AddDefferedItem((void*)myTextureGLIndex, DefferedAction::DESTROY_TEXTURE);
		//kigsprintf("del texture %d - %s\n", myTextureGLIndex, getName().c_str());
	}
	//ModuleRenderer::theGlobalRenderer->DeleteTexture(1, &myTextureGLIndex);
#ifdef SUPPORT_ETC_TEXTURE
	if (myIDETCAlphaTexture)
	{
		ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
		scenegraph->AddDefferedItem((void*)myIDETCAlphaTexture, DefferedAction::DESTROY_TEXTURE);
		//	ModuleRenderer::theGlobalRenderer->DeleteTexture(1, &myIDETCAlphaTexture);
	}
#endif



	if (privatePointer)
		delete privatePointer;

	if (myPow2Buffer)
		delete[] myPow2Buffer;
}

void	OpenGLTexture::InitModifiable()
{
	if (_isInit)
		return;

	myCanReuseBuffer = false;

	Texture::InitModifiable();
}

void	OpenGLTexture::ProtectedDestroy()
{
	CoreModifiableAttribute* delayed = getAttribute("DelayedInit");

	if (delayed) // delay init
	{
		void* datastruct;
		if (delayed->getValue(datastruct))
		{
			TextureDelayedInitData* delayedStruct = (TextureDelayedInitData*)datastruct;
			delete delayedStruct;
			RemoveDynamicAttribute("DelayedInit");
		}

	}
	Texture::ProtectedDestroy();
}

void	OpenGLTexture::UninitModifiable()
{
	if (!_isInit)
		return;
	// get TextureIndex
	if (!HasFlag(isDirtyContext) && myTextureGLIndex != (u32)-1)
	{
		ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
		scenegraph->AddDefferedItem((void*)myTextureGLIndex, DefferedAction::DESTROY_TEXTURE);
		//ModuleRenderer::theGlobalRenderer->DeleteTexture(1, &myTextureGLIndex);
	}
	myTextureGLIndex = (u32)-1;


	CoreModifiableAttribute* delayed = getAttribute("DelayedInit");

	if (delayed) // delay init
	{
		void* datastruct;
		if (delayed->getValue(datastruct))
		{
			TextureDelayedInitData* delayedStruct = (TextureDelayedInitData*)datastruct;
			delete delayedStruct;
			RemoveDynamicAttribute("DelayedInit");
		}

	}

	Texture::UninitModifiable();
}

void	OpenGLTexture::InitForFBO()
{
	ModuleRenderer::theGlobalRenderer->CreateTexture(1, &myTextureGLIndex);
	Texture::InitForFBO();
}

bool	OpenGLTexture::PreDraw(TravState* travstate)
{
	CoreModifiableAttribute* delayed = getAttribute("DelayedInit");

	if (delayed) // delay init
	{
		void* datastruct;
		if (delayed->getValue(datastruct))
		{
			TextureDelayedInitData* delayedStruct = (TextureDelayedInitData*)datastruct;
			CreateFromImage(delayedStruct->delayedimage, true);
			delete delayedStruct;
			RemoveDynamicAttribute("DelayedInit");
		}

	}

	if (myTextureGLIndex == (u32)-1)
	{
		return false;
	}


	if (Texture::PreDraw(travstate))
	{
		RendererTextureType type;
		switch ((int)myTextureType)
		{
		case 	TEXTURE_1D:
		{
			type = RENDERER_TEXTURE_1D;
			break;
		}
		case 	TEXTURE_2D:
		{
			type = RENDERER_TEXTURE_2D;
			break;
		}
		case 	TEXTURE_3D:
		{
			type = RENDERER_TEXTURE_3D;
			break;
		}
		case 	TEXTURE_CUBE_MAP:
		{
			type = RENDERER_TEXTURE_CUBE_MAP;
			break;
		}
		}


		
		RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer);
		
#ifdef SUPPORT_ETC_TEXTURE
		// check for separate alpha texture
		if (myIDETCAlphaTexture)
		{
			renderer->ActiveTextureChannel(1);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, myIDETCAlphaTexture);
			renderer->ActiveTextureChannel(0);
		}
#endif

		renderer->BindTexture(type, myTextureGLIndex);
		renderer->EnableTexture();

		renderer->TextureParameteri(type, RENDERER_TEXTURE_WRAP_S, (myRepeatU) ? RENDERER_REPEAT : RENDERER_CLAMP_TO_EDGE);
		renderer->TextureParameteri(type, RENDERER_TEXTURE_WRAP_T, (myRepeatV) ? RENDERER_REPEAT : RENDERER_CLAMP_TO_EDGE);
		renderer->TextureParameteri(type, RENDERER_TEXTURE_MIN_FILTER, myForceNearest ? RENDERER_NEAREST : ((myHasMipmap) ? RENDERER_LINEAR_MIPMAP_LINEAR : RENDERER_LINEAR));
		
		return true;
	}
	return false;
}

bool	OpenGLTexture::PostDraw(TravState* travstate)
{
	if (myTextureGLIndex == -1)
	{
		return false;
	}
	ModuleSpecificRenderer* renderer = travstate->GetRenderer();
	if (Texture::PostDraw(travstate))
	{
#ifdef SUPPORT_ETC_TEXTURE
		if (myIDETCAlphaTexture)
		{
			renderer->ActiveTextureChannel(1);
			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			renderer->ActiveTextureChannel(0);
		}
#endif

		RendererTextureType type;
		switch ((int)myTextureType)
		{
		case 	TEXTURE_1D:
		{
			type = RENDERER_TEXTURE_1D;
			break;
		}
		case 	TEXTURE_2D:
		{
			type = RENDERER_TEXTURE_2D;
			break;
		}
		case 	TEXTURE_3D:
		{
			type = RENDERER_TEXTURE_3D;
			break;
		}
		case 	TEXTURE_CUBE_MAP:
		{
			type = RENDERER_TEXTURE_CUBE_MAP;
			break;
		}
		}
		renderer->UnbindTexture(type, myTextureGLIndex);
		renderer->DisableTexture();

		return true;
	}
	return false;
}

bool OpenGLTexture::ManagePow2Buffer(u32 aWidth, u32 aHeight, u32 aPixSize)
{
	int currentSize = myPow2Width * myPow2Height * myPixelSize;
	int newSize = aWidth * aHeight * aPixSize;
	
	bool reinit = (newSize * 4) < currentSize;

	if ((aWidth <= myWidth) && (aHeight <= myHeight) && (aPixSize <= myPixelSize) && (!reinit) )
		return false;

	if (reinit)
	{
		myPow2Width = 1;
		myPow2Height = 1;
	}

	// get pow2 size
	while (myPow2Width < aWidth)
	{
		myPow2Width = myPow2Width << 1;
		myCanReuseBuffer = false;
	}
	while (myPow2Height < aHeight)
	{
		myPow2Height = myPow2Height << 1;
		myCanReuseBuffer = false;
	}

	// if buffer already in pow2, do not use pow2 buffer
	if (aWidth == myPow2Width && aHeight == myPow2Height)
	{
		myCanReuseBuffer = false;
		if (myPow2Buffer)
			delete[] myPow2Buffer;
		myPow2Buffer = nullptr;
		return false;
	}

	//create pow2buffer
	if (!myCanReuseBuffer)
	{
		// delete older if exist
		if (myPow2Buffer)
			delete[] myPow2Buffer;

		myPow2BufferSize = myPow2Width*myPow2Height*aPixSize;
		myPow2Buffer = new unsigned char[myPow2BufferSize];
		memset(myPow2Buffer, 0, myPow2BufferSize);
		return true;
	}

	return false;
}

bool	OpenGLTexture::CreateFromImage(const SmartPointer<TinyImage>& image, bool directInit)
{
	if (image.isNil())
		return false;

	unsigned char * data;

	TinyImage::ImageFormat format = image->GetFormat();

	RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer);
	bool needRealloc = false;
	

	if (!directInit)
	{
		myTextureType = TEXTURE_2D;

		u32 pixSize = TinyImage::GetPixelValueSize(image->GetFormat());


		if (pixSize > 0)
		{
			needRealloc = ManagePow2Buffer(image->GetWidth(), image->GetHeight(), pixSize);
		}
		else // compressed texture
		{
			myPow2Width = image->GetWidth();
			myPow2Height = image->GetHeight();
		}

		myPixelSize = pixSize;
		myWidth = image->GetWidth();
		myHeight = image->GetHeight();
		int line_size = image->GetPixelLineSize();
		
		if (line_size != myWidth * myPixelSize) 
			myCanReuseBuffer = false;

		if (myPow2Buffer && !myCanReuseBuffer && !CanUseDynamicTexture(image->GetFormat()))
		{
			unsigned char* pos = (unsigned char*)myPow2Buffer;
			unsigned char* posRead = (unsigned char*)image->GetPixelData();

			
			int lLineW = line_size;//myWidth*myPixelSize;
			for (u32 j = 0; j < myHeight; j++)
			{
				memcpy(pos, posRead, lLineW);
				pos += myPow2Width*myPixelSize;
				posRead += line_size; // myWidth*myPixelSize;
			}

			data = myPow2Buffer;
		}
		else
			data = image->GetPixelData();

		// check if delayed init not already asked

		CoreModifiableAttribute* delayed = getAttribute("DelayedInit");
		if (delayed) // if already exist, remove it
		{
			void* datastruct;
			if (delayed->getValue(datastruct))
			{
				TextureDelayedInitData* delayedStruct = (TextureDelayedInitData*)datastruct;
				delete delayedStruct;
				RemoveDynamicAttribute("DelayedInit");
			}
		}

		ComputeRatio();
#ifndef NO_DELAYED_INIT
		// create delayed struct
		TextureDelayedInitData* delayedStruct = new TextureDelayedInitData();
		delayedStruct->delayedimage = image;
		delayedStruct->needRealloc = needRealloc;
		CoreModifiableAttribute* newAttr = AddDynamicAttribute(ATTRIBUTE_TYPE::RAWPTR, "DelayedInit");
		newAttr->setValue(delayedStruct);
		return true;
#endif
	}
	else
	{
		CoreModifiableAttribute* delayed = getAttribute("DelayedInit");

		if (delayed) // delay init
		{
			void* datastruct;
			if (delayed->getValue(datastruct))
			{
				TextureDelayedInitData* delayedStruct = (TextureDelayedInitData*)datastruct;
				needRealloc = delayedStruct->needRealloc;
			}
		}

		if (myPow2Buffer && !myCanReuseBuffer && !CanUseDynamicTexture(image->GetFormat()))
		{
			data = myPow2Buffer;
		}
		else
			data = image->GetPixelData();
	}

	if (myTextureGLIndex == (u32)-1)
	{
		// get TextureIndex
		ModuleRenderer::theGlobalRenderer->CreateTexture(1, &myTextureGLIndex);
	}
	renderer->ActiveTextureChannel(0);
	renderer->BindTexture(RENDERER_TEXTURE_2D, myTextureGLIndex);
	bool success = false;

	renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MIN_FILTER, myForceNearest ? RENDERER_NEAREST :  RENDERER_LINEAR);

	bool mipmap_generated = false;

	if (CanUseDynamicTexture(image->GetFormat()))
	{
		success = UseDynamicTexture(data, myWidth, myHeight, image->GetFormat(), needRealloc);
	}
	renderer->FlushState();
	if (!success)
	{
		switch (image->GetFormat())
		{
		case TinyImage::AI88:
		{
			if (myCanReuseBuffer)
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, myWidth, myHeight, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, (GLsizei)myPow2Width, (GLsizei)myPow2Height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
			myTransparencyType = 2;
			break;
		}
		case TinyImage::ABGR_16_1555_DIRECT_COLOR:
		{
			// create tmp RGBA buffer and convert

			unsigned char*	newdata = new unsigned char[myWidth*myHeight * 4];
			u32 i;
			unsigned char* pos = newdata;
			unsigned short* posRead = (unsigned short*)data;
			for (i = 0; i < myWidth*myHeight; i++)
			{
				*pos++ = (((*posRead) & 0x001F)) << 3;
				*pos++ = (((*posRead) & 0x03E0) >> 5) << 3;
				*pos++ = (((*posRead) & 0x7C00) >> 10) << 3;
				if ((*posRead) & 0x8000)
				{
					*pos++ = 255;
				}
				else
				{
					*pos++ = 0;
				}
				posRead++;
			}

			if (myCanReuseBuffer)
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, myWidth, myHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, myPow2Width, myPow2Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, newdata);
			myTransparencyType = 1;
			delete[] newdata;
		}
		break;

		case TinyImage::PALETTE16_256_COLOR:
		{
			unsigned short*	newdata = new unsigned short[myWidth*myHeight];
			unsigned short* palette = (unsigned short*)image->GetPaletteData();
			u32	colorCount = image->GetPaletteDataSize() / 2; // 1 unsigned short by color

															  // set from ABGR to BGRA
															  // alpha = 0 for index 0
			palette[0] = (palette[0] << 1);
			// alpha = 1 for the others
			for (u32 index = 1; index < colorCount; index++)
				palette[index] = (palette[index] << 1) | 0x1;

			// replace index by associate color
			for (u32 index = 0; index < myWidth*myHeight; index++)
				newdata[index] = palette[data[index]];
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, myWidth, myHeight, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, newdata);
			myTransparencyType = 1;
			delete[] newdata;

		}
		break;
		case TinyImage::PALETTE16_16_COLOR:
		{
			unsigned short*	newdata = new unsigned short[myWidth*myHeight];
			unsigned short* palette = (unsigned short*)image->GetPaletteData();
			u32	colorCount = image->GetPaletteDataSize() / 2; // 1 unsigned short by color
			u32	index;

			// set from ABGR to BGRA
			// alpha = 0 for index 0
			palette[0] = (palette[0] << 1);
			// alpha = 1 for the others
			for (index = 1; index < colorCount; index++)
				palette[index] = (palette[index] << 1) | 0x1;

			// replace index by associate color
			for (index = 0; index < myWidth*myHeight; index++)
			{
				unsigned char p1 = data[index] & 0x0f;
				unsigned char p2 = data[index] >> 4;

				// 2 pixel per char
				newdata[2 * index] = palette[p1];
				newdata[2 * index + 1] = palette[p2];
			}
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, myWidth, myHeight, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, newdata);
			myTransparencyType = 1;
			delete[] newdata;
		}
		break;

		case TinyImage::RGB_16_565:
			if (myCanReuseBuffer)
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, myWidth, myHeight, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, myPow2Width, myPow2Height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data);
			myTransparencyType = 0;
			break;
		case TinyImage::RGBA_16_5551:
			if (myCanReuseBuffer)
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, myWidth, myHeight, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, myPow2Width, myPow2Height, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, data);
			myTransparencyType = 1;
			break;
		case TinyImage::RGBA_32_8888:
			if (myCanReuseBuffer)
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, myWidth, myHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, myPow2Width, myPow2Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

			if (myTransparencyType == -1)
			{
				//! TODO
				myTransparencyType = 2;
			}
			break;
		case TinyImage::RGB_24_888:
			if (myCanReuseBuffer)
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, myWidth, myHeight, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, myPow2Width, myPow2Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			myTransparencyType = 0;
			break;
		case TinyImage::GREYSCALE:
			if (myCanReuseBuffer)
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, myWidth, myHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, myPow2Width, myPow2Height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
			myTransparencyType = 0;
			break;
		case TinyImage::ALPHA_8:
			if (myCanReuseBuffer)
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, myWidth, myHeight, GL_ALPHA, GL_UNSIGNED_BYTE, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, myPow2Width, myPow2Height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
			myTransparencyType = 2;
			break;
		
#ifdef SUPPORT_ETC_TEXTURE
		case TinyImage::ETC1:
		{
#ifndef WIN32
			u32 L_TotalSize = (myPow2Width*myPow2Height) / 2;
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, (GLsizei)myPow2Width, (GLsizei)myPow2Height, 0, L_TotalSize, data);
			myTransparencyType = 0;
#else
			// TODO
#endif
			break;
		}
		case TinyImage::ETC1A8:
		{
#ifndef WIN32

			u32 L_TotalSize = (myPow2Width*myPow2Height) / 2;
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, (GLsizei)myPow2Width, (GLsizei)myPow2Height, 0, L_TotalSize, data);


			if (myIDETCAlphaTexture == 0)
			{
				// get TextureIndex
				ModuleRenderer::theGlobalRenderer->CreateTexture(1, &myIDETCAlphaTexture);
			}

			if (myIDETCAlphaTexture) // if alpha texture, apply same params
			{
				glBindTexture(GL_TEXTURE_2D, myIDETCAlphaTexture);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				unsigned char* alphastart = (unsigned char*)data;
				alphastart += L_TotalSize;
				glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, (GLsizei)myPow2Width, (GLsizei)myPow2Height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, alphastart);
				myTransparencyType = 2;
			}
			
#else
			//TODO
#endif
			break;
		}
		case TinyImage::ETC1A4:
		{
			//TODO
			break;
		}
		case TinyImage::ETC2:
		{
			u32 L_TotalSize = (myPow2Width*myPow2Height) / 2;
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB8_ETC2, (GLsizei)myPow2Width, (GLsizei)myPow2Height, 0, L_TotalSize, data);
			myTransparencyType = 0;
			break;
		}
		case TinyImage::ETC2A8:
		{
			u32 L_TotalSize = (myPow2Width*myPow2Height);
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA8_ETC2_EAC, (GLsizei)myPow2Width, (GLsizei)myPow2Height, 0, L_TotalSize, data);
			myTransparencyType = 2;

			break;
		}

#endif
#ifdef SUPPORT_S3TC_TEXTURE
		case TinyImage::BC1:
		case TinyImage::BC2:
		case TinyImage::BC3:

		{
			int openGLFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

			myTransparencyType = 0;

			if (format == TinyImage::BC1)
			{
				openGLFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			}
			else if (format == TinyImage::BC2)
			{
				openGLFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				myTransparencyType = 2;
			}
			else if (format == TinyImage::BC3)
			{
				openGLFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				myTransparencyType = 2;
			}

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

			if (((bool)myHasMipmap) && image->getMipMapCount())
			{
				mipmap_generated = true;
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
			break;
		}
#endif
		default:
			KIGS_ERROR("Texture::CreateFromImage -> color mode unknown", 1);
			return false;
	}
}

	if (myHasMipmap && !mipmap_generated)
	{
		renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MIN_FILTER, RENDERER_LINEAR_MIPMAP_LINEAR);
		renderer->FlushState();
		glGenerateMipmap(GL_TEXTURE_2D);
	}


	ComputeRatio();

	myCanReuseBuffer = true;
	return true;

}

bool OpenGLTexture::CreateFromText(const unsigned short* text, u32 fontSize, const char* fontName, u32 a_Alignment, float R, float G, float B, float A, TinyImage::ImageFormat format, int a_drawingLimit)
{
	return CreateFromText(text, 0, 0, fontSize, fontName, a_Alignment, R, G, B, A, format,  a_drawingLimit);
}

bool OpenGLTexture::CreateFromText(const unsigned short* text, u32 _maxLineNumber, u32 maxSize, u32 fontSize, const char* fontName, u32 a_Alignment, float R, float G, float B, float A, TinyImage::ImageFormat format, int a_drawingLimit)
{
	myIsText = true;
	bool bRet = false;
	unsigned char * pImageData = 0;
//#ifndef JAVASCRIPT

	// freetype size ratio
	fontSize = (int)(((float)fontSize)*FREETYPE_SIZE_COEFF);

	do
	{
		if (!text)
			break;

		int textSize = 0;
		while (text[textSize] != 0)
			textSize++;


		if (!RendererOpenGL::myDrawer->IsInCache(fontName))
		{
			auto& pathManager = KigsCore::Singleton<FilePathManager>();
			SmartPointer<FileHandle> fullfilenamehandle;

			if (pathManager)
			{
				fullfilenamehandle = pathManager->FindFullName(fontName);
			}
			if ((fullfilenamehandle->myStatus&FileHandle::Exist) == 0)
				return false;

			u64 size;
			CoreRawBuffer* L_Buffer = ModuleFileManager::LoadFile(fullfilenamehandle.get(), size);
			if (L_Buffer)
			{
				unsigned char* pBuffer = (unsigned char*)L_Buffer->CopyBuffer();
				RendererOpenGL::myDrawer->SetFont(fontName, pBuffer, size, fontSize);
				L_Buffer->Destroy();
			}
			else
				break;
		}
		else
		{
			RendererOpenGL::myDrawer->SetFont(fontName, 0, 0, fontSize);
		}

		int L_Width = 0;
		int L_Height = 0;
		pImageData = RendererOpenGL::myDrawer->DrawTextToImage(text, textSize, L_Width, L_Height, (TextAlignment)a_Alignment, false, _maxLineNumber, maxSize, a_drawingLimit, (unsigned char)R, (unsigned char)G, (unsigned char)B);

		if (!pImageData)
			break;

/*#else 
	do
	{
		int L_Width = 0;
		int L_Height = 0;

		JSDrawMultilineTextUTF16(text, _maxLineNumber, maxSize, fontSize, fontName, a_Alignment, 0, R * 255.0f, G * 255.0f, B * 255.0f, A * 255.0f, L_Width, L_Height);

		pImageData = new unsigned char[L_Width*L_Height * 4];

		JSInitImageData(pImageData, L_Width, L_Height);

#endif*/

		SmartPointer<TinyImage>	img = OwningRawPtrToSmartPtr(TinyImage::CreateImage(pImageData, L_Width, L_Height, TinyImage::RGBA_32_8888));

		if (!CreateFromImage(img))
			break;

		bRet = true;
	} while (0);

	if (pImageData)
		delete[] pImageData;

	return bRet;
}

bool	OpenGLTexture::Load()
{

	if ((int)myTextureType == TEXTURE_CUBE_MAP)
	{
		return CubeMapGeneration();
	}
	else
	{
		auto& pathManager = KigsCore::Singleton<FilePathManager>();

		kstl::string fileName = myFileName.const_ref();

		if (fileName != "")
		{
			SmartPointer<FileHandle> lFile = pathManager->FindFullName(fileName);
			if (!lFile)
				return false;

			SmartPointer<TinyImage> toload = OwningRawPtrToSmartPtr(TinyImage::CreateImage(lFile.get()));
			if (!toload.isNil())
			{
				bool result = false;
				if (toload->IsOK())
				{
					bool needResize = false;
					int resizeX = toload->GetWidth();
					int resizeY = toload->GetHeight();

					// check if too big
					if (toload->GetWidth() > 4096 || toload->GetHeight() > 4096)
					{
						needResize = true;

						if (toload->GetWidth() > toload->GetHeight())
						{
							resizeX = 4096;
							resizeY = (toload->GetHeight() * 4096) / toload->GetWidth();
						}
						else
						{
							resizeX = (toload->GetWidth() * 4096) / toload->GetHeight();
							resizeY = 4096;
						}
					}

					// check if force pow 2
					if (myForcePow2)
					{
						// compute power of two textures
						int pow2sizeW = 1;
						while (pow2sizeW < resizeX)
							pow2sizeW = pow2sizeW << 1;

						int pow2sizeH = 1;
						while (pow2sizeH < resizeY)
							pow2sizeH = pow2sizeH << 1;

						if ((pow2sizeW != resizeX) || (pow2sizeH != resizeY))
						{
							// resize on smaller power of two
							needResize = true;
							resizeX = pow2sizeW >> 1;
							resizeY = pow2sizeH >> 1;
						}
					}

					if (needResize)
					{
						toload->FastResize(resizeX, resizeY);
					}

					result = CreateFromImage(toload);
				}

				return result;

			}
		}
		else
		{
			return true;
		}

	}
	return false;
}
