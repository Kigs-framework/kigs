#include "Platform/Core/PlatformCore.h"
#include "Platform/Core/KigsJavaIDManager.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "Platform/Renderer/AndroidGraphicBufferBase.inl.h"
#include "Platform/Renderer/AndroidGraphicBuffer.inl.h"
//#include "Platform/Renderer/AndroidGraphicBufferN.inl.h"
#define AndroidGraphicBufferN AndroidGraphicBuffer

struct OpenGLTexturePlatformImpl
{
	OpenGLTexturePlatformImpl(uint32_t width, uint32_t height, uint32_t usage, TinyImage::ImageFormat format)
	{
		if (KigsJavaIDManager::OsVersion <= 23)
		{
			buffers[0] = new AndroidGraphicBuffer(width, height, usage, format);
			buffers[1] = new AndroidGraphicBuffer(width, height, usage, format);
		}
		else
		{
			buffers[0] = new AndroidGraphicBufferN(width, height, usage, format);
			buffers[1] = new AndroidGraphicBufferN(width, height, usage, format);
		}
	}

	~OpenGLTexturePlatformImpl()
	{
		delete buffers[0];
		delete buffers[1];
	}

	AndroidGraphicBufferBase * buffers[2];
	int current_buffer = 0;
};


	

// special case for cube map
// generate file names from base name
bool	OpenGLTexture::CubeMapGeneration()
{
	bool result=true;
	SP<FilePathManager>	pathManager=KigsCore::GetSingleton("FilePathManager");
	std::string fullfilename;
	char asciiCount[16];
	asciiCount[0]=0;

	// get extension
	std::string	extension=mFileName;
	extension=extension.substr(extension.rfind("."));
	// remove extension
	std::string basefilename=mFileName;
	basefilename=basefilename.substr(0,basefilename.length()-extension.length());

	// check if all 6 textures are ok
	int index;
	for(index=0;index<6;index++)
	{
		std::string filename=basefilename;
		filename+="_";
		sprintf(asciiCount,"%d",(index+1));
		filename+=asciiCount;
		filename+=extension;
		
		SmartPointer<FileHandle> fullfilenamehandle = pathManager->FindFullName(filename);
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

bool	OpenGLTexture::UpdateBufferZone(unsigned char* bitmapbuffer, const BBox2DI& zone, const Point2DI& bitmapSize)
{
	RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer); 
	renderer->BindTexture(RENDERER_TEXTURE_2D, mTextureGLIndex);
	renderer->FlushState();
	Point2DI	zonesize = zone.Size();

	glTexSubImage2D(GL_TEXTURE_2D, 0, zone.m_Min.x, zone.m_Min.y, zonesize.x, zonesize.y, GL_RGBA, GL_UNSIGNED_BYTE, bitmapbuffer);
	return true;
}


///// DYNAMIC TEXTURE

bool			OpenGLTexture::CanUseDynamicTexture(TinyImage::ImageFormat format)
{
	if (!mIsDynamic)
		return false;
	
	//kigsprintf("os version %d\n", KigsJavaIDManager::OsVersion);
	if(KigsJavaIDManager::OsVersion>23) // android N and newer
	return false;

	switch (format)
	{
	case TinyImage::ImageFormat::RGBA_32_8888:
	case TinyImage::ImageFormat::RGB_24_888:
	case TinyImage::ImageFormat::RGB_16_565:
		return true;
		break;
	default:
		break;
	}

	return false;
}
bool			OpenGLTexture::UseDynamicTexture(unsigned char* abuffer, unsigned int awidth, unsigned int aheight, TinyImage::ImageFormat aformat, bool needRealloc)
{
	mTransparencyType = 2; 

	AndroidGraphicBufferBase * lAGB = nullptr;
	if (mPrivatePointer)
	{
		if (needRealloc)
		{
			delete mPrivatePointer;
			mPrivatePointer = new OpenGLTexturePlatformImpl(mPow2Width, mPow2Height, AndroidGraphicBuffer::UsageSoftwareWrite | AndroidGraphicBuffer::UsageTexture, aformat);
		}
	}
	else
	{
		mPrivatePointer = new OpenGLTexturePlatformImpl(mPow2Width, mPow2Height, AndroidGraphicBufferBase::UsageSoftwareWrite | AndroidGraphicBufferBase::UsageTexture, aformat);
	}
	lAGB = mPrivatePointer->buffers[mPrivatePointer->current_buffer];
	mPrivatePointer->current_buffer = 1 - mPrivatePointer->current_buffer;

	if(false== lAGB->Bind())
		return false;

	unsigned char * buffer;
	if(0!= lAGB->Lock(AndroidGraphicBufferBase::UsageSoftwareWrite, &buffer))
		return false;

	int pixSize = TinyImage::GetPixelValueSize(aformat);
	//memcpy(buffer, abuffer, myPow2BufferSize); 

	unsigned char* pos = (unsigned char*)buffer;
	unsigned char* posRead = (unsigned char*)abuffer;
	
	ANativeWindow_Buffer * handle= lAGB->getNativeWindowsHandle();

	if (awidth == handle->stride)
	{
		memcpy(pos, posRead, awidth * pixSize * aheight);
	}
	else
	{
		int lLineW = awidth * pixSize;
		for (unsigned int j = 0; j < aheight; j++)
		{
			memcpy(pos, posRead, lLineW);
			pos += handle->stride * pixSize;
			posRead += lLineW;
		}
	}

	lAGB->Unlock();
	
	return true;
}