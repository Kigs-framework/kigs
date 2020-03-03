#pragma once

#include "RenderingScreen.h"

// rendering screen is too much platform specific
//#include "Platform/Renderer/DX11RenderingScreen.h"

#ifdef WUP
#include <winrt/Windows.Graphics.Holographic.h>
#endif

class DX11RenderingScreen : public RenderingScreen
{
public:
	DECLARE_CLASS_INFO(DX11RenderingScreen, RenderingScreen, Renderer);
	DECLARE_INLINE_CONSTRUCTOR(DX11RenderingScreen){}
	
	bool SetActive(TravState* state) override;
	void Release(TravState* state) override;

	void Resize(kfloat sizeX, kfloat sizeY) override;


	void FetchPixels(int x, int y, int width, int height, void *pRGBAPixels) override;
	void FetchDepth(int x, int y, int width, int height, float *pDepthPixels) override;
	void FetchDepth(int x, int y, int width, int height, unsigned int *pDepthPixels) override;

	void ScreenShot(char * filename);

	virtual void SetWindowByHandle(void* PtrToHandle) override {}
	virtual void* GetContextHandle() override { return nullptr; }

	bool CreateResources();

	bool IsMainRenderingScreen() { return !myUseFBO && !myIsOffScreen; }

	void SetRenderingParameters(winrt::Windows::Graphics::Holographic::HolographicCameraRenderingParameters params) { mRenderingParameters = params; }

protected:
	DECLARE_METHOD(Snapshot);
	COREMODIFIABLE_METHODS(Snapshot);

	void DelayedInit();
	void ReleaseResources();

	void Update(const Timer&  timer, void* /*addParam*/) override;

	void InitModifiable() override;
	void UninitModifiable() override;
	
	void setCurrentContext() override;

	HDC   myhDC = NULL;
	HGLRC myhRC = NULL;
	HWND  myhWnd = NULL;

	winrt::com_ptr<ID3D11Texture2D>			m_renderTargetBuffer;
	winrt::com_ptr<ID3D11RenderTargetView>	m_renderTargetView;
	winrt::com_ptr<ID3D11Texture2D>			m_depthStencilBuffer;
	winrt::com_ptr<ID3D11DepthStencilView>	m_depthStencilView;
	
	unsigned int mCurrentFrameNumber = -1;
	winrt::Windows::Graphics::Holographic::HolographicCameraRenderingParameters mRenderingParameters = nullptr;
};

