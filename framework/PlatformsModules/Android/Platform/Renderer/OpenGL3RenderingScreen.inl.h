#include "GLSLShader.h"
#include "Window.h"
#include "RendererOpenGL.h"
#include "TextureFileManager.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>

extern android_app* 			mAndroidApp;

namespace Kigs
{
	namespace Draw
	{
		IMPLEMENT_CLASS_INFO(OpenGLPlatformRenderingScreen)

		OpenGLPlatformRenderingScreen::OpenGLPlatformRenderingScreen(const std::string& name, CLASS_NAME_TREE_ARG) : RenderingScreen(name, PASS_CLASS_NAME_TREE_ARG)
		{
			mDisplay= nullptr;
			mSurface= nullptr;
			mContext= nullptr;
		}

		OpenGLPlatformRenderingScreen::~OpenGLPlatformRenderingScreen()
		{

		}

		void OpenGLPlatformRenderingScreen::InitModifiable()
		{
			if (_isInit)
				return;
			RenderingScreen::InitModifiable();

			mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

			// TODO : use rendering screen preferences for attribs
			constexpr EGLint attribs[] = {
					EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
					EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
					EGL_BLUE_SIZE, 8,
					EGL_GREEN_SIZE, 8,
					EGL_RED_SIZE, 8,
					EGL_DEPTH_SIZE, 24,
					EGL_NONE
			};

			EGLConfig config;
			EGLint numConfigs;
			eglGetConfigs(mDisplay, NULL, 0, &numConfigs);
			eglChooseConfig(mDisplay, attribs, &config, 1, &numConfigs);
			EGLint format;
			eglGetConfigAttrib(mDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
			mSurface = eglCreateWindowSurface(mDisplay, config, mAndroidApp->window, nullptr);

			// Create a GLES 3 context
			EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
			mContext = eglCreateContext(mDisplay, config, nullptr, contextAttribs);

			eglMakeCurrent(mDisplay, mSurface, mSurface, mContext);

			InitializeGL(mSize[0], mSize[1]);

		}
		
		void	OpenGLPlatformRenderingScreen::SwapBuffer(TravState*)
		{
			eglSwapBuffers(mDisplay, mSurface);
		}
		void  OpenGLPlatformRenderingScreen::Update(const Timer&  timer, void* addParam) {
			
			if(mDisplay && mSurface)
			{
				EGLint width;
				eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &width);

				EGLint height;
				eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &height);

				if (width != mSize[0] || height != mSize[1]) {
					Resize(width, height);
				}
			}
			RenderingScreen::Update(timer,addParam);
		}
	}
}