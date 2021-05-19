#include "ModuleRenderer.h"
#include "DX11RenderingScreen.h"
#include "math.h"
#include "Window.h"
#include "Core.h"
#include "RendererDX11.h"
#include "FilePathManager.h"
#include "UIVerticesInfo.h"
#include "HLSLShader.h"
#include "TimeProfiler.h"

#include "DX11Texture.h"
#include "TextureFileManager.h"

/*#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>*/

#include <d2d1_2.h>
#include <d3d11_4.h>
#include <wrl/client.h>

#ifdef WUP
#include "Platform/Main/BaseApp.h"

#include <Windows.Graphics.DirectX.Direct3D11.Interop.h>

#include <winrt/Windows.Graphics.Holographic.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>


using namespace Microsoft::WRL;
using namespace winrt::Windows::Graphics::Holographic;
using namespace winrt::Windows::Graphics::DirectX::Direct3D11;


inline winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface CreateDepthTextureInteropObject(
	const winrt::com_ptr<ID3D11Texture2D> spTexture2D)
{
	
	// Direct3D interop APIs are used to provide the buffer to the WinRT API.
	winrt::com_ptr<IDXGIResource1> depthStencilResource = spTexture2D.as<IDXGIResource1>();
	winrt::com_ptr<IDXGISurface2> depthDxgiSurface;
	winrt::check_hresult(depthStencilResource->CreateSubresourceSurface(0, depthDxgiSurface.put()));
	winrt::com_ptr<::IInspectable> inspectableSurface;
	winrt::check_hresult(CreateDirect3D11SurfaceFromDXGISurface(
			depthDxgiSurface.get(),
			inspectableSurface.put()));
	
	return inspectableSurface.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface>();
}


#endif

using Microsoft::WRL::ComPtr;

// rendering screen is too much platform specific
//#include "Platform/Renderer/DX11RenderingScreen.inl.h"


IMPLEMENT_CLASS_INFO(DX11RenderingScreen)


void DX11RenderingScreen::FetchPixels(int x, int y, int width, int height, void *pRGBAPixels)
{
}

void DX11RenderingScreen::FetchDepth(int x, int y, int width, int height, float *pDepthPixels)
{
}

void DX11RenderingScreen::FetchDepth(int x, int y, int width, int height, unsigned int *pDepthPixels)
{
}

void DX11RenderingScreen::Resize(kfloat sizeX, kfloat sizeY)
{
	// before screen is init, only update sizeX and sizeY
	if (!IsInit())
	{
		mSize = v2f(sizeX,sizeY);
	}
	else
	{
		ReleaseResources();
		CreateResources();
	}

	RecomputeDesignCoef();

	EmitSignal(Signals::Resized);
}

bool DX11RenderingScreen::SetActive(TravState* state)
{
	CoreModifiableAttribute* delayed = getAttribute("DelayedInit");
	if (delayed)
	{
		DelayedInit();
	}

/*
	auto frame_number = state->GetFrameNumber();
	if (frame_number != mCurrentFrameNumber)
	{
		bool is_main_screen = !mIsOffScreen && !mUseFBO;
		RendererDX11* renderer = reinterpret_cast<RendererDX11*>(ModuleRenderer::mTheGlobalRenderer);
		DXInstance* dxinstance = renderer->getDXInstance();
		auto space = dxinstance->mHolographicSpace;

		if (is_main_screen && space) 
		{
			CreateResources();
		}
		mCurrentFrameNumber = frame_number;
	}
*/

	setCurrentContext();

	ParentClassType::SetActive(state);
	
	RendererDX11* renderer = static_cast<RendererDX11*>(ModuleRenderer::mTheGlobalRenderer);
	renderer->SetCullMode(RENDERER_CULL_NONE);
	return true;
}

void DX11RenderingScreen::Release(TravState* state)
{
	RendererDX11* renderer = static_cast<RendererDX11*>(ModuleRenderer::mTheGlobalRenderer);
	renderer->GetVertexBufferManager()->DoDelayedAction();

	if (mUseFBO)
	{
		DX11RenderingScreen* firstScreen = (DX11RenderingScreen*)renderer->getFirstRenderingScreen();
		firstScreen->SetActive(state);

		if (!mIsOffScreen && state)
		{
			renderer->pushShader((API3DShader*)renderer->getDefaultUiShader().get(), state);

			unsigned int shader_flag = ModuleRenderer::VERTEX_ARRAY_MASK | ModuleRenderer::NO_LIGHT_MASK;
			shader_flag |= ModuleRenderer::TEXCOORD_ARRAY_MASK;

			renderer->GetActiveShader()->ChooseShader(state, shader_flag);

			renderer->SetBlendMode(RENDERER_BLEND_OFF);
			renderer->SetLightMode(RENDERER_LIGHT_OFF);
			renderer->SetDepthTestMode(false);
			renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_OFF);
			renderer->Ortho(MATRIX_MODE_PROJECTION, 0.0f, (float)mSize[0], 0.0f, (float)mSize[1], -1.0f, 1.0f);
			renderer->LoadIdentity(MATRIX_MODE_MODEL);
			renderer->LoadIdentity(MATRIX_MODE_VIEW);
			renderer->ActiveTextureChannel(DX11_COLOR_MAP_SLOT);
			mFBOTexture->DoPreDraw(state);
			//renderer->BindTexture(RENDERER_TEXTURE_2D, fbo_texture_id);
			renderer->SetScissorValue(0, 0, mSize[0], mSize[1]);
			renderer->Viewport(0, 0, mSize[0], mSize[1]);

			VInfo2D vi;
			UIVerticesInfo mVI = UIVerticesInfo(&vi);
			VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(mVI.Buffer());

			buf[0].setVertex(0.0f, 0.0f);
			buf[1].setVertex(0.0, (float)mSize[1]);
			buf[2].setVertex((float)mSize[0], 0.0);
			buf[3].setVertex((float)mSize[0], (float)mSize[1]);

			//Draw Quad that fits the screen

			buf[0].setTexUV(0.0f, 0.0f);
			buf[1].setTexUV(0.0f, 1.0f);
			buf[2].setTexUV(1.0f, 0.0f);
			buf[3].setTexUV(1.0f, 1.0f);


			mVI.SetFlag(UIVerticesInfo_Vertex | UIVerticesInfo_Texture);

			renderer->DrawUIQuad(state, &mVI);

			renderer->popShader((API3DShader*)renderer->getDefaultUiShader().get(), state);
		}
	}


}

void DX11RenderingScreen::ScreenShot(char * filename)
{
}

DEFINE_METHOD(DX11RenderingScreen, Snapshot)
{
	if (privateParams)
	{
		char * filename = (char *)privateParams;
		ScreenShot(filename);
	}
	return true;
}

void DX11RenderingScreen::Update(const Timer&  timer, void* addParam)
{
	if (addParam && !*(bool*)addParam) return;

	setCurrentContext();

	ParentClassType::Update(timer, addParam);
	RendererDX11* renderer = ModuleRenderer::mTheGlobalRenderer->as<RendererDX11>();
#ifdef WUP
	if (mRenderingParameters)
	{
		mRenderingParameters.CommitDirect3D11DepthBuffer(CreateDepthTextureInteropObject(mDepthStencilBuffers[mHoloCommittedDepthBuffer]));
	}
#endif
	if (!mIsOffScreen && !mUseFBO)
	{
		if (!mIsStereo)
		{
			// Present the back buffer to the screen since rendering is complete.
			if (mVSync)
			{
				// Lock to screen refresh rate.
				renderer->getDXInstance()->mSwapChain->Present(1, 0);
			}
			else
			{
				// Present as fast as possible.
				renderer->getDXInstance()->mSwapChain->Present(0, 0);
			}
		}
		else
		{
#ifdef WUP
			auto result = renderer->getDXInstance()->mCurrentFrame.PresentUsingCurrentPrediction(mVSync ? HolographicFramePresentWaitBehavior::WaitForFrameToFinish : HolographicFramePresentWaitBehavior::DoNotWaitForFrameToFinish);
			if (result == HolographicFramePresentResult::DeviceRemoved)
			{
				kigsprintf("Device removed !\n");
			}
#endif
		}
	}
}

void DX11RenderingScreen::UninitModifiable()
{
	ParentClassType::UninitModifiable();
	if (IsMainRenderingScreen())
	{
		ModuleSpecificRenderer* renderer = ModuleRenderer::mTheGlobalRenderer;
		renderer->UninitHardwareState();
	}

}

void DX11RenderingScreen::InitModifiable()
{
	if (IsInit()) return;

#ifdef NO_DELAYED_INIT
	DelayedInit();
#else
	CoreModifiableAttribute* delayed = getAttribute("DelayedInit");
	if (delayed) // delayed init already asked
	{
		return;
	}
	// ask for delayed init
	CoreModifiableAttribute* newAttr = AddDynamicAttribute(ATTRIBUTE_TYPE::BOOL, "DelayedInit");
	newAttr->setValue(true);
#endif
}

void DX11RenderingScreen::DelayedInit()
{
	ModuleSpecificRenderer* renderer = ModuleRenderer::mTheGlobalRenderer; // ((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	DX11RenderingScreen* firstScreen = (DX11RenderingScreen*)renderer->getFirstRenderingScreen();

	if (firstScreen != this)
	{
		if (firstScreen->mIsInit == false)
		{
			firstScreen->DelayedInit();
		}
	}

	ParentClassType::InitModifiable();

	mIsInit = true;

	RemoveDynamicAttribute("DelayedInit");

	if (firstScreen == this)
	{
		bool isFullScreen = false;
		if (!mHWnd)
		{
			if (mParentWindow && mParentWindow->IsInit())
			{
				mHWnd = (HWND)mParentWindow->GetHandle();
				mParentWindow->getValue("FullScreen", isFullScreen);
			}
		}
	}

	CreateResources();

	if (firstScreen == this)
	{
		setCurrentContext();
		renderer->InitHardwareState();
	}
}

bool DX11RenderingScreen::CreateResources()
{
	KIGS_ASSERT(!(!mUseFBO && mIsOffScreen)); // offscreen must be fbo

	bool is_main_screen = !mIsOffScreen && !mUseFBO;

	RendererDX11* renderer = reinterpret_cast<RendererDX11*>(ModuleRenderer::mTheGlobalRenderer);
	DXInstance* dxinstance = renderer->getDXInstance();
#ifdef WUP
	if (mIsStereo && !gIsHolographic) mIsStereo = false;
#else
	if (mIsStereo) mIsStereo = false;
#endif
	bool is_stereo = mIsStereo;

#ifdef WUP
	if (is_stereo && !mRenderingParameters)
	{
		return false;
	}
#endif

	int wanted_x = mSize[0];
	int wanted_y = mSize[1];

	if (wanted_x <= 0) wanted_x = 1280;
	if (wanted_y <= 0) wanted_y = 960;
	
	if (is_main_screen)
	{
		if (is_stereo)
		{
#ifdef WUP
			auto surface = mRenderingParameters.Direct3D11BackBuffer();
			wanted_x = surface.Description().Width;
			wanted_y = surface.Description().Height;

			if (mSize[0] != wanted_x || mSize[1] != wanted_y)
			{
				mDepthStencilViews.clear();
			}

			mSize[0] = wanted_x;
			mSize[1] = wanted_y;
			/*myDesignSizeX = wanted_x;
			myDesignSizeY = wanted_y;*/
			RecomputeDesignCoef();
			winrt::com_ptr<ID3D11Texture2D> cameraBackBuffer;
			DX::ThrowIfFailed(surface.as<::Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>()->GetInterface(__uuidof(cameraBackBuffer), cameraBackBuffer.put_void()));

			if (cameraBackBuffer.get() != mRenderTargetBuffer.get())
			{
				mRenderTargetBuffer = cameraBackBuffer;
			}
			else if(mDepthStencilViews.size())
			{
				// No need to recreate depth stencil
				return true;
			}
#else
			KIGS_ASSERT(!"stereo rendering requires an UWP application");
#endif
		}
		else
		{
			// Create a descriptor for the swap chain.
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

			swapChainDesc.Width = wanted_x;
			swapChainDesc.Height = wanted_y;

			swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 2;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			swapChainDesc.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;
#ifndef WUP
			swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
#else
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
#endif
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;


			mRenderTargetBuffer = nullptr;
			mDepthStencilViews.clear();
			if(mFBOTexture) mFBOTexture->as<DX11Texture>()->SetD3DTexture(nullptr);

			if (dxinstance->mSwapChain)
			{
				dxinstance->mSwapChain->ResizeBuffers(2, wanted_x, wanted_y, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
#ifdef WUP
				DX::ThrowIfFailed(dxinstance->mSwapChain->GetBuffer(0, __uuidof(mRenderTargetBuffer), mRenderTargetBuffer.put_void()));
#else
				DX::ThrowIfFailed(dxinstance->mSwapChain->GetBuffer(0, __uuidof(mRenderTargetBuffer), (void**)mRenderTargetBuffer.ReleaseAndGetAddressOf()));
#endif
			}
			else
			{
#ifdef WUP
				// First, retrieve the underlying DXGI Device from the D3D Device.
				winrt::com_ptr<IDXGIDevice1> dxgiDevice = dxinstance->mDevice.as<IDXGIDevice1>();

				// Identify the physical adapter (GPU or card) this device is running on.
				winrt::com_ptr<IDXGIAdapter> dxgiAdapter;
				DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.put()));

				// And obtain the factory object that created it.
				winrt::com_ptr<IDXGIFactory2> dxgiFactory;
				DX::ThrowIfFailed(dxgiAdapter->GetParent(__uuidof(dxgiFactory), dxgiFactory.put_void()));

				auto windowPtr = static_cast<::IUnknown*>(winrt::get_abi(dxinstance->mWindow));
				DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForCoreWindow(
					dxinstance->mDevice.get(),
					windowPtr,
					&swapChainDesc,
					nullptr,
					dxinstance->mSwapChain.put()
				));
				DX::ThrowIfFailed(dxinstance->mSwapChain->GetBuffer(0, __uuidof(mRenderTargetBuffer), mRenderTargetBuffer.put_void()));
#else

				// First, retrieve the underlying DXGI Device from the D3D Device.
				Microsoft::WRL::ComPtr<IDXGIDevice1> dxgiDevice;
				DX::ThrowIfFailed(dxinstance->mDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)dxgiDevice.ReleaseAndGetAddressOf()));

				// Identify the physical adapter (GPU or card) this device is running on.
				Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
				DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.ReleaseAndGetAddressOf()));

				// And obtain the factory object that created it.
				Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
				DX::ThrowIfFailed(dxgiAdapter->GetParent(__uuidof(dxgiFactory), (void**)dxgiFactory.ReleaseAndGetAddressOf()));

				DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
					dxinstance->mDevice.Get(),
					mHWnd,
					&swapChainDesc,
					nullptr,
					nullptr,
					dxinstance->mSwapChain.GetAddressOf()
				));

				DX::ThrowIfFailed(dxinstance->mSwapChain->GetBuffer(0, __uuidof(mRenderTargetBuffer), (void**)mRenderTargetBuffer.ReleaseAndGetAddressOf()));
#endif	
			}
		}
	}
	else
	{
		CD3D11_TEXTURE2D_DESC colorBufferDesc = {};
		colorBufferDesc.Width = wanted_x;
		colorBufferDesc.Height = wanted_y;
		colorBufferDesc.MipLevels = 1;
		colorBufferDesc.ArraySize = 1;
		colorBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		colorBufferDesc.SampleDesc.Count = 1;
		colorBufferDesc.SampleDesc.Quality = 0;
		colorBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		colorBufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		colorBufferDesc.CPUAccessFlags = 0;
		colorBufferDesc.MiscFlags = 0;
		
		mRenderTargetBuffer = nullptr;
#ifdef WUP
		DX::ThrowIfFailed(dxinstance->mDevice->CreateTexture2D(&colorBufferDesc, NULL, mRenderTargetBuffer.put()));
#else
		DX::ThrowIfFailed(dxinstance->mDevice->CreateTexture2D(&colorBufferDesc, NULL, mRenderTargetBuffer.ReleaseAndGetAddressOf()));
#endif
	
		SP<TextureFileManager> texfileManager = KigsCore::GetSingleton("TextureFileManager");
		mFBOTexture = texfileManager->CreateTexture(getName());
		mFBOTexture->setValue("Width", wanted_x);
		mFBOTexture->setValue("Height", wanted_y);
		mFBOTexture->InitForFBO();
		mFBOTexture->SetRepeatUV(false, false);
#ifdef WUP
		mFBOTexture->as<DX11Texture>()->SetD3DTexture(mRenderTargetBuffer.get());
#else
		mFBOTexture->as<DX11Texture>()->SetD3DTexture(mRenderTargetBuffer.Get());
#endif
		mDepthStencilViews.clear();
	}

	mRenderTargetView = nullptr;
#ifdef WUP
	DX::ThrowIfFailed(dxinstance->mDevice->CreateRenderTargetView(mRenderTargetBuffer.get(), NULL, mRenderTargetView.put()));
#else
	DX::ThrowIfFailed(dxinstance->mDevice->CreateRenderTargetView(mRenderTargetBuffer.Get(), NULL, mRenderTargetView.ReleaseAndGetAddressOf()));
#endif

	if (mDepthStencilViews.size()) return true;

	CD3D11_TEXTURE2D_DESC depthBufferDesc = {};

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = wanted_x;
	depthBufferDesc.Height = wanted_y;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = is_stereo ? 2 : 1;

	DXGI_FORMAT depth_stencil_format = DXGI_FORMAT_R32_TYPELESS;
	DXGI_FORMAT depth_stencil_view_format = DXGI_FORMAT_D32_FLOAT;
	
	if (is_stereo)
	{
		depth_stencil_format = DXGI_FORMAT_R16_TYPELESS;
		depth_stencil_view_format = DXGI_FORMAT_D16_UNORM;
	}

	if (mBitsForStencil > 0)
	{
		depth_stencil_format = DXGI_FORMAT_R24G8_TYPELESS;
		depth_stencil_view_format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	}

	depthBufferDesc.Format = depth_stencil_format;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	if(is_stereo)
		depthBufferDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	mDepthStencilBuffers.clear();
	mDepthStencilBuffers.resize(mDepthBufferCount);
	mDepthStencilViews.clear();
	mDepthStencilViews.resize(mDepthBufferCount);

	for (int i = 0; i < mDepthBufferCount; ++i)
	{
		auto& depth_stencil_buffer = mDepthStencilBuffers[i];
		auto& depth_stencil_view = mDepthStencilViews[i];
		
#ifdef WUP
		DX::ThrowIfFailed(dxinstance->mDevice->CreateTexture2D(&depthBufferDesc, NULL, depth_stencil_buffer.put()));
#else
		DX::ThrowIfFailed(dxinstance->mDevice->CreateTexture2D(&depthBufferDesc, NULL, depth_stencil_buffer.ReleaseAndGetAddressOf()));
#endif
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = depth_stencil_view_format;
		depthStencilViewDesc.ViewDimension = is_stereo ? D3D11_DSV_DIMENSION_TEXTURE2DARRAY : D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		depthStencilViewDesc.Texture2DArray.ArraySize = is_stereo ? 2 : 1;

		// Create the depth stencil view.

#ifdef WUP
		DX::ThrowIfFailed(dxinstance->mDevice->CreateDepthStencilView(depth_stencil_buffer.get(), &depthStencilViewDesc, depth_stencil_view.put()));
#else
		DX::ThrowIfFailed(dxinstance->mDevice->CreateDepthStencilView(depth_stencil_buffer.Get(), &depthStencilViewDesc, depth_stencil_view.ReleaseAndGetAddressOf()));
#endif
	}

	return true;
}

void DX11RenderingScreen::setCurrentContext()
{
	RendererDX11* renderer = reinterpret_cast<RendererDX11*>(ModuleRenderer::mTheGlobalRenderer);
	DXInstance* dxinstance = renderer->getDXInstance();

	if (mUseFBO)
	{
		ID3D11ShaderResourceView* shader_res = nullptr;
		dxinstance->mDeviceContext->PSSetShaderResources(0, 1, &shader_res);
	}

	dxinstance->mCurrentRenderTarget = mRenderTargetView;
	dxinstance->mCurrentDepthStencilTarget = mDepthStencilViews[mActiveDepthBuffer];
#ifdef WUP
	ID3D11RenderTargetView* render_targets[] = { mRenderTargetView.get() };
	dxinstance->mDeviceContext->OMSetRenderTargets(1, render_targets, dxinstance->mCurrentDepthStencilTarget.get());
#else
	ID3D11RenderTargetView* render_targets[] = { mRenderTargetView.Get() };
	dxinstance->mDeviceContext->OMSetRenderTargets(1, render_targets, dxinstance->mCurrentDepthStencilTarget.Get());
#endif
	dxinstance->mIsFBORenderTarget = mUseFBO;
}

void DX11RenderingScreen::ReleaseResources()
{
	mRenderTargetBuffer = nullptr;
	mRenderTargetView = nullptr;
	mDepthStencilBuffers.clear();
	mDepthStencilViews.clear();
}