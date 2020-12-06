#pragma once

#include "RenderingScreen.h"


#include <d3d11_1.h>
#include <dxgi1_2.h>

#ifdef WUP
#include <winrt/Windows.Graphics.Holographic.h>
#else
#include <wrl/client.h>
#endif

// ****************************************
// * DX11RenderingScreen class
// * --------------------------------------
/**
 * \file	DX11RenderingScreen.h
 * \class	DX11RenderingScreen
 * \ingroup Renderer
 * \brief	DX11 implementation of RenderingScreen.
 *
 */
 // ****************************************
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

	bool IsMainRenderingScreen() { return !mUseFBO && !mIsOffScreen; }

#ifdef WUP
	void SetRenderingParameters(winrt::Windows::Graphics::Holographic::HolographicCameraRenderingParameters params) { mRenderingParameters = params; }
#endif
protected:
	DECLARE_METHOD(Snapshot);
	COREMODIFIABLE_METHODS(Snapshot);

	void DelayedInit();
	void ReleaseResources();

	void Update(const Timer&  timer, void* /*addParam*/) override;

	void InitModifiable() override;
	void UninitModifiable() override;
	
	void setCurrentContext() override;

	HDC   mhDC = NULL;
	HGLRC mHRC = NULL;
	HWND  mHWnd = NULL;

	maInt mDepthBufferCount = BASE_ATTRIBUTE(DepthBufferCount, 1);
	maInt mActiveDepthBuffer = BASE_ATTRIBUTE(ActiveDepthBuffer, 0);

#ifdef WUP

	maInt mHoloCommittedDepthBuffer = BASE_ATTRIBUTE(HoloCommittedDepthBuffer, 0);

	winrt::com_ptr<ID3D11Texture2D>			mRenderTargetBuffer;
	winrt::com_ptr<ID3D11RenderTargetView>	mRenderTargetView;

	std::vector<winrt::com_ptr<ID3D11Texture2D>>			mDepthStencilBuffers;
	std::vector<winrt::com_ptr<ID3D11DepthStencilView>>		mDepthStencilViews;
#else
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			mRenderTargetBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	mRenderTargetView;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>>		mDepthStencilBuffers;
	std::vector<Microsoft::WRL::ComPtr<ID3D11DepthStencilView>>	mDepthStencilViews;
#endif
	unsigned int mCurrentFrameNumber = -1;
#ifdef WUP
	winrt::Windows::Graphics::Holographic::HolographicCameraRenderingParameters mRenderingParameters = nullptr;
#endif
};

