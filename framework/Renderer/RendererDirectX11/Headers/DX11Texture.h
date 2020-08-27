#ifndef _DX11TEXTURE_H
#define _DX11TEXTURE_H

#include "Texture.h"

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;


// ****************************************
// * DX11Texture class
// * --------------------------------------
/**
 * \file	DX11Texture.h
 * \class	DX11Texture
 * \ingroup Renderer
 * \brief	DX11 implementation of Texture.
 *
 */
 // ****************************************
class DX11Texture : public Texture
{
public:
	DECLARE_CLASS_INFO(DX11Texture,Texture,Renderer)

	DX11Texture(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	bool	PreDraw(TravState*) override;
	bool	PostDraw(TravState* travstate) override;
	   	
	void	SetD3DTexture(ID3D11Texture2D* texture);
protected:
	virtual ~DX11Texture();

	void	ProtectedDestroy() override;	
	void	InitModifiable() override;
	void	UninitModifiable()override;

	bool	Load()override;
	bool	CreateFromImage(const SmartPointer<TinyImage>& image, bool directInit=false) override;
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

	bool mCanReuseBuffer;

	unsigned int	mPow2BufferSize;
	unsigned char*	mPow2Buffer = nullptr;

	ID3D11Texture2D *mTexturePointer = nullptr;
	ID3D11ShaderResourceView* mShaderRes = nullptr;
};

#endif //_DX11TEXTURE_H
