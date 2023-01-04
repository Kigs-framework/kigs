#include "GLSLShader.h"
#include "Window.h"
#include "RendererOpenGL.h"
#include "TextureFileManager.h"

namespace Kigs
{
	namespace Draw
	{
		IMPLEMENT_CLASS_INFO(OpenGLPlatformRenderingScreen)

			OpenGLPlatformRenderingScreen::OpenGLPlatformRenderingScreen(const std::string& name, CLASS_NAME_TREE_ARG) : RenderingScreen(name, PASS_CLASS_NAME_TREE_ARG)
		{

		}

		OpenGLPlatformRenderingScreen::~OpenGLPlatformRenderingScreen()
		{
		}

		void OpenGLPlatformRenderingScreen::InitModifiable()
		{
			if (_isInit)
				return;
			RenderingScreen::InitModifiable();

			InitializeGL(mSize[0], mSize[1]);

		}

	}
}