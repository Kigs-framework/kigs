#pragma once
#ifndef BASE_APP_IMPORT
#include "winrt/Windows.ApplicationModel.Core.h"
#include "winrt/Windows.UI.Core.h"
#include "winrt/Windows.Graphics.Holographic.h"
#include "winrt/Windows.Perception.Spatial.h"

#include "TecLibs/Tec3D.h"

#include <optional>
#include <string>
#include <vector>
#include <set>

/*
template <typename T>
T^ to_cx(winrt::Windows::Foundation::IUnknown const& from)
{
	return safe_cast<T^>(reinterpret_cast<Platform::Object^>(winrt::get_abi(from)));
}
*/

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

	const std::set<winrt::Windows::Graphics::Holographic::HolographicCamera>& GetCameras() const { return mCameras; }

private:
	static App* sApp;


	void OnHolographicDisplayIsAvailableChanged(winrt::Windows::Foundation::IInspectable, winrt::Windows::Foundation::IInspectable);


	bool mWindowClosed = false;
	bool mWindowVisible = true;

	int panelWidth = 1280;
	int panelHeight = 960;

	winrt::Windows::UI::Core::CoreWindow mWindow{ nullptr };

	winrt::Windows::Graphics::Holographic::HolographicSpace mHolographicSpace{ nullptr };
	winrt::Windows::Perception::Spatial::SpatialLocator mSpatialLocator{ nullptr };
	winrt::Windows::Perception::Spatial::SpatialStationaryFrameOfReference mStationaryReferenceFrame{ nullptr };

	std::set<winrt::Windows::Graphics::Holographic::HolographicCamera> mCameras;

};

extern bool gIsHolographic;

#endif
#define NEXTBIM_EXPLORER_STATIC
#ifdef NEXTBIM_EXPLORER_STATIC
#define DECL_SPEC
#else
#ifdef BASE_APP_IMPORT
#define DECL_SPEC __declspec(dllimport)
#else
#define DECL_SPEC __declspec(dllexport)
#endif
#endif

DECL_SPEC void wupmain();