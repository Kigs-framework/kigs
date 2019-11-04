#include "KigsApplication.h"
#include "BaseApp.h"
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

/*
using LogFunc = void(const std::string &);
__declspec(dllimport) LogFunc *gLogFunc;

void KigsLog(const std::string& txt)
{
	return;
	static std::mutex gMutex;
	std::lock_guard<std::mutex> lk{ gMutex };
	static bool first = true;
	auto local_folder = ApplicationData::Current().LocalFolder().Path();
	std::string utf8_path;
	utf8::utf16to8(local_folder.begin(), local_folder.end(), std::back_inserter(utf8_path));
	utf8_path += "/kigs_log.txt";

	std::stringstream ss;
	ss << "[" << std::this_thread::get_id() << "] " << txt << "\n";
	auto hdl = fopen(utf8_path.c_str(), first ? "wb" : "ab");
	auto final_txt = ss.str();
	fwrite(final_txt.c_str(), final_txt.size(), 1, hdl);
	fflush(hdl);
	fclose(hdl);
	first = false;
}
*/

void wupmain()
{
	CoreApplication::Run(App());
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
						gPickedFile->myVirtualFileAccess = new StorageFileFileAccess{ to_cx<::Windows::Storage::StorageFile>(file) };
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


void App::SetWindow(CoreWindow const& window)
{
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

	auto family = AnalyticsInfo::VersionInfo().DeviceFamily();
	//gIsHolographic = false;
	if (gIsHolographic)
	{
		//while (!mHolographicSpace)
		{
			gIsHolographic = false;
			try
			{
				SpatialLocator locator = SpatialLocator::GetDefault();
				if (locator)
				{
					mHolographicSpace = HolographicSpace::CreateForCoreWindow(window);
					if(mHolographicSpace)
					{
						locator.LocatabilityChanged([](SpatialLocator locator, auto object)
						{
							auto loca = locator.Locatability();
							kigsprintf("Locatibility: %d\n", int(loca));
						});
						mStationaryReferenceFrame = locator.CreateStationaryFrameOfReferenceAtCurrentLocation();
						gIsHolographic = true;
						auto family = AnalyticsInfo::VersionInfo().DeviceFamily();
						if (family != L"Windows.Holographic") gIsVR = true;
					}
				}
				else
					mHolographicSpace = nullptr;
			}
			catch (winrt::hresult_error const&)
			{
				mHolographicSpace = nullptr;
			}
			catch (...)
			{
				mHolographicSpace = nullptr;
			}
			
		}
	}


	if(mHolographicSpace)
		InitializeEGL(mHolographicSpace);
	else
		InitializeEGL(window);

	mWindow = window;
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
	
	float factor;
	int tries = 240;
	if (gIsHolographic) tries *= 10;
	do
	{ 
		AngleHolographicGetCurrentSwapChainDimensions(&panelWidth, &panelHeight, &factor);
		std::this_thread::sleep_for(std::chrono::milliseconds(16)); 
		dispatcher.ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
		--tries; 
	} while (panelWidth <= 0 && tries > 0);

	auto family = AnalyticsInfo::VersionInfo().DeviceFamily();
	//gIsVR = gIsVR && family != L"Windows.Holographic";

	EGLint w=-1, h=-1;
	eglQuerySurface(mEglDisplay, mEglSurface, EGL_WIDTH, &w); 
	eglQuerySurface(mEglDisplay, mEglSurface, EGL_HEIGHT, &h);

	if (w != -1)
	{
		panelWidth = w;
		panelHeight = h;
	}
	else if(panelWidth <= 0)
	{
		panelWidth = 1268;
		panelHeight = 720;
	}
	
	//kigsprintf("%d %d\n", panelWidth, panelHeight);


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
			dispatcher.ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
			myApp->UpdateApp();
		}
		else
		{
			dispatcher.ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
	CleanupEGL();


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
	// The call to eglSwapBuffers might not be successful (e.g. due to Device Lost)
	// If the call fails, then we must reinitialize EGL and the GL resources.
	if (eglSwapBuffers(mEglDisplay, mEglSurface) != GL_TRUE)
	{
		CleanupEGL();
		if (KigsCore::GetCoreApplication()->IsHolographic())
		{
			InitializeEGL(mHolographicSpace);
		}
		else
		{
			InitializeEGL(CoreWindow::GetForCurrentThread());
		}
	}
}

void App::InitializeEGL(CoreWindow const& window)
{
	App::InitializeEGLInner(window);
}

void App::InitializeEGL(HolographicSpace const& holographicSpace)
{
	App::InitializeEGLInner(holographicSpace);
}

void App::InitializeEGLInner(IInspectable const& windowBasis)
{
	const EGLint configAttributes[] =
	{
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 16,
		EGL_STENCIL_SIZE, 8,
		EGL_NONE
	};

	const EGLint contextAttributes[] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	const EGLint surfaceAttributes[] =
	{
		// EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER is part of the same optimization as EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER (see above).
		// If you have compilation issues with it then please update your Visual Studio templates.
		EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER, EGL_TRUE,
		EGL_NONE
	};

	const EGLint defaultDisplayAttributes[] =
	{
		// These are the default display attributes, used to request ANGLE's D3D11 renderer.
		// eglInitialize will only succeed with these attributes if the hardware supports D3D11 Feature Level 10_0+.
		EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,

		// EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER is an optimization that can have large performance benefits on mobile devices.
		// Its syntax is subject to change, though. Please update your Visual Studio templates if you experience compilation issues with it.
		EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,

		// EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE is an option that enables ANGLE to automatically call 
		// the IDXGIDevice3::Trim method on behalf of the application when it gets suspended. 
		// Calling IDXGIDevice3::Trim when an application is suspended is a Windows Store application certification requirement.
		EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
		EGL_NONE,
	};

	const EGLint fl9_3DisplayAttributes[] =
	{
		// These can be used to request ANGLE's D3D11 renderer, with D3D11 Feature Level 9_3.
		// These attributes are used if the call to eglInitialize fails with the default display attributes.
		EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
		EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE, 9,
		EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE, 3,
		EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
		EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
		EGL_NONE,
	};

	const EGLint warpDisplayAttributes[] =
	{
		// These attributes can be used to request D3D11 WARP.
		// They are used if eglInitialize fails with both the default display attributes and the 9_3 display attributes.
		EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
		EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE,
		EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
		EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
		EGL_NONE,
	};

	EGLConfig config = NULL;

	// eglGetPlatformDisplayEXT is an alternative to eglGetDisplay. It allows us to pass in display attributes, used to configure D3D11.
	PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
	if (!eglGetPlatformDisplayEXT)
	{
		throw winrt::hresult_error(E_FAIL, L"Failed to get function eglGetPlatformDisplayEXT");
	}

	//
	// To initialize the display, we make three sets of calls to eglGetPlatformDisplayEXT and eglInitialize, with varying 
	// parameters passed to eglGetPlatformDisplayEXT:
	// 1) The first calls uses "defaultDisplayAttributes" as a parameter. This corresponds to D3D11 Feature Level 10_0+.
	// 2) If eglInitialize fails for step 1 (e.g. because 10_0+ isn't supported by the default GPU), then we try again 
	//    using "fl9_3DisplayAttributes". This corresponds to D3D11 Feature Level 9_3.
	// 3) If eglInitialize fails for step 2 (e.g. because 9_3+ isn't supported by the default GPU), then we try again 
	//    using "warpDisplayAttributes".  This corresponds to D3D11 Feature Level 11_0 on WARP, a D3D11 software rasterizer.
	//

	// This tries to initialize EGL to D3D11 Feature Level 10_0+. See above comment for details.
	mEglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, defaultDisplayAttributes);
	if (mEglDisplay == EGL_NO_DISPLAY)
	{
		throw winrt::hresult_error(E_FAIL, L"Failed to get EGL display");
	}

	if (eglInitialize(mEglDisplay, NULL, NULL) == EGL_FALSE)
	{
		// This tries to initialize EGL to D3D11 Feature Level 9_3, if 10_0+ is unavailable (e.g. on some mobile devices).
		mEglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, fl9_3DisplayAttributes);
		if (mEglDisplay == EGL_NO_DISPLAY)
		{
			throw winrt::hresult_error(E_FAIL, L"Failed to get EGL display");
		}

		if (eglInitialize(mEglDisplay, NULL, NULL) == EGL_FALSE)
		{
			// This initializes EGL to D3D11 Feature Level 11_0 on WARP, if 9_3+ is unavailable on the default GPU.
			mEglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, warpDisplayAttributes);
			if (mEglDisplay == EGL_NO_DISPLAY)
			{
				throw winrt::hresult_error(E_FAIL, L"Failed to get EGL display");
			}

			if (eglInitialize(mEglDisplay, NULL, NULL) == EGL_FALSE)
			{
				// If all of the calls to eglInitialize returned EGL_FALSE then an error has occurred.
				throw winrt::hresult_error(E_FAIL, L"Failed to initialize EGL");
			}
		}
	}

	EGLint numConfigs = 0;
	if ((eglChooseConfig(mEglDisplay, configAttributes, &config, 1, &numConfigs) == EGL_FALSE) || (numConfigs == 0))
	{
		throw winrt::hresult_error(E_FAIL, L"Failed to choose first EGLConfig");
	}
	
	// Create a PropertySet and initialize with the EGLNativeWindowType.
	::Windows::Foundation::Collections::PropertySet^ surfaceCreationProperties = ref new ::Windows::Foundation::Collections::PropertySet();
	surfaceCreationProperties->Insert(ref new Platform::String(EGLNativeWindowTypeProperty), reinterpret_cast<Platform::Object^>(winrt::get_abi(windowBasis)));
	
	//winrt::Windows::Foundation::Collections::PropertySet surfaceCreationProperties;
	//surfaceCreationProperties.Insert(EGLNativeWindowTypeProperty, windowBasis);

	if (GetStationaryReferenceFrame() != nullptr)
	{
		surfaceCreationProperties->Insert(ref new Platform::String(EGLBaseCoordinateSystemProperty), to_cx<::Windows::Perception::Spatial::SpatialStationaryFrameOfReference>(GetStationaryReferenceFrame()));
		//surfaceCreationProperties.Insert(EGLBaseCoordinateSystemProperty, GetStationaryReferenceFrame());
	}

	// You can configure the surface to render at a lower resolution and be scaled up to
	// the full window size. This scaling is often free on mobile hardware.
	//
	// One way to configure the SwapChainPanel is to specify precisely which resolution it should render at.
	//Size customRenderSurfaceSize = Size(320, 240);
	//surfaceCreationProperties->Insert(ref new String(EGLRenderSurfaceSizeProperty), PropertyValue::CreateSize(customRenderSurfaceSize));
	//
	// Another way is to tell the SwapChainPanel to render at a certain scale factor compared to its size.
	// e.g. if the SwapChainPanel is 1920x1280 then setting a factor of 0.5f will make the app render at 960x640
	float customResolutionScale = 1.0f;
	surfaceCreationProperties->Insert(ref new Platform::String(EGLRenderResolutionScaleProperty), ::Windows::Foundation::PropertyValue::CreateSingle(customResolutionScale));
	//surfaceCreationProperties.Insert(EGLRenderResolutionScaleProperty, winrt::box_value(customResolutionScale));

		
	mEglSurface = eglCreateWindowSurface(mEglDisplay, config, reinterpret_cast<::IInspectable*>(surfaceCreationProperties), surfaceAttributes); 
	if (mEglSurface == EGL_NO_SURFACE)
	{
		throw winrt::hresult_error(E_FAIL, L"Failed to create EGL fullscreen surface");
	}

	mEglContext = eglCreateContext(mEglDisplay, config, EGL_NO_CONTEXT, contextAttributes);
	if (mEglContext == EGL_NO_CONTEXT)
	{
		throw winrt::hresult_error(E_FAIL, L"Failed to create EGL context");
	}

	if (eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext) == EGL_FALSE)
	{
		throw winrt::hresult_error(E_FAIL, L"Failed to make fullscreen EGLSurface current");
	}
	eglSurfaceAttrib(mEglDisplay, mEglSurface, EGLEXT_WAIT_FOR_VBLANK_ANGLE, 0);
}

void App::CleanupEGL()
{
	if (mEglDisplay != EGL_NO_DISPLAY && mEglSurface != EGL_NO_SURFACE)
	{
		eglDestroySurface(mEglDisplay, mEglSurface);
		mEglSurface = EGL_NO_SURFACE;
	}

	if (mEglDisplay != EGL_NO_DISPLAY && mEglContext != EGL_NO_CONTEXT)
	{
		eglDestroyContext(mEglDisplay, mEglContext);
		mEglContext = EGL_NO_CONTEXT;
	}

	if (mEglDisplay != EGL_NO_DISPLAY)
	{
		eglTerminate(mEglDisplay);
		mEglDisplay = EGL_NO_DISPLAY;
	}
}


__declspec(dllimport) bool SetFrameOfReference(::IInspectable *value);

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
	auto result = SetFrameOfReference(static_cast<::IInspectable*>(val));
	if (result)
	{
		mStationaryReferenceFrame = new_frame_of_reference;
		return m;
	}
	return {};
}

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


#if 0
void App::ChangeOrigin(Windows::Perception::Spatial::SpatialStageFrameOfReference^ ref)
{
	eglExtSurfaceAttribPointerANGLE(
		mEglDisplay, mEglSurface,
		EGLEXT_HOLOGRAPHIC_SPATIAL_FRAME_OF_REFERENCE_ANGLE,
		reinterpret_cast<IInspectable*>(ref));
}
#endif
