


extern void*			globalGLContext;
extern void setCurrentGLContext(void* context);

IMPLEMENT_CLASS_INFO(OpenGLRenderingScreen)

OpenGLRenderingScreen::OpenGLRenderingScreen(const kstl::string& name, CLASS_NAME_TREE_ARG) : RenderingScreen(name, PASS_CLASS_NAME_TREE_ARG)
{
	myContext = 0;
	myBindedTexture = 0;
	sBuffer = -1;
}

OpenGLRenderingScreen::~OpenGLRenderingScreen()
{
	if (!myIsOffScreen)
	{
	}

	if (sBuffer != 0xffffffff)
		glDeleteBuffers(1, &sBuffer);

	//	[myContext release];
	myContext = 0;
}

void*	OpenGLRenderingScreen::GetContextHandle()
{
	//	return (__bridge void*)myContext;
	// return myContext;
	return 0;
}

void    OpenGLRenderingScreen::InitModifiable()
{
	if (!myIsOffScreen)
	{
		kstl::set<CoreModifiable*>	instances;
		CoreModifiable::GetInstancesByName("Window", myParentWindowName, instances);

		if (instances.size())
		{
			Window* parentwindow = (Window*)(*instances.begin());
			if (!parentwindow->IsInit())
			{
				KIGS_ERROR("A rendering screen needs a parent window", 1);
				return;
			}

			int sizeX, sizeY;
			parentwindow->getValue("SizeX", sizeX);
			parentwindow->getValue("SizeY", sizeY);

			mySizeX = (unsigned int)sizeX;
			mySizeY = (unsigned int)sizeY;

			myContext = globalGLContext;
		
		}
	}



	RenderingScreen::InitModifiable();
	myIsInit = true;

	InitializeGL(mySizeX, mySizeY);
}

void OpenGLRenderingScreen::InitializeGL(int width, int height)
{
	mySizeX = (unsigned int)width;
	mySizeY = (unsigned int)height;
	glViewport(0, 0, width, height);

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
		InitializeGL(sizeX, sizeY);
	}
}

void    OpenGLRenderingScreen::Update(const Timer&  timer, void* addParam)
{

	RendererOpenGL* renderer = (RendererOpenGL*)((ModuleRenderer*)KigsCore::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();

	renderer->SetClearColorValue(myBackDropColor[0], myBackDropColor[1], myBackDropColor[2], 0.0);
	renderer->FlushState();
	RenderingScreen::Update(timer, addParam);
	// brightness (fade) effect here
	ManageFade();


	glFlush();
	glFinish();

}

bool    OpenGLRenderingScreen::SetActive(TravState* state)
{
	//KIGS_MESSAGE("SetScreen active");
	RenderingScreen::SetActive(state);
	setCurrentGLContext(myContext);

	if (myIsOffScreen)
	{
		if (myBindedTexture)
		{
			glBindTexture(GL_TEXTURE_2D, myBindedTexture->GetGLID());
		}
	}

	return true;
}

void    OpenGLRenderingScreen::Release()
{
	if (myBindedTexture && myIsOffScreen)
	{
		glBindTexture(GL_TEXTURE_2D, myBindedTexture->GetGLID());
	}

	KIGS_ASSERT(glGetError() == GL_NO_ERROR);
}

void	OpenGLRenderingScreen::BindTexture(Texture* texture)
{
	myBindedTexture = (OpenGLTexture*)texture;
}



void OpenGLRenderingScreen::FetchPixels(int x, int y, int width, int height, void *pRGBAPixels)
{
	glFlush();
	glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pRGBAPixels);
}
