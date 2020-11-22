#include "PrecompiledHeaders.h"

#include "math.h"
#include "FilePathManager.h"
#include "Core.h"
#include "ModuleRenderer.h"
#include "TravState.h"

#include "DX11Texture.h"
#include "RendererDefines.h"

#include "TinyImage.h"
#include "FreeType_TextDrawer.h"
#include "RendererDX11.h"
#include "ModuleFileManager.h"
#include "FilePathManager.h"

#include "ModuleSceneGraph.h"

#include "Platform/Renderer/DX11TexturePlatformSpecific.inl.h"
#include "TGAClass.h"


#ifdef JAVASCRIPT
extern "C" void		JSDrawMultilineTextUTF16(const unsigned short* TextToWrite, int maxLineNumber, int maxWidth, u32 FontSize, const char* FontName, u32 a_Alignment, u32 jumpedLines, int R, int G, int B, int A, int& width, int& height);
extern "C" void		JSInitImageData(void* pixels, int width, int height);
#endif

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

IMPLEMENT_CLASS_INFO(DX11Texture)

DX11Texture::DX11Texture(const std::string& name, CLASS_NAME_TREE_ARG)
	: Texture(name, PASS_CLASS_NAME_TREE_ARG)
	, mCanReuseBuffer(false)
	, mPow2BufferSize(0)
	, mPow2Buffer(NULL)

{
}

DX11Texture::~DX11Texture()
{
	// release d3d object
	if (mTexturePointer)
		mTexturePointer->Release();
	if (mShaderRes)
		mShaderRes->Release();

	mTexturePointer = nullptr;
	mShaderRes = nullptr;

	if (mPow2Buffer)
		delete[] mPow2Buffer;
}

void DX11Texture::InitModifiable()
{
	if (_isInit)
		return;

	mCanReuseBuffer = false;

	Texture::InitModifiable();
}

void DX11Texture::ProtectedDestroy()
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

void DX11Texture::UninitModifiable()
{
	if (!_isInit)
		return;

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

bool DX11Texture::PreDraw(TravState* travstate)
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

	if (Texture::PreDraw(travstate))
	{
#ifdef WUP
		// winrt uses get and WRL uses Get
		auto pd3dContext = ModuleRenderer::mTheGlobalRenderer->as<RendererDX11>()->getDXInstance()->mDeviceContext.get(); // Don't forget to initialize this
#else
		// winrt uses get and WRL uses Get
		auto pd3dContext = ModuleRenderer::mTheGlobalRenderer->as<RendererDX11>()->getDXInstance()->mDeviceContext.Get(); // Don't forget to initialize this
#endif
		// Set shader texture resource in the pixel shader.
		pd3dContext->PSSetShaderResources(ModuleRenderer::mTheGlobalRenderer->as<RendererDX11>()->GetActiveTextureChannel(), 1, &mShaderRes);
		ModuleRenderer::mTheGlobalRenderer->as<RendererDX11>()->SetSampler(mRepeat_U, mRepeat_V, mForceNearest);

		return true;
	}
	return false;
}

bool DX11Texture::PostDraw(TravState* travstate)
{
	if (Texture::PostDraw(travstate))
	{
		return true;
	}
	return false;
}

bool DX11Texture::ManagePow2Buffer(u32 aWidth, u32 aHeight, u32 aPixSize)
{
	if (aWidth <= mWidth && aHeight <= mHeight && aPixSize <= mPixelSize)
		return false;

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

		mPow2BufferSize = mPow2Width * mPow2Height*aPixSize;
		mPow2Buffer = new unsigned char[mPow2BufferSize];
		memset(mPow2Buffer, 0, mPow2BufferSize);
		return true;
	}

	return false;
}

bool DX11Texture::CreateFromImage(const SmartPointer<TinyImage>& image, bool directInit)
{
	if (image.isNil())
		return false;

	unsigned char* data;

	TinyImage::ImageFormat format = image->GetFormat();

	RendererDX11* renderer = static_cast<RendererDX11*>(ModuleRenderer::mTheGlobalRenderer);
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

		// compute ratio before delayed load and after delayed load
		// so that texture ratio can be used just after initialization
		ComputeRatio();

		int line_size = image->GetPixelLineSize();

		if (line_size != mPow2Width * mPixelSize)
			mCanReuseBuffer = false;

		if (mPow2Buffer && !mCanReuseBuffer && !CanUseDynamicTexture(image->GetFormat()))
		{
			unsigned char* pos = (unsigned char*)mPow2Buffer;
			unsigned char* posRead = (unsigned char*)image->GetPixelData();


			int lLineW = line_size;//mWidth*mPixelSize;
			for (u32 j = 0; j < mHeight; j++)
			{
				memcpy(pos, posRead, lLineW);
				pos += mPow2Width * mPixelSize;
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

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = mPow2Width;
	desc.Height = mPow2Height;
	desc.ArraySize = 1;
	desc.MipLevels = 1;// mHasMipmap ? 0 : 1;
	desc.SampleDesc.Quality = 0;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	unsigned char* converteddata = nullptr;

	// setting up D3D11_SUBRESOURCE_DATA 
	std::vector<D3D11_SUBRESOURCE_DATA> subresources;
	subresources.resize(1);

	subresources[0].pSysMem = (void*)data;
	subresources[0].SysMemPitch = mPow2Width * mPixelSize;
	subresources[0].SysMemSlicePitch = 0; // Not needed since this is a 2d texture

	bool has_mipmaps = false;
	bool is_bc = false;
	int bpe = 0;
	switch (image->GetFormat())
	{
	case TinyImage::AI88:
	{
		// no AI88 texture for DirectX11
		// Convert to 32 bit rgba
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		mTransparencyType = 2;
		mPixelSize = 4;
		converteddata = new unsigned char[mPow2Width * mPow2Height * 4];
		subresources[0].pSysMem = (void*)converteddata;
		subresources[0].SysMemPitch = mPow2Width * 4;
		subresources[0].SysMemSlicePitch = 0; // Not needed since this is a 2d texture

		// pointer on file pixel data 
		u8* pixels = converteddata;

		// pointer on image pixel data
		u8* imagedata = data;

		unsigned int totalSize = mPow2Width * mPow2Height;
		for (u32 i = 0; i < totalSize; i++, imagedata += 2, pixels += 4)
		{
			pixels[0] = pixels[1] = pixels[2] = imagedata[0];
			pixels[3] = imagedata[1];
			/*pixels[0] = imagedata[0];
			pixels[1] = pixels[2] = pixels[3] = imagedata[1];*/
		}

		data = converteddata;
	}
	break;

	case TinyImage::RGB_16_565:
	case TinyImage::RGBA_16_5551:
	case TinyImage::RGBA_32_8888:
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		mTransparencyType = 2;
		break;
	case TinyImage::BC1:
		is_bc = true;
		desc.Format = DXGI_FORMAT_BC1_UNORM;
		has_mipmaps = (mHasMipmap && image->getMipMapCount() > 0);
		mTransparencyType = 0;
		bpe = 8;
		break;
	case TinyImage::BC2:
		is_bc = true;
		desc.Format = DXGI_FORMAT_BC2_UNORM;
		has_mipmaps = (mHasMipmap && image->getMipMapCount() > 0);
		mTransparencyType = 2;
		bpe = 16;
		break;
	case TinyImage::BC3:
		is_bc = true;
		desc.Format = DXGI_FORMAT_BC3_UNORM;
		has_mipmaps = (mHasMipmap && image->getMipMapCount() > 0);
		mTransparencyType = 2;
		bpe = 16;
		break;
	case TinyImage::RGB_24_888:
	{
		// no 24 bit texture for DirectX11
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		mTransparencyType = 0;
		mPixelSize = 4;
		converteddata = new unsigned char[mPow2Width * mPow2Height * 4];
		subresources[0].pSysMem = (void*)converteddata;
		subresources[0].SysMemPitch = mPow2Width * 4;
		subresources[0].SysMemSlicePitch = 0; // Not needed since this is a 2d texture
		ReadColorFunc currentReadFunc = TGAClass::GetReadFunction(TinyImage::RGBA_32_8888, 3, false);

		// pointer on file pixel data 
		u8* pixels = converteddata;

		// pointer on image pixel data
		u8* imagedata = data;

		unsigned int totalSize = mPow2Width * mPow2Height;
		for (u32 i = 0; i < totalSize; i++, imagedata += 3, pixels += 4)
		{
			currentReadFunc(imagedata, pixels);
		}

		data = converteddata;
	}
	break;
	case TinyImage::ABGR_16_1555_DIRECT_COLOR:
	case TinyImage::PALETTE16_256_COLOR:
	case TinyImage::PALETTE16_16_COLOR:
	case TinyImage::GREYSCALE:
	case TinyImage::ALPHA_8:
	default:
		KIGS_ERROR("Texture::CreateFromImage -> color mode unknown", 1);
		return false;
	}


	auto get_surface_info = [](int w, int h, int bpe)
	{
		uint64_t numBlocksWide = 0;
		if (w > 0)
		{
			numBlocksWide = std::max<uint64_t>(1u, (uint64_t(w) + 3u) / 4u);
		}
		uint64_t numBlocksHigh = 0;
		if (h > 0)
		{
			numBlocksHigh = std::max<uint64_t>(1u, (uint64_t(h) + 3u) / 4u);
		}

		int rowBytes = numBlocksWide * bpe;
		int numRows = numBlocksHigh;
		int numBytes = rowBytes * numBlocksHigh;

		return std::make_tuple(rowBytes, numRows, numBytes);
	};

	if (is_bc)
	{
		if (!has_mipmaps)
		{
			auto [rowBytes, numRows, numBytes] = get_surface_info(mWidth, mHeight, bpe);
			subresources[0].SysMemPitch = rowBytes;
		}
		else
		{
			desc.MipLevels = image->getMipMapCount();

			int mipsizex = mWidth;
			int mipsizey = mHeight;

			unsigned char* mipdatastart = image->GetPixelData();
			subresources.resize(image->getMipMapCount());
			for (int mipmapC = 0; mipmapC < image->getMipMapCount(); mipmapC++)
			{
				auto& subres = subresources[mipmapC];

				auto [rowBytes, numRows, numBytes] = get_surface_info(mipsizex, mipsizey, bpe);
				subres.pSysMem = mipdatastart;
				subres.SysMemPitch = rowBytes;
				subres.SysMemSlicePitch = 0;

				mipdatastart += numBytes;
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
			}
		}
	}
#ifdef WUP
	auto pd3dDevice = renderer->getDXInstance()->mDevice.get(); // Don't forget to initialize this
#else
	auto pd3dDevice = renderer->getDXInstance()->mDevice.Get(); // Don't forget to initialize this
#endif
	HRESULT res = pd3dDevice->CreateTexture2D(&desc, subresources.data(), &mTexturePointer);
	if (FAILED(res))
	{
		if (converteddata)
			delete[] converteddata;
		return false;
	}

	/*D3D11_MAPPED_SUBRESOURCE mappedResource;
	auto *pd3dContext = ModuleRenderer::mTheGlobalRenderer->as<RendererDX11>()->getDXInstance()->mDeviceContext; // Don't forget to initialize this
	res = pd3dContext->Map(mTexturePointer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	memcpy(mappedResource.pData, data, mPow2Width*mPow2Height * 4);
	pd3dContext->Unmap(mTexturePointer, 0);*/

	// create shader resources
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Format = desc.Format;
	HRESULT hr = pd3dDevice->CreateShaderResourceView(mTexturePointer, &srvDesc, &mShaderRes);

	/*if (srvDesc.Texture2D.MipLevels > 1)
	{
		renderer->getDXInstance()->mDeviceContext->GenerateMips(mShaderRes);
	}*/

	if (converteddata)
		delete[] converteddata;

	// compute ratio before delayed load and after delayed load
	// so that texture ratio can be used just after initialization
	ComputeRatio();

	mCanReuseBuffer = true;
	return true;
	}




	

bool DX11Texture::CreateFromText(const unsigned short* text, unsigned int fontSize, const char* fontName, unsigned int a_Alignment, float R, float G, float B, float A, TinyImage::ImageFormat format, int a_drawingLimit) 
{
	return CreateFromText(text, 0, 0, fontSize, fontName, a_Alignment, R, G, B, A, format, a_drawingLimit);
}

bool DX11Texture::CreateFromText(const unsigned short* text, unsigned int _maxLineNumber, unsigned int maxSize, unsigned int fontSize, const char* fontName, unsigned int a_Alignment, float R, float G, float B, float A, TinyImage::ImageFormat format, int a_drawingLimit)
{
	mIsText = true;
	bool bRet = false;
	unsigned char * pImageData = 0;

	// freetype size ratio
	fontSize = (int)(((float)fontSize)*FREETYPE_SIZE_COEFF);

	do
	{
		if (!text)
			break;

		int textSize = 0;
		while (text[textSize] != 0)
			textSize++;


		if (!RendererDX11::myDrawer->IsInCache(fontName))
		{
			SP<FilePathManager> L_PathManager = KigsCore::GetSingleton("FilePathManager");
			SmartPointer<FileHandle> fullfilenamehandle;

			if (L_PathManager)
			{
				fullfilenamehandle = L_PathManager->FindFullName(fontName);
			}
			if ((fullfilenamehandle->mStatus&FileHandle::Exist) == 0)
				return false;

			u64 size;
			CoreRawBuffer* L_Buffer = ModuleFileManager::LoadFile(fullfilenamehandle.get(), size);
			if (L_Buffer)
			{
				unsigned char* pBuffer = (unsigned char*)L_Buffer->CopyBuffer();
				RendererDX11::myDrawer->SetFont(fontName, pBuffer, size, fontSize);
				L_Buffer->Destroy();
			}
			else
				break;
		}
		else
		{
			RendererDX11::myDrawer->SetFont(fontName, 0, 0, fontSize);
		}

		int L_Width = 0;
		int L_Height = 0;
		pImageData = RendererDX11::myDrawer->DrawTextToImage(text, textSize, L_Width, L_Height, (TextAlignment)a_Alignment, false, _maxLineNumber, maxSize, a_drawingLimit, (unsigned char)R, (unsigned char)G, (unsigned char)B);

		if (!pImageData)
			break;

		SmartPointer<TinyImage>	img = OwningRawPtrToSmartPtr(TinyImage::CreateImage(pImageData, L_Width, L_Height, TinyImage::RGBA_32_8888));

		if (!CreateFromImage(img))
			break;

		bRet = true;
	} while (0);

	if (pImageData)
		delete[] pImageData;

	return bRet;
}

bool DX11Texture::Load()
{
	if ((int)mTextureType == TEXTURE_CUBE_MAP)
	{
		return CubeMapGeneration();
	}
	else
	{
		SP<FilePathManager>	pathManager = KigsCore::GetSingleton("FilePathManager");

		std::string fileName = mFileName.const_ref();
		auto extdot = fileName.rfind('.');
		if (extdot != std::string::npos)
		{
			std::string ext = fileName.substr(extdot);
			if ((ext == ".etc") || (ext == ".etx") || (ext == ".etz"))
			{
				fileName = fileName.substr(0, fileName.rfind('.'));
				fileName += ".png";
			}
		}
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

void DX11Texture::SetD3DTexture(ID3D11Texture2D* texture)
{
	if (mTexturePointer) mTexturePointer->Release();
	if (mShaderRes) mShaderRes->Release();

	mTexturePointer = nullptr;
	mShaderRes = nullptr;
	if (!texture) return;

	RendererDX11* renderer = static_cast<RendererDX11*>(ModuleRenderer::mTheGlobalRenderer);

	mTexturePointer = texture;
	texture->AddRef();

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DX::ThrowIfFailed(renderer->getDXInstance()->mDevice->CreateShaderResourceView(mTexturePointer, &srvDesc, &mShaderRes));
}