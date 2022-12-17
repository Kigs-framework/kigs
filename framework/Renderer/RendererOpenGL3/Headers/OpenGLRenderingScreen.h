#ifndef _OPENGLRENDERINGSCREEN_H
#define _OPENGLRENDERINGSCREEN_H

#include "RenderingScreen.h"

// rendering screen is really to specific to share accross platform
#include "Platform/Renderer/OpenGLRenderingScreen.h"

// ****************************************
// * OpenGLRenderingScreen class
// * --------------------------------------
/**
 * \file	OpenGLRenderingScreen.h
 * \class	OpenGLRenderingScreen
 * \ingroup Renderer
 * \brief	OpenGL implementation of RendereingScreen.
 */
 // ****************************************
class OpenGLRenderingScreen : public OpenGLPlatformRenderingScreen
{
public:
	DECLARE_CLASS_INFO(OpenGLRenderingScreen, OpenGLPlatformRenderingScreen, Renderer)
	OpenGLRenderingScreen(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~OpenGLRenderingScreen();

	bool    SetActive(TravState* state) override;
	void    Release(TravState* state) override;

	void	Resize(float sizeX, float sizeY) override;


	void	FetchPixels(int x, int y, int width, int height, void *pRGBAPixels)override;
	void	FetchDepth(int x, int y, int width, int height, float *pDepthPixels)override;
	void	FetchDepth(int x, int y, int width, int height, unsigned int *pDepthPixels)override;

	void	ScreenShot(char * filename);
	DECLARE_METHOD(Snapshot);
	COREMODIFIABLE_METHODS(Snapshot);
protected:
	void	DelayedInit();
	void Update(const Timer&  timer, void* /*addParam*/) override;

	void	InitModifiable() override;
	void	UninitModifiable() override;

	void InitializeGL(GLsizei width, GLsizei height) override;

	unsigned int	mFBOFrameBufferID = 0xffffffff;
	unsigned int	mFBODepthBufferID = 0xffffffff;
	unsigned int	mFBOStencilBufferID = 0xffffffff;
	unsigned int	mFBOTextureID = 0xffffffff;

	unsigned int	mDefaultFrameBuffer = 0xffffffff;
};


#endif //_OPENGLRENDERINGSCREEN_H
