
#include "ModuleRenderer.h"
#include "OpenGLRenderingScreen.h"
#include "math.h"
#include "OpenGLTexture.h"
#include "Window.h"
#include "Core.h"
#include "RendererOpenGL.h"
#include "GLSLUIShader.h"
#include "FilePathManager.h"
#include "UIVerticesInfo.h"

#include "TimeProfiler.h"


// include after all other kigs include (for iOS)
#include "Platform/Renderer/OpenGLInclude.h"

// rendering screen is too much platform specific
#include "Platform/Renderer/OpenGL3RenderingScreen.inl.h"

IMPLEMENT_CLASS_INFO(OpenGLRenderingScreen)

OpenGLRenderingScreen::OpenGLRenderingScreen(const kstl::string& name, CLASS_NAME_TREE_ARG) : OpenGLPlatformRenderingScreen(name, PASS_CLASS_NAME_TREE_ARG)
{
	
}

OpenGLRenderingScreen::~OpenGLRenderingScreen()
{

}


void OpenGLRenderingScreen::FetchPixels(int x, int y, int width, int height, void *pRGBAPixels)
{
	glFlush();
	glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pRGBAPixels);
}

void OpenGLRenderingScreen::FetchDepth(int x, int y, int width, int height, float *pDepthPixels)
{
	glFlush();
	y = mSizeY - 1 - y;
	glReadPixels(x, y, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, pDepthPixels);
}

void OpenGLRenderingScreen::FetchDepth(int x, int y, int width, int height, unsigned int *pDepthPixels)
{
	glFlush();
	y = mSizeY - 1 - y;
	glReadPixels(x, y, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, pDepthPixels);
}

void	OpenGLRenderingScreen::Resize(kfloat sizeX, kfloat sizeY)
{
	// before screen is init, only update sizeX and sizeY
	if (!IsInit())
	{
		mSizeX = (unsigned int)sizeX;
		mSizeY = (unsigned int)sizeY;
	}
	else
	{
		//When resizing an offscreen surface, we keep the big surface but we let the camera clip the scene
		InitializeGL((int)sizeX, (int)sizeY);
	}

	if (mResizeDesignSize)
	{
		mDesignSizeX = mSizeX;
		mDesignSizeY = mSizeY;
	}

	RecomputeDesignCoef();

	EmitSignal(Signals::Resized);
}


void OpenGLRenderingScreen::InitializeGL(GLsizei width, GLsizei height)
{
	RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer); // (RendererOpenGL*)((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();

	mSizeX = (unsigned int)width;
	mSizeY = (unsigned int)height;
	kfloat    aspect;
	
	renderer->SetClearColorValue(0.8f, 0.8f, 1.0f, 0.0f);
	renderer->SetDepthValueMode(1.0);
	renderer->SetDepthTestMode(true);

	aspect = (float)width / (float)height;
	renderer->Perspective(MATRIX_MODE_PROJECTION, 45.0f, aspect, 0.1f, 20.0f);

	renderer->LoadIdentity(MATRIX_MODE_MODEL);
	renderer->LoadIdentity(MATRIX_MODE_VIEW);
	renderer->LoadIdentity(MATRIX_MODE_UV);
	
}

bool    OpenGLRenderingScreen::SetActive(TravState* state)
{
	CoreModifiableAttribute* delayed = getAttribute("DelayedInit");
	if (delayed)
	{
		DelayedInit();
	}

	setCurrentContext();

	OpenGLPlatformRenderingScreen::SetActive(state);

	if (mUseFBO)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mFBOFrameBufferID); CHECK_GLERROR;
	}

	RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer);

	renderer->SetCullMode((RendererCullMode)(int)RENDERER_CULL_NONE);
	return true;
}

void    OpenGLRenderingScreen::Release(TravState* state)
{
	RendererOpenGL* renderer = reinterpret_cast<RendererOpenGL*>(ModuleRenderer::mTheGlobalRenderer);

	renderer->GetVertexBufferManager()->DoDelayedAction();

	if (mUseFBO)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mDefaultFrameBuffer); CHECK_GLERROR;
		if (!mIsOffScreen && state)
		{

			renderer->pushShader((API3DShader*)renderer->getDefaultUiShader().get(), state);

			unsigned int shader_flag = ModuleRenderer::VERTEX_ARRAY_MASK | ModuleRenderer::NO_LIGHT_MASK;
			shader_flag |= ModuleRenderer::TEXCOORD_ARRAY_MASK;

			renderer->GetActiveShader()->ChooseShader(state, shader_flag);

			//glClear(0);

			renderer->SetBlendMode(RENDERER_BLEND_OFF);
			renderer->SetLightMode(RENDERER_LIGHT_OFF);
			renderer->SetDepthTestMode(false);
			renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_OFF);
			renderer->Ortho(MATRIX_MODE_PROJECTION, 0.0f, (float)mSizeX, 0.0f, (float)mSizeY, -1.0f, 1.0f);
			renderer->LoadIdentity(MATRIX_MODE_MODEL);
			renderer->LoadIdentity(MATRIX_MODE_VIEW);
			renderer->LoadIdentity(MATRIX_MODE_UV);
			renderer->BindTexture(RENDERER_TEXTURE_2D, mFBOTextureID);
			renderer->SetScissorValue(0, 0, mSizeX, mSizeY);
			renderer->Viewport(0, 0, mSizeX, mSizeY);

			VInfo2D vi;
			UIVerticesInfo mVI = UIVerticesInfo(&vi);
			VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(mVI.Buffer());

			buf[0].setVertex(0.0f, 0.0f);
			buf[1].setVertex(0.0, (float)mSizeY);
			buf[2].setVertex((float)mSizeX, 0.0);
			buf[3].setVertex((float)mSizeX, (float)mSizeY);

			//Draw Quad that fits the screen

			buf[0].setTexUV(0.0f, 0.0f);
			buf[1].setTexUV(0.0f, 1.0f);
			buf[2].setTexUV(1.0f, 0.0f);
			buf[3].setTexUV(1.0f, 1.0f);
			

			mVI.SetFlag(UIVerticesInfo_Vertex | UIVerticesInfo_Texture);

			renderer->DrawUIQuad(state, &mVI);

			renderer->popShader((API3DShader*)renderer->getDefaultUiShader().get(), state);
		}
	}

}


void OpenGLRenderingScreen::ScreenShot(char * filename)
{
	// TODO : use TinyImage and fetchpixel

/*	int windowWidth = mSizeX;
	int windowHeight = mSizeY;


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
	}*/
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


void OpenGLRenderingScreen::Update(const Timer&  timer, void* addParam)
{
	if (addParam && !*(bool*)addParam) return;

	setCurrentContext();

	OpenGLPlatformRenderingScreen::Update(timer, addParam);

}

void	OpenGLRenderingScreen::UninitModifiable()
{
	OpenGLPlatformRenderingScreen::UninitModifiable();

	// reset shaders
	//API3DShader::ResetContext();

	ModuleSpecificRenderer* renderer = ModuleRenderer::mTheGlobalRenderer;
	renderer->UninitHardwareState();
}


void    OpenGLRenderingScreen::InitModifiable()
{
	if (_isInit)
		return;

#ifdef NO_DELAYED_INIT
	DelayedInit();
#else
	CoreModifiableAttribute* delayed = getAttribute("DelayedInit");
	if (delayed) // delayed init already asked
	{
		return;
	}
	// ask for delayed init
	CoreModifiableAttribute* newAttr = AddDynamicAttribute(ATTRIBUTE_TYPE::BOOL, "DelayedInit");
	newAttr->setValue(true);
#endif
}

void	OpenGLRenderingScreen::DelayedInit()
{
	ModuleSpecificRenderer* renderer = ModuleRenderer::mTheGlobalRenderer; // ((ModuleRenderer*)Core::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	OpenGLRenderingScreen* firstScreen = (OpenGLRenderingScreen*)renderer->getFirstRenderingScreen();

	if (firstScreen != this)
	{
		if (firstScreen->mIsInit == false)
		{
			firstScreen->DelayedInit();
		}
	}

	OpenGLPlatformRenderingScreen::InitModifiable();
#ifdef WUP
	if (mIsStereo && !gIsHolographic) mIsStereo = false;
#endif
	mIsInit = true;

	if (firstScreen == this)
		renderer->InitHardwareState();

	if (mUseFBO)
	{
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mDefaultFrameBuffer); CHECK_GLERROR;

		glGenFramebuffers(1, &mFBOFrameBufferID); CHECK_GLERROR;
		int zbits = (int)mBitsPerZ;

		bool use_depth_buffer = zbits != 0;

		if (use_depth_buffer)
		{
			glGenRenderbuffers(1, &mFBODepthBufferID);
		}


		// create texture with fbo
		auto& texfileManager = KigsCore::Singleton<TextureFileManager>();
		mFBOTexture = texfileManager->CreateTexture(getName());
		mFBOTexture->setValue("Width", mFBOSizeX);
		mFBOTexture->setValue("Height", mFBOSizeY);
		mFBOTexture->InitForFBO();
		mFBOTexture->SetRepeatUV(false, false);

		mFBOTextureID = ((OpenGLTexture*)mFBOTexture.get())->GetGLID();

		renderer->ActiveTextureChannel(0);

		renderer->BindTexture(RENDERER_TEXTURE_2D, mFBOTextureID);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mFBOSizeX, mFBOSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		if (use_depth_buffer)
		{
#ifdef GL_ES2
			if (zbits != 16)
			{
				KIGS_WARNING("OpenGL ES 2.0 only supports 16 bits fot the depth buffer", 3);
			}
#endif

			glBindRenderbuffer(GL_RENDERBUFFER, mFBODepthBufferID);

#ifndef GL_ES2
			if (zbits == 32)
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, mFBOSizeX, mFBOSizeY);
			else if (zbits == 24)
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mFBOSizeX, mFBOSizeY);
			else if (zbits == 16)
#endif
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mFBOSizeX, mFBOSizeY);

#ifndef GL_ES2
			else
			{
				KIGS_WARNING("Unsupported depth buffer bit size, using 32 bits instead", 3);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, mFBOSizeX, mFBOSizeY);
			}
#endif

		}

		int stencilbits = mBitsForStencil;
		if (stencilbits != 0)
		{
#ifdef GL_ES2
			if (stencilbits != 8)
				KIGS_WARNING("OpenGL ES 2.0 only supports 8 bits for the stencil buffer", 3);
#endif
			//glGenRenderbuffers(1, &mFBOStencilBufferID);
			mFBOStencilBufferID = mFBODepthBufferID;
			glBindRenderbuffer(GL_RENDERBUFFER, mFBOStencilBufferID);
			if (stencilbits == 8)
				glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, mFBOSizeX, mFBOSizeY);
#ifndef GL_ES2
			else if (stencilbits == 16)
				glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX16, mFBOSizeX, mFBOSizeY);
#endif
			else
			{
				KIGS_WARNING("Unsupported stencil buffer bit size, using 8 bits instead", 3);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, mFBOSizeX, mFBOSizeY);
			}
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mFBOStencilBufferID);

		}

		glBindFramebuffer(GL_FRAMEBUFFER, mFBOFrameBufferID); CHECK_GLERROR;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFBOTextureID, 0); CHECK_GLERROR;

		if(use_depth_buffer)
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mFBODepthBufferID);



#ifndef GL_ES2
		unsigned int DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers); CHECK_GLERROR;
#endif
		GLenum status;
		if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
		{
			STACK_STRING(str, 1024, "glCheckFramebufferStatus error: %d", status);
			KIGS_ERROR(str, 1);
			return;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0); CHECK_GLERROR;

	RemoveDynamicAttribute("DelayedInit");
}
