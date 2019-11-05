
#include "ModuleRenderer.h"
#include "DX11RenderingScreen.h"
#include "math.h"
#include "Window.h"
#include "Core.h"
#include "RendererDX11.h"
#include "FilePathManager.h"
#include "UIVerticesInfo.h"

#include "TimeProfiler.h"

#include "DX11Texture.h"
#include "TextureFileManager.h"

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <wrl/client.h>

#ifdef WUP
#include "Platform/Main/BaseApp.h"

#include <winrt/Windows.Graphics.Holographic.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <windows.graphics.directx.direct3d11.interop.h>

using namespace winrt::Windows::Graphics::Holographic;
using namespace winrt::Windows::Graphics::DirectX::Direct3D11;

inline winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface CreateDepthTextureInteropObject(
	const Microsoft::WRL::ComPtr<ID3D11Texture2D> spTexture2D)
{
	// Direct3D interop APIs are used to provide the buffer to the WinRT API.
	Microsoft::WRL::ComPtr<IDXGIResource1> depthStencilResource;
	winrt::check_hresult(spTexture2D.As(&depthStencilResource));
	Microsoft::WRL::ComPtr<IDXGISurface2> depthDxgiSurface;
	winrt::check_hresult(depthStencilResource->CreateSubresourceSurface(0, &depthDxgiSurface));
	winrt::com_ptr<::IInspectable> inspectableSurface;
	winrt::check_hresult(
		CreateDirect3D11SurfaceFromDXGISurface(
			depthDxgiSurface.Get(),
			inspectableSurface.put()
		));

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
		mySizeX = (unsigned int)sizeX;
		mySizeY = (unsigned int)sizeY;
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

	if(!m_renderTargetView.Get())
		CreateResources();

	setCurrentContext();

	ParentClassType::SetActive(state);
	
	RendererDX11* renderer = static_cast<RendererDX11*>(RendererDX11::theGlobalRenderer);
	renderer->SetCullMode(RENDERER_CULL_NONE);
	return true;
}

void DX11RenderingScreen::Release(TravState* state)
{
	RendererDX11* renderer = static_cast<RendererDX11*>(ModuleRenderer::theGlobalRenderer);
	renderer->GetVertexBufferManager()->DoDelayedAction();

	if (myUseFBO)
	{
		DX11RenderingScreen* firstScreen = (DX11RenderingScreen*)renderer->getFirstRenderingScreen();
		firstScreen->SetActive(state);

		if (!myIsOffScreen && state)
		{
			renderer->pushShader((API3DShader*)renderer->getDefaultUiShader(), state);

			unsigned int shader_flag = ModuleRenderer::VERTEX_ARRAY_MASK | ModuleRenderer::NO_LIGHT_MASK;
			shader_flag |= ModuleRenderer::TEXCOORD_ARRAY_MASK;

			renderer->GetActiveShader()->ChooseShader(state, shader_flag);

			renderer->SetBlendMode(RENDERER_BLEND_OFF);
			renderer->SetLightMode(RENDERER_LIGHT_OFF);
			renderer->SetDepthTestMode(false);
			renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_OFF);
			renderer->Ortho(MATRIX_MODE_PROJECTION, 0.0f, (float)mySizeX, 0.0f, (float)mySizeY, -1.0f, 1.0f);
			renderer->LoadIdentity(MATRIX_MODE_MODEL);
			renderer->LoadIdentity(MATRIX_MODE_VIEW);
			renderer->ActiveTextureChannel(DX11_COLOR_MAP_SLOT);
			myFBOTexture->DoPreDraw(state);
			//renderer->BindTexture(RENDERER_TEXTURE_2D, fbo_texture_id);
			renderer->SetScissorValue(0, 0, mySizeX, mySizeY);
			renderer->Viewport(0, 0, mySizeX, mySizeY);

			VInfo2D vi;
			UIVerticesInfo mVI = UIVerticesInfo(&vi);
			VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(mVI.Buffer());

			buf[0].setVertex(0.0f, 0.0f);
			buf[1].setVertex(0.0, (float)mySizeY);
			buf[2].setVertex((float)mySizeX, 0.0);
			buf[3].setVertex((float)mySizeX, (float)mySizeY);

			//Draw Quad that fits the screen

			buf[0].setTexUV(0.0f, 0.0f);
			buf[1].setTexUV(0.0f, 1.0f);
			buf[2].setTexUV(1.0f, 0.0f);
			buf[3].setTexUV(1.0f, 1.0f);


			mVI.SetFlag(UIVerticesInfo_Vertex | UIVerticesInfo_Texture);

			renderer->DrawUIQuad(state, &mVI);

			renderer->popShader((API3DShader*)renderer->getDefaultUiShader(), state);
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
	RendererDX11* renderer = RendererDX11::theGlobalRenderer->as<RendererDX11>();
	
	if (!myIsOffScreen && !myUseFBO)
	{
		if (!myIsStereo)
		{
			// Present the back buffer to the screen since rendering is complete.
			if (myWaitVSync)
			{
				// Lock to screen refresh rate.
				renderer->getDXInstance()->m_swapChain->Present(1, 0);
			}
			else
			{
				// Present as fast as possible.
				renderer->getDXInstance()->m_swapChain->Present(0, 0);
			}
		}
		else
		{
#ifdef WUP
			
			renderer->getDXInstance()->mCurrentRenderingParameters.CommitDirect3D11DepthBuffer(CreateDepthTextureInteropObject(m_depthStencilBuffer));
			auto result = renderer->getDXInstance()->mCurrentFrame.PresentUsingCurrentPrediction(myWaitVSync ? HolographicFramePresentWaitBehavior::WaitForFrameToFinish : HolographicFramePresentWaitBehavior::DoNotWaitForFrameToFinish);
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
		ModuleSpecificRenderer* renderer = RendererDX11::theGlobalRenderer;
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
	CoreModifiableAttribute* newAttr = AddDynamicAttribute(BOOL, "DelayedInit");
	newAttr->setValue(true);
#endif
}

void DX11RenderingScreen::DelayedInit()
{
	ModuleSpecificRenderer* renderer = RendererDX11::theGlobalRenderer; // ((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	DX11RenderingScreen* firstScreen = (DX11RenderingScreen*)renderer->getFirstRenderingScreen();

	if (firstScreen != this)
	{
		if (firstScreen->myIsInit == false)
		{
			firstScreen->DelayedInit();
		}
	}

	ParentClassType::InitModifiable();

	myIsInit = true;

	RemoveDynamicAttribute("DelayedInit");

	if (firstScreen == this)
	{
		bool isFullScreen = false;
		if (!myhWnd)
		{
			if (MyParentWindow && MyParentWindow->IsInit())
			{
				myhWnd = (HWND)MyParentWindow->GetHandle();
				MyParentWindow->getValue("FullScreen", isFullScreen);
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
	KIGS_ASSERT(!(!myUseFBO && myIsOffScreen)); // offscreen must be fbo

	bool is_main_screen = !myIsOffScreen && !myUseFBO;

	RendererDX11* renderer = reinterpret_cast<RendererDX11*>(ModuleRenderer::theGlobalRenderer);
	DXInstance* dxinstance = renderer->getDXInstance();
#ifdef WUP
	if (myIsStereo && !gIsHolographic) myIsStereo = false;
#else
	if (myIsStereo) myIsStereo = false;
#endif
	bool is_stereo = myIsStereo;

#ifdef WUP
	if (is_stereo && !dxinstance->mCurrentRenderingParameters)
	{
		return false;
	}
#endif

	int wanted_x = mySizeX;
	int wanted_y = mySizeY;
	
	if (myUseFBO)
	{
		wanted_x = myFBOSizeX;
		wanted_y = myFBOSizeY;
	}

	if (wanted_x <= 0) wanted_x = 1280;
	if (wanted_y <= 0) wanted_y = 960;
	
	if (is_main_screen)
	{
		if (is_stereo)
		{
#ifdef WUP
			auto surface = dxinstance->mCurrentRenderingParameters.Direct3D11BackBuffer();			
			wanted_x = surface.Description().Width;
			wanted_y = surface.Description().Height;
			mySizeX = wanted_x;
			mySizeY = wanted_y;
			RecomputeDesignCoef();
			ComPtr<ID3D11Texture2D> cameraBackBuffer;
			DX::ThrowIfFailed(surface.as<::Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>()->GetInterface(IID_PPV_ARGS(cameraBackBuffer.GetAddressOf())));

			if (cameraBackBuffer.Get() != m_renderTargetBuffer.Get())
			{
				m_renderTargetBuffer = cameraBackBuffer;
			}
			else
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

			dxinstance->m_swapChain.Reset();

			// First, retrieve the underlying DXGI Device from the D3D Device.
			ComPtr<IDXGIDevice1> dxgiDevice;
			DX::ThrowIfFailed(dxinstance->m_device.As(&dxgiDevice));

			// Identify the physical adapter (GPU or card) this device is running on.
			ComPtr<IDXGIAdapter> dxgiAdapter;
			DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

			// And obtain the factory object that created it.
			ComPtr<IDXGIFactory2> dxgiFactory;
			DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

#ifdef WUP
			auto windowPtr = static_cast<::IUnknown*>(winrt::get_abi(App::GetApp()->GetWindow()));
			DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForCoreWindow(
				dxinstance->m_device.Get(),
				windowPtr,
				&swapChainDesc,
				nullptr,
				dxinstance->m_swapChain.GetAddressOf()
			));
#else
			DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
				dxinstance->m_device.Get(),
				myhWnd,
				&swapChainDesc,
				nullptr,
				nullptr,
				dxinstance->m_swapChain.GetAddressOf()
			));
#endif		
			m_renderTargetBuffer.Reset();
			DX::ThrowIfFailed(dxinstance->m_swapChain->GetBuffer(0, IID_PPV_ARGS(m_renderTargetBuffer.GetAddressOf())));
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
		
		m_renderTargetBuffer.Reset();
		DX::ThrowIfFailed(dxinstance->m_device->CreateTexture2D(&colorBufferDesc, NULL, m_renderTargetBuffer.GetAddressOf()));
		if (myFBOTexture) myFBOTexture->Destroy();
		TextureFileManager* texfileManager = (TextureFileManager*)KigsCore::GetSingleton("TextureFileManager");
		myFBOTexture = (Texture*)texfileManager->CreateTexture(getName());
		myFBOTexture->setValue("Width", wanted_x);
		myFBOTexture->setValue("Height", wanted_y);
		myFBOTexture->InitForFBO();
		myFBOTexture->SetRepeatUV(false, false);
		myFBOTexture->as<DX11Texture>()->SetD3DTexture(m_renderTargetBuffer.Get());
	}

	m_renderTargetView.Reset();
	DX::ThrowIfFailed(dxinstance->m_device->CreateRenderTargetView(m_renderTargetBuffer.Get(), NULL, m_renderTargetView.GetAddressOf()));

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

	if (myBitsForStencil > 0)
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
	m_depthStencilBuffer.Reset();
	DX::ThrowIfFailed(dxinstance->m_device->CreateTexture2D(&depthBufferDesc, NULL, m_depthStencilBuffer.GetAddressOf()));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = depth_stencil_view_format;
	depthStencilViewDesc.ViewDimension = is_stereo ? D3D11_DSV_DIMENSION_TEXTURE2DARRAY : D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	depthStencilViewDesc.Texture2DArray.ArraySize = is_stereo ? 2 : 1;

	// Create the depth stencil view.
	m_depthStencilView.Reset();
	DX::ThrowIfFailed(dxinstance->m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, m_depthStencilView.GetAddressOf()));

	return true;
}

void DX11RenderingScreen::setCurrentContext()
{
	RendererDX11* renderer = reinterpret_cast<RendererDX11*>(ModuleRenderer::theGlobalRenderer);
	DXInstance* dxinstance = renderer->getDXInstance();

	if (myUseFBO)
	{
		ID3D11ShaderResourceView* shader_res = nullptr;
		dxinstance->m_deviceContext->PSSetShaderResources(0, 1, &shader_res);
	}

	dxinstance->m_currentRenderTarget = m_renderTargetView;
	dxinstance->m_currentDepthStencilTarget = m_depthStencilView;
	ID3D11RenderTargetView* render_targets[] = { m_renderTargetView.Get() };
	dxinstance->m_deviceContext->OMSetRenderTargets(1, render_targets, m_depthStencilView.Get());
	dxinstance->m_isFBORenderTarget = myUseFBO;
}

void DX11RenderingScreen::ReleaseResources()
{
	m_renderTargetBuffer.Reset();
	m_renderTargetView.Reset();
	m_depthStencilBuffer.Reset();
	m_depthStencilView.Reset();
}