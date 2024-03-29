﻿#include "KigsApplication.h"
#include "BaseAppDX11.h"
#include "Core.h"
#include "CoreBaseApplication.h"
#include "FilePathManager.h"
#include "NotificationCenter.h"

#include "winrt_helpers.h"

#include <thread>
#include <mutex>
#include <future>

//#include <windows.foundation.h>
#include "winrt/Windows.System.Profile.h"
#include "winrt/Windows.ApplicationModel.h"
#include "winrt/Windows.ApplicationModel.Activation.h"
#include "winrt/Windows.ApplicationModel.Preview.Holographic.h"
#include "winrt/Windows.Storage.h"
#include "winrt/Windows.UI.ViewManagement.h"
#include "winrt/Windows.UI.Xaml.h"
#include <winrt/Windows.Foundation.Collections.h>
#include "winrt/Windows.Graphics.DirectX.Direct3D11.h"

#include <sstream>

#include <mutex>


using namespace winrt::Windows;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::ViewManagement;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::Graphics::Holographic;
using namespace winrt::Windows::Perception::Spatial;
using namespace winrt::Windows::Storage;

using namespace Kigs;
using namespace Kigs::Core;

App* App::sApp=nullptr;
bool gIsHolographic = true;
bool gIsVR = false;

std::mutex gPickedFileMutex;
winrt::Windows::Storage::StorageFile gPickedStorageFile = nullptr;

using namespace winrt::Windows::System::Profile;

#define ApplicationName(a) Stringify(a)
#define Stringify(a) #a

#include "utf8.h"

#include <Windows.h>

void DEBUG_LOG(const char* txt)
{
	static FILE* file = nullptr;
	if (!file)
	{
		auto local_folder = winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
		auto path = to_utf8(local_folder.Path().c_str());
		path += "//startup.log";
		file = fopen(path.c_str(), "ab");
	}
	if (!file) return;
	fwrite(txt, 1, strlen(txt), file);
	fflush(file);
	//fclose(file);
}

#define DEBUG_LOG_LINE DEBUG_LOG(__FUNCTION__ "@" __FILE__ ":" ApplicationName(__LINE__) "\n");

void print_context(const char* iden)
{
	APTTYPE type;
	APTTYPEQUALIFIER qualifier;
	HRESULT const result = CoGetApartmentType(&type, &qualifier);
	const char* appart = "N/A";
	if (result == S_OK)
	{
		appart = (type == APTTYPE_MTA ? "MTA" : "STA");
	}
	kigsprintf("[%s] thread:%d apartment:%s\n", iden, GetCurrentThreadId(), appart);
}

#ifdef UWP_STATIC_LIB
#define DECL_SPEC
#else
#ifdef BASE_APP_IMPORT
#define DECL_SPEC __declspec(dllimport)
#else
#define DECL_SPEC __declspec(dllexport)
#endif
#endif

void DECL_SPEC wupmain()
{
	time_t tc = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	char time_output[512];
	strftime(time_output, 512, "\nStartup on %d/%m/%y %H:%M:%S\n", std::localtime(&tc));
	DEBUG_LOG(time_output);
	DEBUG_LOG_LINE;
	winrt::init_apartment();
	CoreApplication::Run(winrt::make<App>());
	winrt::uninit_apartment();
}

// Helper to convert a length in device-independent pixels (DIPs) to a length in physical pixels.
inline float ConvertDipsToPixels(float dips, float dpi)
{
	static const float dipsPerInch = 96.0f;
	return floor(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
}


// The first method called when the IFrameworkView is being created.
void App::Initialize(CoreApplicationView const& applicationView)
{	
	DEBUG_LOG_LINE;
	Kigs::File::StorageFileFileAccess::setMainThreadID();
	using namespace winrt::Windows::ApplicationModel::Activation;

	applicationView.Activated([this](CoreApplicationView const& view, IActivatedEventArgs args)
	{
		DEBUG_LOG_LINE;
		if (args.Kind() == ActivationKind::File)
		{
			auto file_args = args.as<FileActivatedEventArgs>();
			if (file_args.Files().Size())
			{
				auto item = file_args.Files().First().Current();
				if (item.IsOfType(StorageItemTypes::File))
				{
					auto file = item.as<StorageFile>();
					{
						std::lock_guard<std::mutex> lk{ gPickedFileMutex };
						gPickedStorageFile = file;
					}
				}
			}
		}
		// Run() won't start until the CoreWindow is activated.
		CoreWindow::GetForCurrentThread().Activate();
	});
	
	CoreApplication::Suspending([this](winrt::Windows::Foundation::IInspectable sender, winrt::Windows::ApplicationModel::SuspendingEventArgs args)
	{
		if (KigsCore::Instance() && KigsCore::Instance()->GetCoreApplication())
			KigsCore::Instance()->GetCoreApplication()->EmitSignal("UWP_ApplicationSuspendedEvent");
		
		//mWindowClosed = true;
	});
}

void App::OnHolographicDisplayIsAvailableChanged(winrt::Windows::Foundation::IInspectable, winrt::Windows::Foundation::IInspectable)
{
	DEBUG_LOG_LINE;
	HolographicDisplay defaultHolographicDisplay = HolographicDisplay::GetDefault();
	if (defaultHolographicDisplay)
	{
		mSpatialLocator = defaultHolographicDisplay.SpatialLocator();
	}
	else
	{
		gIsHolographic = false;
		mHolographicSpace = nullptr; 
		return;
	}

	DEBUG_LOG_LINE;
	mSpatialLocator.LocatabilityChanged([](SpatialLocator locator, auto object)
	{
		auto loca = locator.Locatability();
		kigsprintf("Locatibility: %d\n", int(loca));
	});
	mStationaryReferenceFrame = mSpatialLocator.CreateStationaryFrameOfReferenceAtCurrentLocation();
	DEBUG_LOG_LINE;
	auto family = AnalyticsInfo::VersionInfo().DeviceFamily();
	if (family != L"Windows.Holographic") gIsVR = true;
}

void App::SetWindow(CoreWindow const& window)
{
	DEBUG_LOG_LINE;
	mWindow = window;

	window.VisibilityChanged([this](CoreWindow const& window, VisibilityChangedEventArgs args)
	{
		DEBUG_LOG_LINE;
		mWindowVisible = args.Visible();
		if (KigsCore::Instance() && KigsCore::Instance()->GetCoreApplication())
			KigsCore::Instance()->GetCoreApplication()->EmitSignal("UWP_VisibilityChangedEvent", (bool)mWindowVisible);
	});
	
	window.Closed([this](CoreWindow const& window, CoreWindowEventArgs args)
	{
		DEBUG_LOG_LINE;
		mWindowClosed = true;
	});

	HolographicSpace::IsAvailableChanged([this](winrt::Windows::Foundation::IInspectable a, winrt::Windows::Foundation::IInspectable b)
	{
		OnHolographicDisplayIsAvailableChanged(a, b);
	});


	auto family = AnalyticsInfo::VersionInfo().DeviceFamily();
	
	if(family != L"Windows.Desktop")
		mHolographicSpace = HolographicSpace::CreateForCoreWindow(mWindow);

	OnHolographicDisplayIsAvailableChanged(nullptr, nullptr); 
	if (mHolographicSpace)
	{
		mHolographicSpace.CameraAdded([this](HolographicSpace space, const HolographicSpaceCameraAddedEventArgs& args)
		{
			kigsprintf("CAMERA ADDED\n");
			mCameras.insert(args.Camera());
			args.GetDeferral().Complete();
		});

		mHolographicSpace.CameraRemoved([this](HolographicSpace space, const HolographicSpaceCameraRemovedEventArgs& args)
		{
			kigsprintf("CAMERA REMOVED\n");
			mCameras.erase(args.Camera());
		});
	}
}

void App::Load(winrt::hstring const& entryPoint)
{
	DEBUG_LOG_LINE;
}

void App::Run()
{
	DEBUG_LOG_LINE;
	print_context("Run");

	setlocale(LC_NUMERIC, "C");
	KigsCore::Init();
	DEBUG_LOG_LINE;
	CoreWindow window = CoreWindow::GetForCurrentThread();
	CoreDispatcher dispatcher = window.Dispatcher();

	DECLARE_CLASS_INFO_WITHOUT_FACTORY(KIGS_APPLICATION_CLASS, ApplicationName(KIGS_APPLICATION_CLASS));
	SP<CoreBaseApplication> app = KIGS_APPLICATION_CLASS::CreateInstance(ApplicationName(KIGS_APPLICATION_CLASS));
	DEBUG_LOG_LINE;
#ifdef INIT_DEFAULT_MODULES
#ifdef BASE_DATA_PATH
	app->InitApp(BASE_DATA_PATH, true);
#else
	app->InitApp(0, true);
#endif //BASE_DATA_PATH

#else
	app->InitApp(0, false);
#endif //INIT_DEFAULT_MODULES

	DEBUG_LOG_LINE;
	while (!mWindowClosed && (!app->NeedExit()))
	{
		if (mWindowVisible)
		{
			CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
			app->UpdateApp();
		}
		else
		{
			CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
	DEBUG_LOG_LINE;
	app->CloseApp();
	app.reset();
	KigsCore::Close();
	DEBUG_LOG_LINE;
}

void App::Uninitialize()
{
	mWindowClosed = true;
	DEBUG_LOG_LINE;
}

void App::Swap()
{
	if (!mWindow) return;
}

std::optional<mat3x4> App::CreateNewFrameOfReference()
{
	SpatialLocator locator = SpatialLocator::GetDefault();
	if (!locator) return {};
	auto new_frame_of_reference = locator.CreateStationaryFrameOfReferenceAtCurrentLocation();
	if (!new_frame_of_reference) return {};

	auto transform_result = mStationaryReferenceFrame.CoordinateSystem().TryGetTransformTo(new_frame_of_reference.CoordinateSystem());
	if (!transform_result) return {};
	
	auto transform = transform_result.Value();
	auto mat = mat4(
		transform.m11, transform.m21, transform.m31, transform.m41,
		transform.m12, transform.m22, transform.m32, transform.m42,
		transform.m13, transform.m23, transform.m33, transform.m43,
		transform.m14, transform.m24, transform.m34, transform.m44);

	mat3x4 m{ mat.XAxis.xyz, mat.YAxis.xyz, mat.ZAxis.xyz, mat.Pos.xyz };
	
	auto val = winrt::get_abi(new_frame_of_reference);
	//auto result = SetFrameOfReference(static_cast<::IInspectable*>(val));
	//if (result)
	{
		mStationaryReferenceFrame = new_frame_of_reference;
		return m;
	}
	return {};
}

