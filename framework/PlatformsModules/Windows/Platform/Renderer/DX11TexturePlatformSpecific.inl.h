#include <d3d11.h>

namespace Kigs
{
	namespace Draw
	{

		// special case for cube map
		// generate file names from base name
		bool	DX11Texture::CubeMapGeneration()
		{
			return true;
		}

		bool	DX11Texture::UpdateBufferZone(unsigned char* bitmapbuffer, const BBox2DI& zone, const Point2DI& bitmapSize)
		{
			RendererDX11* renderer = static_cast<RendererDX11*>(ModuleRenderer::mTheGlobalRenderer);
			auto pd3dDevice = renderer->getDXInstance()->mDevice.Get(); // Don't forget to initialize this

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			auto pd3dContext = renderer->getDXInstance()->mDeviceContext; // Don't forget to initialize this
			auto res = pd3dContext->Map(mTexturePointer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			memcpy(mappedResource.pData, bitmapbuffer, mPow2Width * mPow2Height * 4);
			pd3dContext->Unmap(mTexturePointer, 0);

			return true;
		}

		///// DYNAMIC TEXTURE

		bool			DX11Texture::CanUseDynamicTexture(Pict::TinyImage::ImageFormat format)
		{
			return false;
		}
		bool			DX11Texture::UseDynamicTexture(unsigned char* buffer, unsigned int width, unsigned int height, Pict::TinyImage::ImageFormat format, bool needRealloc)
		{
			return true;
		}

		void	DX11Texture::GetPixels(unsigned int* _array)
		{
		}

		void	DX11Texture::SetPixels(unsigned int* _array, int Width, int Height)
		{
		}

	}
}