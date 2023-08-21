#include "PrecompiledHeaders.h"
#include "DisplayDeviceCapsAndroid.h"
#include "Core.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

using namespace Kigs::Gui;

extern android_app* 			mAndroidApp;

IMPLEMENT_CLASS_INFO(DisplayDeviceCapsAndroid)

DisplayDeviceCapsAndroid::DisplayDeviceCapsAndroid(const std::string& name,CLASS_NAME_TREE_ARG) : DisplayDeviceCaps(name,PASS_CLASS_NAME_TREE_ARG)
{
	// Choose your render attributes
	constexpr EGLint attribs[] = {
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_NONE
	};

    int width = ANativeWindow_getWidth(mAndroidApp->window);
    int height = ANativeWindow_getHeight(mAndroidApp->window);

	// The default display is probably what you want on Android
	auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(display, nullptr, nullptr);

	// figure out how many configs there are
	EGLint numConfigs;
	eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);

	// get the list of configurations
	std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
	eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);

	mDisplayDeviceList.clear();

	DisplayDevice toAdd;

	// only one screen
	toAdd.mName="SCREEN";
	toAdd.mMain=true;

	for(uint32_t i=0;i<numConfigs;i++)
	{
		DisplayDeviceCapacity modeToAdd;

		EGLint attrib,r,g,b;

		const EGLConfig &config=supportedConfigs.get()[i];

		modeToAdd.mWidth = width;
		modeToAdd.mHeight = height;

		eglGetConfigAttrib(display,config,EGL_RED_SIZE,&r);
		eglGetConfigAttrib(display,config,EGL_GREEN_SIZE,&g);
		eglGetConfigAttrib(display,config,EGL_BLUE_SIZE,&b);

		modeToAdd.mBitPerPixel=r+g+b;
		// set first one to current ?
		modeToAdd.mIsCurrent=(i==0);

		toAdd.mCapacityList.push_back(modeToAdd);

	}

	mDisplayDeviceList[toAdd.mName]=toAdd;

}

//! destructor
DisplayDeviceCapsAndroid::~DisplayDeviceCapsAndroid()
{  
}