#ifndef _WINDOWJAVASCRIPT_H_
#define _WINDOWJAVASCRIPT_H_

#include "Window.h"

// ****************************************
// * WindowJavascript class
// * --------------------------------------
/**
* \file	WindowJavascript.h
* \class	WindowJavascript
* \ingroup GUIModule
* \brief	Javascript Window class.
*/
// ****************************************
class WindowJavascript : public Window
{
public:
    DECLARE_CLASS_INFO(WindowJavascript,Window,GUI)

	//! constructor
    WindowJavascript(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
	//! show method, display the window
    void  Show() override;
	//! update method, call rendering screen update and manage messages
	void Update(const Timer&  timer, void* /*addParam*/) override;
	//! show or hide system mouse
	void ShowMouse(bool bOn) override;
    
	//! callback called when window is resized
	//static void	Resize(HWND hWnd, int width,int height);

	//! Set parent Window for creation
	//virtual void SetParentWindow(HWND hParent);
	
	void	GetMousePosInWindow(int posx,int posy,kfloat& wposx,kfloat& wposy) override;
	void	GetMousePosInDesignWindow(int posx,int posy,kfloat& wposx,kfloat& wposy) override;

  	//! destructor
	virtual ~WindowJavascript();
   
protected:
    

	//! os dependant Window creation, or fullscreen mode setup
    void  ProtectedInit() override;
	
	static	WindowJavascript*	mFirstWindow;
	maBool mAbsolutePos = BASE_ATTRIBUTE(AbsolutePos, true);
};    

#endif //_WINDOWJAVASCRIPT_H_
