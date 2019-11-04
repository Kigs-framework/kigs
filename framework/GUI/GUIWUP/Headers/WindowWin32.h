#ifndef _WINDOWWIN32_H_
#define _WINDOWWIN32_H_

#include "Window.h"


// ****************************************
// * WindowWin32 class
// * --------------------------------------
/*!  \class WindowWin32
     a win32 window, used for rendering, inherit Window base class from GUI module
     \ingroup GUIWindowsModule
*/
// ****************************************

#define PSEUDO_FULLSCREEN

class WindowWin32 : public Window
{
public:
    DECLARE_CLASS_INFO(WindowWin32,Window,GUI)

	//! constructor
    WindowWin32(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
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
    
	//! destructor
	virtual ~WindowWin32();
 
	//! os dependant Window creation, or fullscreen mode setup
    virtual void  ProtectedInit();
			
	//! manage screensaver desactivation
	bool		   myScreenSaverActive;
	

	float mSizeXToSet = 0.0f;
	float mSizeYToSet = 0.0f;
	bool mHasEvent = false;
	bool mRetryResize = false;
};    

#endif //_WINDOWWIN32_H_
