#include "Window.h"
#include "OpenGLTexture.h"
#include "RendererOpenGL.h"
#include "FilePathManager.h"
#include "GLSLShader.h"
#include "TextureFileManager.h"

// WGL_ARB_extensions_string
PFNWGLGETEXTENSIONSSTRINGARBPROC OpenGLPlatformRenderingScreen::wglGetExtensionsStringARB = 0;

// WGL_ARB_pixel_format
PFNWGLCHOOSEPIXELFORMATARBPROC OpenGLPlatformRenderingScreen::wglChoosePixelFormatARB = 0;

// WGL_ARB_render_texture
PFNWGLBINDTEXIMAGEARBPROC     OpenGLPlatformRenderingScreen::wglBindTexImageARB = 0;
PFNWGLRELEASETEXIMAGEARBPROC  OpenGLPlatformRenderingScreen::wglReleaseTexImageARB = 0;
PFNWGLSETPBUFFERATTRIBARBPROC OpenGLPlatformRenderingScreen::wglSetPbufferAttribARB = 0;


IMPLEMENT_CLASS_INFO(OpenGLPlatformRenderingScreen)

OpenGLPlatformRenderingScreen::OpenGLPlatformRenderingScreen(const kstl::string& name, CLASS_NAME_TREE_ARG) : RenderingScreen(name, PASS_CLASS_NAME_TREE_ARG)
{
	myhDC = 0;
	myhRC = 0;
	myhWnd = 0;
}

OpenGLPlatformRenderingScreen::~OpenGLPlatformRenderingScreen()
{
	if (myhRC != 0)
		wglDeleteContext(myhRC);

	if (myhDC != 0)
		ReleaseDC(myhWnd, myhDC);
}

void    OpenGLPlatformRenderingScreen::InitModifiable()
{
	if (_isInit)
		return;

	RenderingScreen::InitModifiable();

	ModuleSpecificRenderer* renderer = RendererOpenGL::theGlobalRenderer; // ((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	OpenGLRenderingScreen* firstScreen = (OpenGLRenderingScreen*)renderer->getFirstRenderingScreen();


	RECT rect;
	if (!myhWnd)
	{
		if (MyParentWindow && MyParentWindow->IsInit())
			myhWnd = (HWND)MyParentWindow->GetHandle();
		/*else
			myhWnd = firstScreen->myhWnd;*/
	}

	if (myhWnd)
	{
		myhDC = ::GetDC(myhWnd);
		if (!SetupPixelFormat(myhDC))
			PostQuitMessage(0);

		// create a new context, as pixel format can be different
		int attriblist[] = { WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 2,
			WGL_CONTEXT_PROFILE_MASK_ARB ,  WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			0, 0 };

		if (firstScreen == this)
		{

			HGLRC temporaryContext = wglCreateContext(myhDC);
			wglMakeCurrent(myhDC, temporaryContext);

			GLenum err = glewInit();
			myhRC = wglCreateContextAttribsARB(myhDC, 0, attriblist); CHECK_GLERROR;

			wglMakeCurrent(myhDC, 0);
			wglDeleteContext(temporaryContext);
		}
		else
		{
			myhRC = wglCreateContextAttribsARB(myhDC, firstScreen->myhRC, attriblist); CHECK_GLERROR;
		}
		wglMakeCurrent(myhDC, myhRC);

		GetClientRect(myhWnd, &rect);
#ifdef FRAPS_CAPTURE_MODE
		InitializeGL(rect.right, rect.bottom - 96);
#else
		InitializeGL(rect.right, rect.bottom);
#endif
	}
	else
	{
		myhRC = firstScreen->myhRC;
		myhDC = firstScreen->myhDC;
		wglMakeCurrent(myhDC, myhRC);
	}

	typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");

	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT((bool)myWaitVSync);

}


bool OpenGLPlatformRenderingScreen::SetupPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd, *ppfd;
	int pixelformat;

	ppfd = &pfd;
	memset(ppfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
	ppfd->nVersion = 1;
	ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SWAP_EXCHANGE | PFD_GENERIC_ACCELERATED;
	if (myNeedDoubleBuffer)
	{
		ppfd->dwFlags |= PFD_DOUBLEBUFFER;
	}

	ppfd->dwLayerMask = PFD_MAIN_PLANE;

	ppfd->iPixelType = PFD_TYPE_RGBA;

	ppfd->cColorBits = mBitsPerPixel;
	ppfd->cDepthBits = mBitsPerZ;

	ppfd->cAccumBits = 0;
	ppfd->cStencilBits = mBitsForStencil;

	pixelformat = ChoosePixelFormat(hdc, ppfd);

	if ((pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0)
	{
		return false;
	}

	if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE)
	{
		return false;
	}

	return true;
}

void     OpenGLPlatformRenderingScreen::setCurrentContext()
{
	HDC current_dc = wglGetCurrentDC();
	if (current_dc != myhDC)
		wglMakeCurrent(myhDC, myhRC);

}

void OpenGLPlatformRenderingScreen::Update(const Timer&  timer, void* addParam)
{
	RenderingScreen::Update(timer, addParam);
	// move it in endframe ?
	SwapBuffers(myhDC);
}

void	OpenGLPlatformRenderingScreen::UninitModifiable()
{
	if (myhRC != 0)
		wglDeleteContext(myhRC);

	if (myhDC != 0)
		ReleaseDC(myhWnd, myhDC);
}


void	OpenGLPlatformRenderingScreen::SetWindowByHandle(void *PtrToHandle)
{
	myhWnd = (HWND)PtrToHandle;
}

