#ifndef _WIN32OPENGLRENDERINGSCREEN_H
#define _WIN32OPENGLRENDERINGSCREEN_H

class OpenGLTexture;
class UIVerticesInfo;

#include "Platform\Renderer\OpenGLInclude.h"

class OpenGLPlatformRenderingScreen : public RenderingScreen
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(OpenGLPlatformRenderingScreen, RenderingScreen, Renderer)

	OpenGLPlatformRenderingScreen(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void Update(const Timer&  timer, void* /*addParam*/) override;
	
	void	SetWindowByHandle(void *PtrToHandle) {}

	void setCurrentContext() override {}

	void*	GetContextHandle()
	{
		return nullptr;
	}

protected:

	virtual void InitializeGL(GLsizei width, GLsizei height) =0;

	void	InitModifiable() override;
	void	UninitModifiable()override;


	virtual ~OpenGLPlatformRenderingScreen();

};


#endif //_WIN32OPENGLRENDERINGSCREEN_H