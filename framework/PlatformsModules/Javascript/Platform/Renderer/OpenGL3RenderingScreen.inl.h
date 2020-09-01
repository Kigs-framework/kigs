#include "GLSLShader.h"
#include "Window.h"
#include "RendererOpenGL.h"
#include "TextureFileManager.h"

extern "C" void				KigsCreateContext(bool useWebGL);
#ifdef USE_EGL
OpenGLPlatformRenderingScreen::ESContext OpenGLPlatformRenderingScreen::esContext;
#endif

IMPLEMENT_CLASS_INFO(OpenGLPlatformRenderingScreen)

OpenGLPlatformRenderingScreen::OpenGLPlatformRenderingScreen(const kstl::string& name,CLASS_NAME_TREE_ARG) : RenderingScreen(name,PASS_CLASS_NAME_TREE_ARG)
{

	

	RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer);
	RenderingScreen* firstScreen = (RenderingScreen*)renderer->getFirstRenderingScreen();

	if (firstScreen == this)
	{
		KigsCreateContext(true);

#ifdef USE_EGL
		EGLint attribList[] =
		{
			EGL_RED_SIZE,       8,
			EGL_GREEN_SIZE,     8,
			EGL_BLUE_SIZE,      8,
			EGL_ALPHA_SIZE,     8,
			EGL_DEPTH_SIZE,     EGL_DONT_CARE,
			EGL_STENCIL_SIZE,   8,
			EGL_SAMPLE_BUFFERS,  0,
			EGL_NONE
		};

		
		EGLint numConfigs;
		EGLint majorVersion;
		EGLint minorVersion;
		EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2,EGL_NONE };

		esContext.eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		assert(esContext.eglDisplay != EGL_NO_DISPLAY);
		assert(eglGetError() == EGL_SUCCESS);

		EGLBoolean ret = eglInitialize(esContext.eglDisplay, &majorVersion, &minorVersion);
		assert(eglGetError() == EGL_SUCCESS);
		assert(ret == EGL_TRUE);
		assert(majorVersion * 10000 + minorVersion >= 10004);

		// Get configs
		ret = eglGetConfigs(esContext.eglDisplay, NULL, 0, &numConfigs);
		assert(eglGetError() == EGL_SUCCESS);
		assert(ret == EGL_TRUE);

		// Choose config
		ret = eglChooseConfig(esContext.eglDisplay, attribList, &esContext.eglConfig, 1, &numConfigs);
		assert(eglGetError() == EGL_SUCCESS);
		assert(ret == EGL_TRUE);

		// Create a surface
		EGLNativeWindowType dummyWindow;
		esContext.eglSurface = eglCreateWindowSurface(esContext.eglDisplay, esContext.eglConfig, dummyWindow, NULL);
		assert(eglGetError() == EGL_SUCCESS);
		assert(esContext.eglSurface != 0);

		// Create a GL context
		esContext.eglContext = eglCreateContext(esContext.eglDisplay, esContext.eglConfig, 0, contextAttribs);
		assert(eglGetError() == EGL_SUCCESS);
		assert(esContext.eglContext != 0);

		eglContext = esContext.eglContext;

		// Make the context current
		ret = eglMakeCurrent(esContext.eglDisplay, esContext.eglSurface, esContext.eglSurface, esContext.eglContext);
		assert(eglGetError() == EGL_SUCCESS);
		assert(ret == EGL_TRUE);
		assert(eglGetCurrentContext() == esContext.eglContext);
		assert(eglGetCurrentSurface(EGL_READ) == esContext.eglSurface);
		assert(eglGetCurrentSurface(EGL_DRAW) == esContext.eglSurface);
#endif
	}
}


OpenGLPlatformRenderingScreen::~OpenGLPlatformRenderingScreen()
{
}    

void    OpenGLPlatformRenderingScreen::InitModifiable()
{
	if (_isInit)
		return;

	RenderingScreen::InitModifiable();

	ModuleSpecificRenderer* renderer = ModuleRenderer::mTheGlobalRenderer; // ((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	OpenGLRenderingScreen* firstScreen = (OpenGLRenderingScreen*)renderer->getFirstRenderingScreen();

#ifdef USE_EGL
	/*

	EGLint attribList[] =
	{
		EGL_RED_SIZE,       8,
		EGL_GREEN_SIZE,     8,
		EGL_BLUE_SIZE,      8,
		EGL_ALPHA_SIZE,     8,
		EGL_DEPTH_SIZE,     EGL_DONT_CARE,
		EGL_STENCIL_SIZE,   8,
		EGL_SAMPLE_BUFFERS,  0,
		EGL_NONE
	};

	if (firstScreen == this)
	{
		EGLint numConfigs;
		EGLint majorVersion;
		EGLint minorVersion;
		EGLConfig config;
		EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2,EGL_NONE };

		esContext.eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		assert(esContext.eglDisplay != EGL_NO_DISPLAY);
		assert(eglGetError() == EGL_SUCCESS);

		EGLBoolean ret = eglInitialize(esContext.eglDisplay, &majorVersion, &minorVersion);
		assert(eglGetError() == EGL_SUCCESS);
		assert(ret == EGL_TRUE);
		assert(majorVersion * 10000 + minorVersion >= 10004);

		// Get configs
		ret = eglGetConfigs(esContext.eglDisplay, NULL, 0, &numConfigs);
		assert(eglGetError() == EGL_SUCCESS);
		assert(ret == EGL_TRUE);

		// Choose config
		ret = eglChooseConfig(esContext.eglDisplay, attribList, &config, 1, &numConfigs);
		assert(eglGetError() == EGL_SUCCESS);
		assert(ret == EGL_TRUE);

		// Create a surface
		EGLNativeWindowType dummyWindow;
		esContext.eglSurface = eglCreateWindowSurface(esContext.eglDisplay, config, dummyWindow, NULL);
		assert(eglGetError() == EGL_SUCCESS);
		assert(esContext.eglSurface != 0);

		// Create a GL context
		esContext.eglContext = eglCreateContext(esContext.eglDisplay, config, 0, contextAttribs);
		assert(eglGetError() == EGL_SUCCESS);
		assert(esContext.eglContext != 0);


		// Make the context current
		ret = eglMakeCurrent(esContext.eglDisplay, esContext.eglSurface, esContext.eglSurface, esContext.eglContext);
		assert(eglGetError() == EGL_SUCCESS);
		assert(ret == EGL_TRUE);
		assert(eglGetCurrentContext() == esContext.eglContext);
		assert(eglGetCurrentSurface(EGL_READ) == esContext.eglSurface);
		assert(eglGetCurrentSurface(EGL_DRAW) == esContext.eglSurface);
	}*/

	if (firstScreen != this)
	{
		EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2,EGL_NONE };
		esContext.eglContext = eglCreateContext(esContext.eglDisplay, esContext.eglConfig, 0, contextAttribs);
		assert(eglGetError() == EGL_SUCCESS);
		assert(esContext.eglContext != 0);

		eglContext = esContext.eglContext;
	}
#else

	if (mParentWindow && !mIsOffScreen)
	{
		std::string name = mParentWindow->getName();
		if (firstScreen == this)
		{
			name ="canvas";
		}
		EmscriptenWebGLContextAttributes attrs;
		emscripten_webgl_init_context_attributes(&attrs);
		attrs.depth = 1;
		attrs.stencil = 1;
		attrs.antialias = 0;

		EM_context = emscripten_webgl_create_context(name.c_str(), &attrs);
		if(EM_context<0)
		{
			printf("GL Context creation failed on canvas %s, create default context\n",name.c_str());
			emscripten_webgl_init_context_attributes(&attrs);
			EM_context = emscripten_webgl_create_context(name.c_str(), &attrs);
			if(EM_context<0)
			{
				printf("GL Context creation failed on canvas %s with code %d\n",name.c_str(),EM_context);
			}
		}
	}
	else
	{
		EM_context = firstScreen->EM_context;
	}

#endif

	setCurrentContext();
	InitializeGL(mSizeX, mSizeY);

}  

 
  
