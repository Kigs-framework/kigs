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
	, mTextureGLIndex((u32)-1)
	, mCanReuseBuffer(false)
	, mPow2BufferSize(0)
	, mPow2Buffer(NULL)
	, mPrivatePointer(NULL)
#ifdef SUPPORT_ETC_TEXTURE
	, mIDETCAlphaTexture(0)
#endif
{
}

OpenGLTexture::~OpenGLTexture()
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

	if (mTextureGLIndex != (u32)-1)
	{
		ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
		scenegraph->AddDefferedItem((void*)mTextureGLIndex, DefferedAction::DESTROY_TEXTURE);
		//kigsprintf("del texture %d - %s\n", mTextureGLIndex, getName().c_str());
	}
	//ModuleRenderer::mTheGlobalRenderer->DeleteTexture(1, &mTextureGLIndex);
#ifdef SUPPORT_ETC_TEXTURE
	if (mIDETCAlphaTexture)
	{
		ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
		scenegraph->AddDefferedItem((void*)mIDETCAlphaTexture, DefferedAction::DESTROY_TEXTURE);
		//	ModuleRenderer::mTheGlobalRenderer->DeleteTexture(1, &mIDETCAlphaTexture);
	}
#endif

	if (mPrivatePointer)
		delete mPrivatePointer;

	if (mPow2Buffer)
		delete[] mPow2Buffer;
}

void	OpenGLTexture::InitModifiable()
{
	if (_isInit)
		return;

	mCanReuseBuffer = false;

	Texture::InitModifiable();
}


void	OpenGLTexture::UninitModifiable()
{
	if (!_isInit)
		return;
	// get TextureIndex
	if (!isUserFlagSet(isDirtyContext) && mTextureGLIndex != (u32)-1)
	{
		ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
		scenegraph->AddDefferedItem((void*)mTextureGLIndex, DefferedAction::DESTROY_TEXTURE);
		//ModuleRenderer::mTheGlobalRenderer->DeleteTexture(1, &mTextureGLIndex);
	}
	mTextureGLIndex = (u32)-1;


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
	ModuleRenderer::mTheGlobalRenderer->CreateTexture(1, &mTextureGLIndex);
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

	if (mTextureGLIndex == (u32)-1)
	{
		return false;
	}


	if (Texture::PreDraw(travstate))
	{
		RendererTextureType type;
		switch ((int)mTextureType)
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


		
		RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer);
		
#ifdef SUPPORT_ETC_TEXTURE
		// check for separate alpha texture
		if (mIDETCAlphaTexture)
		{
			renderer->ActiveTextureChannel(1);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, mIDETCAlphaTexture);
			renderer->ActiveTextureChannel(0);
		}
#endif

		renderer->BindTexture(type, mTextureGLIndex);
		renderer->EnableTexture();

		renderer->TextureParameteri(type, RENDERER_TEXTURE_WRAP_S, (mRepeat_U) ? RENDERER_REPEAT : RENDERER_CLAMP_TO_EDGE);
		renderer->TextureParameteri(type, RENDERER_TEXTURE_WRAP_T, (mRepeat_V) ? RENDERER_REPEAT : RENDERER_CLAMP_TO_EDGE);
		renderer->TextureParameteri(type, RENDERER_TEXTURE_MIN_FILTER, mForceNearest ? RENDERER_NEAREST : ((mHasMipmap) ? RENDERER_LINEAR_MIPMAP_LINEAR : RENDERER_LINEAR));
		
		// Is it a problem to have RENDERER_TEXTURE_MAG_FILTER set too ? 
		renderer->TextureParameteri(type, RENDERER_TEXTURE_MAG_FILTER, mForceNearest ? RENDERER_NEAREST : ((mHasMipmap) ? RENDERER_LINEAR_MIPMAP_LINEAR : RENDERER_LINEAR));

		return true;
	}
	return false;
}

bool	OpenGLTexture::PostDraw(TravState* travstate)
{
	if (mTextureGLIndex == -1)
	{
		return false;
	}
	ModuleSpecificRenderer* renderer = travstate->GetRenderer();
	if (Texture::PostDraw(travstate))
	{
#ifdef SUPPORT_ETC_TEXTURE
		if (mIDETCAlphaTexture)
		{
			renderer->ActiveTextureChannel(1);
			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			renderer->ActiveTextureChannel(0);
		}
#endif

		RendererTextureType type;
		switch ((int)mTextureType)
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
		renderer->UnbindTexture(type, mTextureGLIndex);
		renderer->DisableTexture();

		return true;
	}
	return false;
}

bool OpenGLTexture::ManagePow2Buffer(u32 aWidth, u32 aHeight, u32 aPixSize)
{
	int currentSize = mPow2Width * mPow2Height * mPixelSize;
	int newSize = aWidth * aHeight * aPixSize;
	
	bool reinit = (newSize * 4) < currentSize;

	if ((aWidth <= mWidth) && (aHeight <= mHeight) && (aPixSize <= mPixelSize) && (!reinit) )
		return false;

	if (reinit)
	{
		mPow2Width = 1;
		mPow2Height = 1;
	}

	// get pow2 size
	while (mPow2Width < aWidth)
	{
		mPow2Width = mPow2Width << 1;
		mCanReuseBuffer = false;
	}
	while (mPow2Height < aHeight)
	{
		mPow2Height = mPow2Height << 1;
		mCanReuseBuffer = false;
	}

	// if buffer already in pow2, do not use pow2 buffer
	if (aWidth == mPow2Width && aHeight == mPow2Height)
	{
		mCanReuseBuffer = false;
		if (mPow2Buffer)
			delete[] mPow2Buffer;
		mPow2Buffer = nullptr;
		return false;
	}

	//create pow2buffer
	if (!mCanReuseBuffer)
	{
		// delete older if exist
		if (mPow2Buffer)
			delete[] mPow2Buffer;

		mPow2BufferSize = mPow2Width*mPow2Height*aPixSize;
		mPow2Buffer = new unsigned char[mPow2BufferSize];
		memset(mPow2Buffer, 0, mPow2BufferSize);
		return true;
	}

	return false;
}

bool	OpenGLTexture::CreateFromImage(const SmartPointer<TinyImage>& image, bool directInit)
{
	if (!image)
		return false;

	unsigned char * data;

	TinyImage::ImageFormat format = image->GetFormat();

	RendererOpenGL* renderer = static_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer);
	bool needRealloc = false;
	

	if (!directInit)
	{
		mTextureType = TEXTURE_2D;

		u32 pixSize = TinyImage::GetPixelValueSize(image->GetFormat());


		if (pixSize > 0)
		{
			needRealloc = ManagePow2Buffer(image->GetWidth(), image->GetHeight(), pixSize);
		}
		else // compressed texture
		{
			mPow2Width = image->GetWidth();
			mPow2Height = image->GetHeight();
		}

		mPixelSize = pixSize;
		mWidth = image->GetWidth();
		mHeight = image->GetHeight();
		int line_size = image->GetPixelLineSize();
		
		if (line_size != mWidth * mPixelSize) 
			mCanReuseBuffer = false;

		if (mPow2Buffer && !mCanReuseBuffer && !CanUseDynamicTexture(image->GetFormat()))
		{
			unsigned char* pos = (unsigned char*)mPow2Buffer;
			unsigned char* posRead = (unsigned char*)image->GetPixelData();

			
			int lLineW = line_size;//mWidth*mPixelSize;
			for (u32 j = 0; j < mHeight; j++)
			{
				memcpy(pos, posRead, lLineW);
				pos += mPow2Width*mPixelSize;
				posRead += line_size; // mWidth*mPixelSize;
			}

			data = mPow2Buffer;
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

		if (mPow2Buffer && !mCanReuseBuffer && !CanUseDynamicTexture(image->GetFormat()))
		{
			data = mPow2Buffer;
		}
		else
			data = image->GetPixelData();
	}

	if (mTextureGLIndex == (u32)-1)
	{
		// get TextureIndex
		ModuleRenderer::mTheGlobalRenderer->CreateTexture(1, &mTextureGLIndex);
	}
	renderer->ActiveTextureChannel(0);
	renderer->BindTexture(RENDERER_TEXTURE_2D, mTextureGLIndex);
	bool success = false;

	renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MIN_FILTER, mForceNearest ? RENDERER_NEAREST :  RENDERER_LINEAR);

	bool mipmap_generated = false;

	if (CanUseDynamicTexture(image->GetFormat()))
	{
		success = UseDynamicTexture(data, mWidth, mHeight, image->GetFormat(), needRealloc);
	}
	renderer->FlushState();
	if (!success)
	{
		switch (image->GetFormat())
		{
		case TinyImage::AI88:
		{
			if (mCanReuseBuffer)
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, (GLsizei)mPow2Width, (GLsizei)mPow2Height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
			mTransparencyType = 2;
			break;
		}
		case TinyImage::ABGR_16_1555_DIRECT_COLOR:
		{
			// create tmp RGBA buffer and convert

			unsigned char*	newdata = new unsigned char[mWidth*mHeight * 4];
			u32 i;
			unsigned char* pos = newdata;
			unsigned short* posRead = (unsigned short*)data;
			for (i = 0; i < mWidth*mHeight; i++)
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

			if (mCanReuseBuffer)
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mPow2Width, mPow2Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, newdata);
			mTransparencyType = 1;
			delete[] newdata;
		}
		break;

		case TinyImage::PALETTE16_256_COLOR:
		{
			unsigned short*	newdata = new unsigned short[mWidth*mHeight];
			unsigned short* palette = (unsigned short*)image->GetPaletteData();
			u32	colorCount = image->GetPaletteDataSize() / 2; // 1 unsigned short by color

															  // set from ABGR to BGRA
															  // alpha = 0 for index 0
			palette[0] = (palette[0] << 1);
			// alpha = 1 for the others
			for (u32 index = 1; index < colorCount; index++)
				palette[index] = (palette[index] << 1) | 0x1;

			// replace index by associate color
			for (u32 index = 0; index < mWidth*mHeight; index++)
				newdata[index] = palette[data[index]];
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, newdata);
			mTransparencyType = 1;
			delete[] newdata;

		}
		break;
		case TinyImage::PALETTE16_16_COLOR:
		{
			unsigned short*	newdata = new unsigned short[mWidth*mHeight];
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
			for (index = 0; index < mWidth*mHeight; index++)
			{
				unsigned char p1 = data[index] & 0x0f;
				unsigned char p2 = data[index] >> 4;

				// 2 pixel per char
				newdata[2 * index] = palette[p1];
				newdata[2 * index + 1] = palette[p2];
			}
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, newdata);
			mTransparencyType = 1;
			delete[] newdata;
		}
		break;

		case TinyImage::RGB_16_565:
			if (mCanReuseBuffer)
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mPow2Width, mPow2Height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data);
			mTransparencyType = 0;
			break;
		case TinyImage::RGBA_16_5551:
			if (mCanReuseBuffer)
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mPow2Width, mPow2Height, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, data);
			mTransparencyType = 1;
			break;
		case TinyImage::RGBA_32_8888:
			if (mCanReuseBuffer)
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mPow2Width, mPow2Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

			if (mTransparencyType == -1)
			{
				//! TODO
				mTransparencyType = 2;
			}
			break;
		case TinyImage::RGB_24_888:
			if (mCanReuseBuffer)
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mPow2Width, mPow2Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			mTransparencyType = 0;
			break;
		case TinyImage::GREYSCALE:
			if (mCanReuseBuffer)
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, mPow2Width, mPow2Height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
			mTransparencyType = 0;
			break;
		case TinyImage::ALPHA_8:
			if (mCanReuseBuffer)
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_ALPHA, GL_UNSIGNED_BYTE, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, mPow2Width, mPow2Height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
			mTransparencyType = 2;
			break;
		
#ifdef SUPPORT_ETC_TEXTURE
		case TinyImage::ETC1:
		{
#ifndef WIN32
			u32 L_TotalSize = (mPow2Width*mPow2Height) / 2;
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, (GLsizei)mPow2Width, (GLsizei)mPow2Height, 0, L_TotalSize, data);
			mTransparencyType = 0;
#else
			// TODO
#endif
			break;
		}
		case TinyImage::ETC1A8:
		{
#ifndef WIN32

			u32 L_TotalSize = (mPow2Width*mPow2Height) / 2;
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, (GLsizei)mPow2Width, (GLsizei)mPow2Height, 0, L_TotalSize, data);


			if (mIDETCAlphaTexture == 0)
			{
				// get TextureIndex
				ModuleRenderer::mTheGlobalRenderer->CreateTexture(1, &mIDETCAlphaTexture);
			}

			if (mIDETCAlphaTexture) // if alpha texture, apply same params
			{
				glBindTexture(GL_TEXTURE_2D, mIDETCAlphaTexture);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				unsigned char* alphastart = (unsigned char*)data;
				alphastart += L_TotalSize;
				glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, (GLsizei)mPow2Width, (GLsizei)mPow2Height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, alphastart);
				mTransparencyType = 2;
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
			u32 L_TotalSize = (mPow2Width*mPow2Height) / 2;
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB8_ETC2, (GLsizei)mPow2Width, (GLsizei)mPow2Height, 0, L_TotalSize, data);
			mTransparencyType = 0;
			break;
		}
		case TinyImage::ETC2A8:
		{
			u32 L_TotalSize = (mPow2Width*mPow2Height);
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA8_ETC2_EAC, (GLsizei)mPow2Width, (GLsizei)mPow2Height, 0, L_TotalSize, data);
			mTransparencyType = 2;

			break;
		}

#endif
#ifdef SUPPORT_S3TC_TEXTURE
		case TinyImage::BC1:
		case TinyImage::BC2:
		case TinyImage::BC3:

		{
			int openGLFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

			mTransparencyType = 0;

			if (format == TinyImage::BC1)
			{
				openGLFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			}
			else if (format == TinyImage::BC2)
			{
				openGLFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				mTransparencyType = 2;
			}
			else if (format == TinyImage::BC3)
			{
				openGLFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				mTransparencyType = 2;
			}

			//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			if (((bool)mHasMipmap) && image->getMipMapCount())
				//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MIN_FILTER, RENDERER_LINEAR_MIPMAP_LINEAR);
			else
			{
				// force no mipmap
				mHasMipmap = false;

				//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MIN_FILTER, RENDERER_LINEAR);
			}

			if (((bool)mHasMipmap) && image->getMipMapCount())
			{
				mipmap_generated = true;
				int mipsizex = mWidth;
				int mipsizey = mHeight;
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

				glCompressedTexImage2D(GL_TEXTURE_2D, 0, openGLFormat, (GLsizei)mWidth, (GLsizei)mHeight, 0, image->GetPixelDataSize(), image->GetPixelData());
			}
			break;
		}
#endif
		default:
			KIGS_ERROR("Texture::CreateFromImage -> color mode unknown", 1);
			return false;
	}
}

	if (mHasMipmap && !mipmap_generated)
	{
		renderer->TextureParameteri(RENDERER_TEXTURE_2D, RENDERER_TEXTURE_MIN_FILTER, RENDERER_LINEAR_MIPMAP_LINEAR);
		renderer->FlushState();
		glGenerateMipmap(GL_TEXTURE_2D);
	}


	ComputeRatio();

	mCanReuseBuffer = true;
	return true;

}

bool OpenGLTexture::CreateFromText(const unsigned short* text, u32 fontSize, const char* fontName, u32 a_Alignment, float R, float G, float B, float A, TinyImage::ImageFormat format, int a_drawingLimit)
{
	return CreateFromText(text, 0, 0, fontSize, fontName, a_Alignment, R, G, B, A, format,  a_drawingLimit);
}

bool OpenGLTexture::CreateFromText(const unsigned short* text, u32 _maxLineNumber, u32 maxSize, u32 fontSize, const char* fontName, u32 a_Alignment, float R, float G, float B, float A, TinyImage::ImageFormat format, int a_drawingLimit)
{
	mIsText = true;
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


		if (!ModuleSpecificRenderer::mDrawer->IsInCache(fontName))
		{
			auto pathManager = KigsCore::Singleton<FilePathManager>();
			SmartPointer<FileHandle> fullfilenamehandle;

			if (pathManager)
			{
				fullfilenamehandle = pathManager->FindFullName(fontName);
			}
			if ((fullfilenamehandle->mStatus&FileHandle::Exist) == 0)
				return false;

			u64 size;
			auto L_Buffer = ModuleFileManager::LoadFile(fullfilenamehandle.get(), size);
			if (L_Buffer)
			{
				unsigned char* pBuffer = (unsigned char*)L_Buffer->CopyBuffer();
				ModuleSpecificRenderer::mDrawer->SetFont(fontName, pBuffer, size, fontSize);
			}
			else
				break;
		}
		else
		{
			ModuleSpecificRenderer::mDrawer->SetFont(fontName, 0, 0, fontSize);
		}

		int L_Width = 0;
		int L_Height = 0;
		pImageData = ModuleSpecificRenderer::mDrawer->DrawTextToImage(text, textSize, L_Width, L_Height, (TextAlignment)a_Alignment, false, _maxLineNumber, maxSize, a_drawingLimit, (unsigned char)R, (unsigned char)G, (unsigned char)B);

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

		SmartPointer<TinyImage>	img = TinyImage::CreateImage(pImageData, L_Width, L_Height, TinyImage::RGBA_32_8888);

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

	if ((int)mTextureType == TEXTURE_CUBE_MAP)
	{
		return CubeMapGeneration();
	}
	else
	{
		auto pathManager = KigsCore::Singleton<FilePathManager>();

		kstl::string fileName = mFileName.const_ref();

		if (fileName != "")
		{
			SmartPointer<FileHandle> lFile = pathManager->FindFullName(fileName);
			if (!lFile)
				return false;

			SmartPointer<TinyImage> toload = TinyImage::CreateImage(lFile.get());
			if (toload)
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
					if (mForcePow2)
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
