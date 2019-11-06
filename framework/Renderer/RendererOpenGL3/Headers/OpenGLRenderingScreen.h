#ifndef _OPENGLRENDERINGSCREEN_H
#define _OPENGLRENDERINGSCREEN_H

#include "RenderingScreen.h"

// rendering screen is really to specific to share accross platform
#include "Platform/Renderer/OpenGLRenderingScreen.h"

class OpenGLRenderingScreen : public OpenGLPlatformRenderingScreen
{
public:
	DECLARE_CLASS_INFO(OpenGLRenderingScreen, OpenGLPlatformRenderingScreen, Renderer)

	OpenGLRenderingScreen(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	bool    SetActive(TravState* state) override;
	void    Release(TravState* state) override;

	void	Resize(kfloat sizeX, kfloat sizeY) override;


	void	FetchPixels(int x, int y, int width, int height, void *pRGBAPixels)override;
	void	FetchDepth(int x, int y, int width, int height, float *pDepthPixels)override;
	void	FetchDepth(int x, int y, int width, int height, unsigned int *pDepthPixels)override;

	void	ScreenShot(char * filename);
	DECLARE_METHOD(Snapshot);
	
protected:

	void	DelayedInit();

	void Update(const Timer&  timer, void* /*addParam*/) override;

	void	InitModifiable() override;
	void	UninitModifiable() override;

	virtual ~OpenGLRenderingScreen();

	void InitializeGL(GLsizei width, GLsizei height) override;

	unsigned int	fbo_frame_buffer_id;
	unsigned int	fbo_depth_buffer_id;
	unsigned int	fbo_stencil_buffer_id;
	unsigned int	fbo_texture_id;

	int	default_frame_buffer;
};


#endif //_OPENGLRENDERINGSCREEN_H
