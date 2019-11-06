
// WGL_ARB_extensions_string
PFNWGLGETEXTENSIONSSTRINGARBPROC OpenGLRenderingScreen::wglGetExtensionsStringARB = 0;

// WGL_ARB_pbuffer
PFNWGLCREATEPBUFFERARBPROC    OpenGLRenderingScreen::wglCreatePbufferARB = 0;
PFNWGLGETPBUFFERDCARBPROC     OpenGLRenderingScreen::wglGetPbufferDCARB = 0;
PFNWGLRELEASEPBUFFERDCARBPROC OpenGLRenderingScreen::wglReleasePbufferDCARB = 0;
PFNWGLDESTROYPBUFFERARBPROC   OpenGLRenderingScreen::wglDestroyPbufferARB = 0;
PFNWGLQUERYPBUFFERARBPROC     OpenGLRenderingScreen::wglQueryPbufferARB = 0;

// WGL_ARB_pixel_format
PFNWGLCHOOSEPIXELFORMATARBPROC OpenGLRenderingScreen::wglChoosePixelFormatARB = 0;

// WGL_ARB_render_texture
PFNWGLBINDTEXIMAGEARBPROC     OpenGLRenderingScreen::wglBindTexImageARB = 0;
PFNWGLRELEASETEXIMAGEARBPROC  OpenGLRenderingScreen::wglReleaseTexImageARB = 0;
PFNWGLSETPBUFFERATTRIBARBPROC OpenGLRenderingScreen::wglSetPbufferAttribARB = 0;

int OpenGLRenderingScreen::myActivatedCount = 0;

IMPLEMENT_CLASS_INFO(OpenGLRenderingScreen)

OpenGLRenderingScreen::OpenGLRenderingScreen(const kstl::string& name, CLASS_NAME_TREE_ARG) : RenderingScreen(name, PASS_CLASS_NAME_TREE_ARG)

{
	CONSTRUCT_METHOD(OpenGLRenderingScreen, Snapshot)
	myhDC = 0;
	myhRC = 0;
	myhWnd = 0;
	myBindedTexture = 0;
}

OpenGLRenderingScreen::~OpenGLRenderingScreen()
{
	if (!myIsOffScreen)
	{
		if (myhRC != 0)
			wglDeleteContext(myhRC);

		if (myhDC != 0)
			ReleaseDC(myhWnd, myhDC);
	}
	else
	{
		if (myhRC != 0)
			wglDestroyPbufferARB(myPBuffer);

		if (myhDC != 0)
			wglReleasePbufferDCARB(myPBuffer, myhDC);
	}
}

void    OpenGLRenderingScreen::InitModifiable()
{
	RenderingScreen::InitModifiable();

	ModuleSpecificRenderer* renderer = ModuleRenderer::theGlobalRenderer; // ((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();

	OpenGLRenderingScreen* firstScreen = (OpenGLRenderingScreen*)renderer->getFirstRenderingScreen();

	RECT rect;
	if (!myIsOffScreen)
	{
		if (!myhWnd)
		{
			if (!MyParentWindow->IsInit())
			{
				return;
			}


			myhWnd = (HWND)MyParentWindow->GetHandle();
		}

		myhDC = ::GetDC(myhWnd);
		if (!SetupPixelFormat(myhDC))
			PostQuitMessage(0);

		if (firstScreen == this)
		{
			myhRC = wglCreateContext(myhDC);
		}
		else
		{
			myhRC = firstScreen->myhRC;
		}
		wglMakeCurrent(myhDC, myhRC);

		GetClientRect(myhWnd, &rect);
		KIGS_ASSERT(glGetError() == GL_NO_ERROR);
#ifdef FRAPS_CAPTURE_MODE
		InitializeGL(rect.right, rect.bottom-96);
#else
		InitializeGL(rect.right, rect.bottom);
#endif
		KIGS_ASSERT(glGetError() == GL_NO_ERROR);
	}
	else // offscreen rendering screen
	{
		if (wglChoosePixelFormatARB)
		{
			int pf_attr[] =
			{
				WGL_SUPPORT_OPENGL_ARB, TRUE,       // P-buffer will be used with OpenGL
				WGL_DRAW_TO_PBUFFER_ARB, TRUE,      // Enable render to p-buffer
				WGL_BIND_TO_TEXTURE_RGBA_ARB, TRUE, // P-buffer will be used as a texture
				WGL_RED_BITS_ARB, 8,                // At least 8 bits for RED channel
				WGL_GREEN_BITS_ARB, 8,              // At least 8 bits for GREEN channel
				WGL_BLUE_BITS_ARB, 8,               // At least 8 bits for BLUE channel
				WGL_ALPHA_BITS_ARB, 8,              // At least 8 bits for ALPHA channel
				WGL_DEPTH_BITS_ARB, 16,             // At least 16 bits for depth buffer
				WGL_DOUBLE_BUFFER_ARB, FALSE,       // We don't require double buffering
				0                                   // Zero terminates the list
			};

			unsigned int count = 0;
			int pixelFormat;
			wglChoosePixelFormatARB(firstScreen->myhDC, (const int*)pf_attr, NULL, 1, &pixelFormat, &count);

			int pb_attr[] =
			{
				WGL_TEXTURE_FORMAT_ARB, WGL_TEXTURE_RGBA_ARB, // Our p-buffer will have a texture format of RGBA
				WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_2D_ARB,   // Of texture target will be GL_TEXTURE_2D
				0                                             // Zero terminates the list
			};

			//
			// Create the p-buffer...
			//

#ifdef FRAPS_CAPTURE_MODE
			myPBuffer = wglCreatePbufferARB( firstScreen->myhDC, pixelFormat, mySizeX, mySizeY+96, pb_attr );
#else
			myPBuffer = wglCreatePbufferARB(firstScreen->myhDC, pixelFormat, mySizeX, mySizeY, pb_attr);
#endif
			myhDC = wglGetPbufferDCARB(myPBuffer);

			myhRC = firstScreen->myhRC;
		}
	}


	if (firstScreen->myhRC != myhRC)
	{
		wglShareLists(firstScreen->myhRC, myhRC);
	}
	else
	{

		GLenum err = glewInit();

		if (wglCreatePbufferARB == 0)
		{
			wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
			wglCreatePbufferARB = (PFNWGLCREATEPBUFFERARBPROC)wglGetProcAddress("wglCreatePbufferARB");
			wglGetPbufferDCARB = (PFNWGLGETPBUFFERDCARBPROC)wglGetProcAddress("wglGetPbufferDCARB");
			wglReleasePbufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)wglGetProcAddress("wglReleasePbufferDCARB");
			wglDestroyPbufferARB = (PFNWGLDESTROYPBUFFERARBPROC)wglGetProcAddress("wglDestroyPbufferARB");
			wglQueryPbufferARB = (PFNWGLQUERYPBUFFERARBPROC)wglGetProcAddress("wglQueryPbufferARB");
			wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
			wglBindTexImageARB = (PFNWGLBINDTEXIMAGEARBPROC)wglGetProcAddress("wglBindTexImageARB");
			wglReleaseTexImageARB = (PFNWGLRELEASETEXIMAGEARBPROC)wglGetProcAddress("wglReleaseTexImageARB");
			wglSetPbufferAttribARB = (PFNWGLSETPBUFFERATTRIBARBPROC)wglGetProcAddress("wglSetPbufferAttribARB");

		}
	}

	typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");

	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT((bool)myWaitVSync);


	myIsInit = true;

}


void OpenGLRenderingScreen::InitializeGL(GLsizei width, GLsizei height)
{
	RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();

	mySizeX = (unsigned int)width;
	mySizeY = (unsigned int)height;
	GLfloat     /*maxObjectSize,*/ aspect;
	//	GLdouble    near_plane, far_plane;

	//glC_learColor(0.8f,0.8f,1.0f,0.0f);
	renderer->SetClearColorValue(0.8f, 0.8f, 1.0f, 0.0f);
	//glClearDepth( 1.0f );
	renderer->SetDepthValueMode(1.0);
	//glEnable(GL_D_EPTH_TEST);
	renderer->SetDepthTestMode(true);
	aspect = (GLfloat)width / height;
	renderer->Perspective(MATRIX_MODE_PROJECTION, 45.0f, aspect, 0.1f, 20.0f);

	renderer->LoadIdentity(MATRIX_MODE_MODEL);
	renderer->LoadIdentity(MATRIX_MODE_VIEW);
	// default "camera"
	gluLookAt(5.0f, 5.0f, 5.0f, -5.0f, -5.0f, -5.0f, 0.0f, 0.0f, 1.0f);
}

void	OpenGLRenderingScreen::Resize(kfloat sizeX, kfloat sizeY)
{
	// before screen is init, only update sizeX and sizeY
	if (!IsInit())
	{
		mySizeX = (unsigned int)sizeX;
		mySizeY = (unsigned int)sizeY;
	}
	else
	{
		//When resizing an offscreen surface, we keep the big surface but we let the camera clip the scene
		InitializeGL((int)sizeX, (int)sizeY);
	}

	RecomputeDesignCoef();
}

void OpenGLRenderingScreen::FetchPixels(int x, int y, int width, int height, void *pRGBAPixels)
{
	glFlush();
	glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pRGBAPixels);
}

void OpenGLRenderingScreen::FetchDepth(int x, int y, int width, int height, float *pDepthPixels)
{
	glFlush();
	y = mySizeY - 1 - y;
	glReadPixels(x, y, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, pDepthPixels);
}

void OpenGLRenderingScreen::FetchDepth(int x, int y, int width, int height, unsigned int *pDepthPixels)
{
	glFlush();
	y = mySizeY - 1 - y;
	glReadPixels(x, y, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, pDepthPixels);
}


bool OpenGLRenderingScreen::SetupPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd, *ppfd;
	int pixelformat;

	ppfd = &pfd;

	ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
	ppfd->nVersion = 1;
	if (myIsOffScreen)
		ppfd->dwFlags = PFD_SUPPORT_OPENGL;
	else
	{
		ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SWAP_EXCHANGE | PFD_GENERIC_ACCELERATED;
		if (myNeedDoubleBuffer)
		{
			ppfd->dwFlags |= PFD_DOUBLEBUFFER;
		}
	}

	ppfd->dwLayerMask = PFD_MAIN_PLANE;

	ppfd->iPixelType = PFD_TYPE_RGBA;

	ppfd->cColorBits = myBitsPerPixel;
	ppfd->cDepthBits = myBitsPerZ;

	ppfd->cAccumBits = 0;
	ppfd->cStencilBits = 0;

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

void OpenGLRenderingScreen::Update(const Timer& timer, void* addParam)
{
	RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	HDC current_dc = wglGetCurrentDC();
	if (current_dc != myhDC)
		wglMakeCurrent(myhDC, myhRC);


	//glC_learColor(myBackDropColor[0], myBackDropColor[1], myBackDropColor[2], 0.0);
	renderer->SetClearColorValue(myBackDropColor[0], myBackDropColor[1], myBackDropColor[2], 0.0);
	renderer->FlushState();

	RenderingScreen::Update(timer, addParam);

	// brightness (fade) effect here
	ManageFade();

	//glFlush();
	//glFinish();

	SwapBuffers(myhDC);
	
}

void	OpenGLRenderingScreen::UninitModifiable()
{
	if (!myIsOffScreen)
	{
		if (myhRC != 0)
			wglDeleteContext(myhRC);

		if (myhDC != 0)
			ReleaseDC(myhWnd, myhDC);
	}
	else
	{
		if (myhRC != 0)
			wglDestroyPbufferARB(myPBuffer);

		if (myhDC != 0)
			wglReleasePbufferDCARB(myPBuffer, myhDC);
	}
}

bool    OpenGLRenderingScreen::SetActive()
{
	RenderingScreen::SetActive();
	HDC current_dc = wglGetCurrentDC();
	if (current_dc != myhDC)
		wglMakeCurrent(myhDC, myhRC);

	KIGS_ASSERT(myActivatedCount == 0);
	myActivatedCount++;

	RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
		
	if (myIsOffScreen)
	{
		if (myBindedTexture)
		{	//glBindTexture( GL_TEXTURE_2D, myBindedTexture->GetGLID() );
			renderer->BindTexture(RENDERER_TEXTURE_2D, myBindedTexture->GetGLID());
			wglReleaseTexImageARB(myPBuffer, WGL_FRONT_LEFT_ARB);
			KIGS_ASSERT(glGetError() == GL_NO_ERROR);
		}

		int flag = 0;
		//wglQueryPbufferARB( myPBuffer, WGL_PBUFFER_LOST_ARB, &flag );
	}

	renderer->SetCullMode((RendererCullMode)(int)RENDERER_CULL_NONE);

	return true;
}

void    OpenGLRenderingScreen::Release()
{
	if (myBindedTexture && myIsOffScreen)
	{
		RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::theGlobalRenderer);  // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
		//glBindTexture( GL_TEXTURE_2D, myBindedTexture->GetGLID() );
		renderer->BindTexture(RENDERER_TEXTURE_2D, myBindedTexture->GetGLID());
		wglBindTexImageARB(myPBuffer, WGL_FRONT_LEFT_ARB);
	}

	//GLenum err = glGetError();
	//	KIGS_ASSERT(glGetError()==GL_NO_ERROR);
	//wglMakeCurrent(NULL, NULL);
	KIGS_ASSERT(myActivatedCount == 1);
	myActivatedCount--;
}

void	OpenGLRenderingScreen::BindTexture(Texture* texture)
{
	myBindedTexture = (OpenGLTexture*)texture;

}

void	OpenGLRenderingScreen::SetWindowByHandle(void *PtrToHandle)
{
	myhWnd = *(HWND*)PtrToHandle;
}

void		OpenGLRenderingScreen::ChangeBrightness()
{
	//! Brightness is used just before swap buffer
}

void OpenGLRenderingScreen::ScreenShot(char * filename)
{
	int windowWidth = mySizeX;
	int windowHeight = mySizeY;

	HGLRC hRC = wglGetCurrentContext();

	char*  bmpBuffer = (char*)new char[windowWidth*windowHeight * 3];
	if (!bmpBuffer)
		return;

	glReadPixels((GLint)0, (GLint)0,
		(GLint)windowWidth, (GLint)windowHeight,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, bmpBuffer);

	char*  paddedBuffer = bmpBuffer;
	int paddedLineSize = windowWidth * 3;
	// padding line
	if ((windowWidth * 3) & 3) // need padding
	{
		paddedLineSize += 4 - ((windowWidth * 3) & 3);
		paddedBuffer = (char*)new char[paddedLineSize*windowHeight];

		// copy
		char* imgread = bmpBuffer;
		char* imgwrite = paddedBuffer;
		int i;
		for (i = 0; i < windowHeight; i++)
		{
			memcpy(imgwrite, imgread, windowWidth * 3);
			imgwrite += paddedLineSize;
			imgread += windowWidth * 3;
		}
	}
	else
	{
		bmpBuffer = 0;
	}
	SmartPointer<FileHandle> filePtr = Platform_fopen(filename, "wb");
	if (!filePtr)
		return;

	BITMAPFILEHEADER  bitmapFileHeader;
	BITMAPINFOHEADER  bitmapInfoHeader;

	bitmapFileHeader.bfType = 0x4D42;  //"BM"
	bitmapFileHeader.bfSize = paddedLineSize*windowHeight;
	bitmapFileHeader.bfReserved1 = 0;
	bitmapFileHeader.bfReserved2 = 0;
	bitmapFileHeader.bfOffBits =
		sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth = windowWidth;
	bitmapInfoHeader.biHeight = windowHeight;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 24;
	bitmapInfoHeader.biCompression = BI_RGB;
	bitmapInfoHeader.biSizeImage = 0;
	bitmapInfoHeader.biXPelsPerMeter = 0; // ?
	bitmapInfoHeader.biYPelsPerMeter = 0; //  ?
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant = 0;

	Platform_fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	Platform_fwrite(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	Platform_fwrite(paddedBuffer, paddedLineSize*windowHeight, 1, filePtr);
	Platform_fclose(filePtr);

	delete[] paddedBuffer;
	if (bmpBuffer)
	{
		delete[] bmpBuffer;
	}
}

DEFINE_METHOD(OpenGLRenderingScreen, Snapshot)
{
	if (privateParams)
	{
		char * filename = (char *)privateParams;
		ScreenShot(filename);
	}
	return true;
}
