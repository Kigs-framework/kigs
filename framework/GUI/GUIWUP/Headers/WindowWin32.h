#ifndef _WINDOWWIN32_H_
#define _WINDOWWIN32_H_

#include "Window.h"


#define PSEUDO_FULLSCREEN
// ****************************************
// * WindowWin32 class
// * --------------------------------------
/**
* \file	WindowWin32.h
* \class	WindowWin32
* \ingroup GUIModule
* \brief	Win32 UWP Window class.
*/
// ****************************************
class WindowWin32 : public Window
{
public:
    DECLARE_CLASS_INFO(WindowWin32,Window,GUI)
    WindowWin32(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~WindowWin32();

	//! show method, display the window
	virtual void  Show() {};

	//! update method, call rendering screen update and manage messages
	virtual void Update(const Timer&  timer, void* /*addParam*/);

	//! show or hide system mouse
	void ShowMouse(bool bOn);

	virtual void	GetMousePosInWindow(int posx, int posy, kfloat& wposx, kfloat& wposy);
	virtual void	GetMousePosInDesignWindow(int posx, int posy, kfloat& wposx, kfloat& wposy);
		
	virtual void ChangeWindowText(const char * txt)
	{
		//TODO
	}
    
protected:
	//! os dependant Window creation, or fullscreen mode setup
    virtual void  ProtectedInit();
			
	//! manage screensaver desactivation
	bool		   mScreenSaverActive;
	

	float mSizeXToSet = 0.0f;
	float mSizeYToSet = 0.0f;
	bool mHasEvent = false;
	bool mRetryResize = false;
};    

#endif //_WINDOWWIN32_H_
