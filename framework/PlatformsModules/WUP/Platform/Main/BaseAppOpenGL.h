#pragma once
#ifndef BASE_APP_IMPORT
#include "Platform/Main/angle_includes.h"

#include "winrt/Windows.ApplicationModel.Core.h"
#include "winrt/Windows.UI.Core.h"
#include "winrt/Windows.Graphics.Holographic.h"
#include "winrt/Windows.Perception.Spatial.h"

#include "TecLibs/Tec3D.h"

#include <optional>
#include <string>
#include <vector>

__declspec(dllimport) void AngleHolographicGetCurrentSwapChainDimensions(int *w, int *h, float *scaleFactor);

template <typename T>
T^ to_cx(winrt::Windows::Foundation::IUnknown const& from)
{
	return safe_cast<T^>(reinterpret_cast<Platform::Object^>(winrt::get_abi(from)));
}

struct App : winrt::implements<App, winrt::Windows::ApplicationModel::Core::IFrameworkViewSource, winrt::Windows::ApplicationModel::Core::IFrameworkView>
{
public:
	winrt::Windows::ApplicationModel::Core::IFrameworkView CreateView()
	{
		return *this;
	}
	
	App() { sApp = this; }

	static App* GetApp() { return sApp; }

	void Initialize(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& view);
	void SetWindow(winrt::Windows::UI::Core::CoreWindow const& window);
	void Load(const winrt::hstring&);
	void Run();
	void Uninitialize();

	void Swap();

	winrt::Windows::UI::Core::CoreWindow GetWindow() { return mWindow; }

	int GetWindowWidth() { return panelWidth; }
	int GetWindowHeight() { return panelHeight; }

	winrt::Windows::Graphics::Holographic::HolographicSpace GetHolographicSpace() { return mHolographicSpace; }
	winrt::Windows::Perception::Spatial::SpatialStationaryFrameOfReference GetStationaryReferenceFrame() { return mStationaryReferenceFrame; }

	// Transform from old frame of reference to new frame of reference
	std::optional<mat3x4> CreateNewFrameOfReference();

private:
	static App* sApp;

	void InitializeEGL(winrt::Windows::Graphics::Holographic::HolographicSpace const& holographicSpace);
	void InitializeEGL(winrt::Windows::UI::Core::CoreWindow const& window);
	void InitializeEGLInner(winrt::Windows::Foundation::IInspectable const& windowBasis);

	void CleanupEGL();

	bool mWindowClosed = false;
	bool mWindowVisible = true;

	EGLDisplay mEglDisplay = EGL_NO_DISPLAY;
	EGLContext mEglContext = EGL_NO_CONTEXT;
	EGLSurface mEglSurface = EGL_NO_SURFACE;

	EGLint panelWidth = 0;
	EGLint panelHeight = 0;

	winrt::Windows::UI::Core::CoreWindow mWindow{ nullptr };

	winrt::Windows::Graphics::Holographic::HolographicSpace mHolographicSpace{ nullptr };
	winrt::Windows::Perception::Spatial::SpatialStationaryFrameOfReference mStationaryReferenceFrame{ nullptr };
};


//extern std::vector<unsigned char> GetDataFromIBuffer(::Windows::Storage::Streams::IBuffer^ buf);

extern bool gIsHolographic;
/*
namespace UWPKigs
{
	ref class App sealed : public Windows::ApplicationModel::Core::IFrameworkView
    {
    public:
        App();

        // IFrameworkView Methods.
        virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
        virtual void SetWindow(CoreWindow^ window);
        virtual void Load(Platform::String^ entryPoint);
        virtual void Run();
        virtual void Uninitialize();

		void ChangeOrigin(Windows::Perception::Spatial::SpatialStageFrameOfReference^ ref);
		
		int GetWindowWidth() { return panelWidth; }
		int GetWindowHeight() { return panelHeight; }
		
		CoreWindow^ GetWindow() { return TheWindow.Get(); }
		Windows::Graphics::Holographic::HolographicSpace^ GetHolographicSpace() { return mHolographicSpace; }
		Windows::Perception::Spatial::SpatialStationaryFrameOfReference^ GetStationaryReferenceFrame() { return mStationaryReferenceFrame; }
		void Swap();

    private:
        void RecreateRenderer();

        // Application lifecycle event handlers.
        void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);

		void OnFileActivated(Windows::ApplicationModel::Activation::FileActivatedEventArgs^ e);

        // Window event handlers.
        void OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args);
        void OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args);
		void OnLocatabilityChanged(Windows::Perception::Spatial::SpatialLocator^  sender, Object^ args);

        void InitializeEGL(Windows::Graphics::Holographic::HolographicSpace^ holographicSpace);
        void InitializeEGL(CoreWindow^ window);
        void App::InitializeEGLInner(Object^ windowBasis);
        void CleanupEGL();

        bool mWindowClosed;
        bool mWindowVisible;
        
        EGLDisplay mEglDisplay;
        EGLContext mEglContext;
        EGLSurface mEglSurface;

		EGLint panelWidth = 0;
		EGLint panelHeight = 0;

		Platform::Agile<CoreWindow^> TheWindow;
		
        // The holographic space the app will use for rendering.
        Windows::Graphics::Holographic::HolographicSpace^ mHolographicSpace = nullptr;

        // The world coordinate system. In this example, a reference frame placed in the environment.
        Windows::Perception::Spatial::SpatialStationaryFrameOfReference^ mStationaryReferenceFrame = nullptr;
    };
}
*/
//extern UWPKigs::App^ TheApp;

#endif

#ifdef BASE_APP_IMPORT
#define DECL_SPEC __declspec(dllimport)
#else
#define DECL_SPEC __declspec(dllexport)
#endif

DECL_SPEC void wupmain();