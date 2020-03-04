#ifndef _ANDROIDOPENGLRENDERINGSCREEN_H
#define _ANDROIDOPENGLRENDERINGSCREEN_H

class OpenGLTexture;


#include "Platform/Renderer/OpenGLInclude.h"


class OpenGLPlatformRenderingScreen : public RenderingScreen
{
public:
    DECLARE_ABSTRACT_CLASS_INFO(OpenGLPlatformRenderingScreen,RenderingScreen,Renderer)

	OpenGLPlatformRenderingScreen(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
     
   
	void	SetWindowByHandle(void *PtrToHandle) override {	}
	void*	GetContextHandle() override { return 0; }
	

protected:
	virtual ~OpenGLPlatformRenderingScreen();
	virtual void	InitModifiable() override;
	virtual void InitializeGL(GLsizei width, GLsizei height) = 0;
	void setCurrentContext() override
	{
	}
};    

#endif //_ANDROIDOPENGLRENDERINGSCREEN_H