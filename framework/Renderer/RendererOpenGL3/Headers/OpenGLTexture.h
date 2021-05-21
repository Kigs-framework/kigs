#ifndef _OPENGLTEXTURE_H
#define _OPENGLTEXTURE_H

#include "Texture.h"

struct OpenGLTexturePlatformImpl;
// ****************************************
// * OpenGLTexture class
// * --------------------------------------
/**
 * \file	OpenGLTexture.h
 * \class	OpenGLTexture
 * \ingroup Renderer
 * \brief	OpenGL implementation of Texture.
 */
 // ****************************************
class OpenGLTexture : public Texture
{
public:
	DECLARE_CLASS_INFO(OpenGLTexture,Texture,Renderer)
	OpenGLTexture(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	virtual	~OpenGLTexture();

	bool	PreDraw(TravState*) override;
	bool	PostDraw(TravState* travstate) override;

	unsigned int	GetGLID() { return mTextureGLIndex; }
protected:

	void	InitForFBO() override;
	void	InitModifiable() override;
	void	UninitModifiable()override;
	bool	Load()override;
	bool	CreateFromImage(const SmartPointer<TinyImage>& image, bool directInit=false) override;
//	bool	CreateFromBuffer(unsigned char* buffer, unsigned int width, unsigned int height, TinyImage::ImageFormat format, unsigned char* palettebuffer = 0, unsigned int palattedatasize = 0) override;
	bool	UpdateBufferZone(unsigned char* bitmapbuffer, const BBox2DI& zone, const Point2DI& bitmapSize) override;
	bool	CreateFromText(const unsigned short* text, unsigned int fontSize = 16, const char* fontName = "arial.ttf", unsigned int a_Alignment = 1, float R = 0, float G = 0, float B = 0, float A = 1, TinyImage::ImageFormat format = TinyImage::RGBA_32_8888, int a_drawingLimit = -1) override;
	bool	CreateFromText(const unsigned short* text, unsigned int _maxLineNumber = 0, unsigned int maxSize = 128, unsigned int fontSize = 16, const char* fontName = "arial.ttf", unsigned int a_Alignment = 1, float R = 0, float G = 0, float B = 0, float A = 1, TinyImage::ImageFormat format = TinyImage::RGBA_32_8888, int a_drawingLimit = -1) override;

	bool	CubeMapGeneration();
	bool	ManagePow2Buffer(unsigned int w, unsigned int h, unsigned int aPixSize);
	bool	CanUseDynamicTexture(TinyImage::ImageFormat format);
	bool	UseDynamicTexture(unsigned char* buffer, unsigned int width, unsigned int height, TinyImage::ImageFormat format, bool needRealloc);

#ifdef WIN32
	void	GetPixels(unsigned int* _array)override;
	void	SetPixels(unsigned int* _array, int Width, int Height)override;
#endif

	unsigned int  mTextureGLIndex;
	
	bool mCanReuseBuffer;

	unsigned int	mPow2BufferSize;
	unsigned char*	mPow2Buffer = nullptr;

	OpenGLTexturePlatformImpl * mPrivatePointer;

#ifdef SUPPORT_ETC_TEXTURE
	unsigned int	mIDETCAlphaTexture;
#endif
};

#endif //_OPENGLTEXTURE_H
