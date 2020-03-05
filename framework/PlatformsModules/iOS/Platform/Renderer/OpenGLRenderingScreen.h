#ifndef _IOSOPENGLRENDERINGSCREEN_H
#define _IOSOPENGLRENDERINGSCREEN_H

#include "OpenGLTexture.h"

class RenderingView;
class TravState;

class OpenGLRenderingScreen : public RenderingScreen
{
public:
    DECLARE_CLASS_INFO(OpenGLRenderingScreen,RenderingScreen,Renderer)

    OpenGLRenderingScreen(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
     
    virtual void    Update(const Timer& timer, void* addParam);
	bool    SetActive(TravState* state);  
	void    Release(TravState* state);

	void	Resize(kfloat sizeX,kfloat sizeY);
   
	void	SetWindowByHandle(void *PtrToHandle)
	{
		
	}
	void*	GetContextHandle();
    /*
    IOSViewController* getController()
    {
        return myViewController;
    }
    */


	void	BindTexture(Texture*);

	RenderingView*	getView()
	{
		return myRenderingView;
	}
		

	virtual void	FetchPixels(int x, int y, int width, int height, void *pRGBAPixels);

	virtual void	FetchDepth(int x, int y, int width, int height, float *pDepthPixels)
	{
		// TODO ?
	}
	virtual void	FetchDepth(int x, int y, int width, int height, unsigned int *pDepthPixels)
	{
		// TODO ?
	}

	
protected:
    
	virtual void	InitModifiable();
	
	virtual void		ManageFade(TravState*);
	
	virtual ~OpenGLRenderingScreen();
	
	void InitializeGL(int width, int height);

	void *			myContext;
	RenderingView*	myRenderingView;
	// IOSViewController* myViewController;
	OpenGLTexture*		myBindedTexture;
	unsigned int myViewRenderbuffer;
	unsigned int myViewFramebuffer;
	unsigned int myDepthRenderbuffer;
	unsigned int sBuffer;


};    

#endif //_IOSOPENGLRENDERINGSCREEN_H