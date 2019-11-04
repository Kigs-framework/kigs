#include "Core.h"
#include "CoreBaseApplication.h"
#include "TextureFileManager.h"

#include "Platform/Main/BaseApp.h"
#include "Platform/Main/angle_includes.h"

IMPLEMENT_CLASS_INFO(OpenGLPlatformRenderingScreen)

OpenGLPlatformRenderingScreen::OpenGLPlatformRenderingScreen(const kstl::string& name, CLASS_NAME_TREE_ARG) : RenderingScreen(name, PASS_CLASS_NAME_TREE_ARG)
{
	
}

OpenGLPlatformRenderingScreen::~OpenGLPlatformRenderingScreen()
{
	
}

void    OpenGLPlatformRenderingScreen::InitModifiable()
{
	if (IsInit()) return;
	RenderingScreen::InitModifiable();

	auto app = App::GetApp();
	if (MyParentWindow && MyParentWindow->IsInit())
	{
		auto si = MyParentWindow->GetSize();
		InitializeGL(si.x, si.y);
		//InitializeGL(app->GetWindowWidth(), app->GetWindowHeight());
	}
}

void OpenGLPlatformRenderingScreen::Update(const Timer&  timer, void* addParam)
{
	RenderingScreen::Update(timer, addParam);
	auto app = App::GetApp();
	app->Swap();
}

void	OpenGLPlatformRenderingScreen::UninitModifiable()
{
}

