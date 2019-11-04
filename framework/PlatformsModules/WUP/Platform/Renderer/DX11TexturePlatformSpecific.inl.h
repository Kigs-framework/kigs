#include <d3d11.h>

// special case for cube map
// generate file names from base name
bool	DX11Texture::CubeMapGeneration()
{
	return true;
}

bool	DX11Texture::UpdateBufferZone(unsigned char* bitmapbuffer, const BBox2DI& zone, const Point2DI& bitmapSize)
{
	return true;
}

///// DYNAMIC TEXTURE

bool			DX11Texture::CanUseDynamicTexture(TinyImage::ImageFormat format)
{
	return false;
}
bool			DX11Texture::UseDynamicTexture(unsigned char* buffer, unsigned int width, unsigned int height, TinyImage::ImageFormat format, bool needRealloc)
{
	return true;
}

void	DX11Texture::GetPixels(unsigned int* _array)
{
}

void	DX11Texture::SetPixels(unsigned int* _array, int Width, int Height)
{
}