#include "KigsApplication.h"
#include "BaseAppDX11.h"
#include "Core.h"
#include "CoreBaseApplication.h"
#include "FilePathManager.h"
#include "NotificationCenter.h"

#include <thread>
#include <mutex>
#include <future>

//#include <windows.foundation.h>
#include "winrt/Windows.System.Profile.h"
#include "winrt/Windows.ApplicationModel.Activation.h"
#include "winrt/Windows.ApplicationModel.Preview.Holographic.h"
#include "winrt/Windows.Storage.h"
#include "winrt/Windows.UI.ViewManagement.h"

#include "winrt/Windows.Graphics.DirectX.Direct3D11.h"

#include <sstream>

//#include <Inspectable.h>

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

App* App::sApp=nullptr;
bool gIsHolographic = true;
bool gIsVR = false;

std::mutex gPickedFileMutex;
SmartPointer<::FileHandle> gPickedFile;

using namespace winrt::Windows::System::Profile;

#define ApplicationName(a) Stringify(a)
#define Stringify(a) #a

#include "utf8.h"

void wupmain()
{
	winrt::init_apartment();
	CoreApplication::Run(App());
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
	StorageFileFileAccess::setMainThreadID();
	using namespace winrt::Windows::ApplicationModel::Activation;
	applicationView.Activated([this](CoreApplicationView const& view, IActivatedEventArgs args)
	{
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
						auto name = file.Name();
						usString str = (u16*)name.data();
						gPickedFile = FilePathManager::CreateFileHandle(str.ToString()); //@NOTE KigsCore is not created at this point
						gPickedFile->myVirtualFileAccess = new StorageFileFileAccess{ file };
						gPickedFile->myUseVirtualFileAccess = true;

						if (KigsCore::Instance())
						{
							KigsCore::GetNotificationCenter()->postNotificationName("WUPFileActivation");
						}

					}
				}
			}
		}
		// Run() won't start until the CoreWindow is activated.
		CoreWindow::GetForCurrentThread().Activate();
	});

	
}

void App::OnHolographicDisplayIsAvailableChanged(winrt::Windows::Foundation::IInspectable, winrt::Windows::Foundation::IInspectable)
{
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


	mSpatialLocator.LocatabilityChanged([](SpatialLocator locator, auto object)
	{
		auto loca = locator.Locatability();
		kigsprintf("Locatibility: %d\n", int(loca));
	});
	mStationaryReferenceFrame = mSpatialLocator.CreateStationaryFrameOfReferenceAtCurrentLocation();
	
	auto family = AnalyticsInfo::VersionInfo().DeviceFamily();
	if (family != L"Windows.Holographic") gIsVR = true;
}

void App::SetWindow(CoreWindow const& window)
{
	mWindow = window;

	window.VisibilityChanged([this](CoreWindow const& window, VisibilityChangedEventArgs args)
	{
		mWindowVisible = args.Visible();
		if (mWindowVisible)
		{
			auto notif = KigsCore::Instance() ? KigsCore::GetNotificationCenter() : nullptr; 
			if(notif) notif->postNotificationName("WUPWindowVisible");
		}
	});

	window.Closed([this](CoreWindow const& window, CoreWindowEventArgs args)
	{
		mWindowClosed = true;
	});

	HolographicSpace::IsAvailableChanged([this](winrt::Windows::Foundation::IInspectable a, winrt::Windows::Foundation::IInspectable b)
	{
		OnHolographicDisplayIsAvailableChanged(a, b);
	});

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
}

void App::Run()
{
	setlocale(LC_NUMERIC, "C");
	//! First thing to do

	KigsCore::Init(false);

#if defined(WUP) && 0
	Win32OpenFilePicker();
#endif
	CoreWindow window = CoreWindow::GetForCurrentThread();
	CoreDispatcher dispatcher = window.Dispatcher();


	// no need to register app to factory
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(KIGS_APPLICATION_CLASS, ApplicationName(KIGS_APPLICATION_CLASS));
	CoreBaseApplication*	myApp = (CoreBaseApplication*)KIGS_APPLICATION_CLASS::CreateInstance(ApplicationName(KIGS_APPLICATION_CLASS));

	/*auto itr = args->begin();
	auto end = args->end();
	for(;itr!=end;++itr)
	{
	myApp->PushArg((*itr)->Data);
	}*/


#ifdef INIT_DEFAULT_MODULES
#ifdef BASE_DATA_PATH
	//! then init
	myApp->InitApp(BASE_DATA_PATH, true);
#else
	//! then init
	myApp->InitApp(0, true);
#endif //BASE_DATA_PATH

#else
	//! then init
	myApp->InitApp(0, false);

#endif //INIT_DEFAULT_MODULES
	
	//window.Activate();

	//dispatcher.ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
	while (!mWindowClosed && (!myApp->NeedExit()))
	{
		if (mWindowVisible)
		{
			CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
			myApp->UpdateApp();
		}
		else
		{
			CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
	//CleanupEGL();


	//! close
	myApp->CloseApp();

	//! delete
	delete myApp;
	KigsCore::SetCoreApplication(nullptr);

#ifdef _DEBUG
	CoreModifiable::debugPrintfFullTree();
#endif

	//! last thing to do
	KigsCore::Close();
}

void App::Uninitialize()
{
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
/*
template<typename TResult>
bool WaitForAsyncOperation(::Windows::Foundation::IAsyncOperation<TResult>^ op)
{
	for (;;)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
		auto s = op->Status;
		if (s == Windows::Foundation::AsyncStatus::Completed)
			break;

		else if (op->Status != Windows::Foundation::AsyncStatus::Started)
		{
			return false;
		}
	}
	return true;
}
*/