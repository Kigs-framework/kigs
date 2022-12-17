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

OpenGLPlatformRenderingScreen::OpenGLPlatformRenderingScreen(const std::string& name, CLASS_NAME_TREE_ARG) : RenderingScreen(name, PASS_CLASS_NAME_TREE_ARG)
{
	mHDC = 0;
	mHRC = 0;
	mHWnd = 0;
}

OpenGLPlatformRenderingScreen::~OpenGLPlatformRenderingScreen()
{
	if (mHRC != 0)
		wglDeleteContext(mHRC);

	if (mHDC != 0)
		ReleaseDC(mHWnd, mHDC);
}

void    OpenGLPlatformRenderingScreen::InitModifiable()
{
	if (_isInit)
		return;

	RenderingScreen::InitModifiable();

	ModuleSpecificRenderer* renderer = ModuleRenderer::mTheGlobalRenderer; // ((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	OpenGLRenderingScreen* firstScreen = (OpenGLRenderingScreen*)renderer->getFirstRenderingScreen();


	RECT rect;
	if (!mHWnd)
	{
		if (mParentWindow && mParentWindow->IsInit())
			mHWnd = (HWND)mParentWindow->GetHandle();
		/*else
			mHWnd = firstScreen->mHWnd;*/
	}

	if (mHWnd)
	{
		mHDC = ::GetDC(mHWnd);
		if (!SetupPixelFormat(mHDC))
			PostQuitMessage(0);

		// create a new context, as pixel format can be different
		int attriblist[] = { WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 2,
			WGL_CONTEXT_PROFILE_MASK_ARB ,  WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			0, 0 };

		if (firstScreen == this)
		{

			HGLRC temporaryContext = wglCreateContext(mHDC);
			wglMakeCurrent(mHDC, temporaryContext);

			GLenum err = glewInit();
			mHRC = wglCreateContextAttribsARB(mHDC, 0, attriblist); CHECK_GLERROR;

			wglMakeCurrent(mHDC, 0);
			wglDeleteContext(temporaryContext);
		}
		else
		{
			mHRC = wglCreateContextAttribsARB(mHDC, firstScreen->mHRC, attriblist); CHECK_GLERROR;
		}
		wglMakeCurrent(mHDC, mHRC);

		GetClientRect(mHWnd, &rect);
#ifdef FRAPS_CAPTURE_MODE
		InitializeGL(rect.right, rect.bottom - 96);
#else
		InitializeGL(rect.right, rect.bottom);
#endif
	}
	else
	{
		mHRC = firstScreen->mHRC;
		mHDC = firstScreen->mHDC;
		wglMakeCurrent(mHDC, mHRC);
	}

	typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");

	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT((bool)mVSync);

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
	if (mNeedDoubleBuffer)
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
	if (current_dc != mHDC)
		wglMakeCurrent(mHDC, mHRC);

}

void OpenGLPlatformRenderingScreen::Update(const Timer&  timer, void* addParam)
{
	RenderingScreen::Update(timer, addParam);
	// move it in endframe ?
	SwapBuffers(mHDC);
}

void	OpenGLPlatformRenderingScreen::UninitModifiable()
{
	if (mHRC != 0)
		wglDeleteContext(mHRC);

	if (mHDC != 0)
		ReleaseDC(mHWnd, mHDC);
}


void	OpenGLPlatformRenderingScreen::SetWindowByHandle(void *PtrToHandle)
{
	mHWnd = (HWND)PtrToHandle;
}

