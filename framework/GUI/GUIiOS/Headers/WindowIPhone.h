#ifndef _WINDOWIPHONE_H_
#define _WINDOWIPHONE_H_

#include "Window.h"

//#import <UIKit/UIKit.h>
//#import <QuartzCore/QuartzCore.h>
// ****************************************
// * WindowIPhone class
// * --------------------------------------
/*!  \class WindowIPhone
     a IPhone window (UIView), used for rendering, inherit Window base class from GUI module
     \ingroup GUIWindowsModule
*/
// ****************************************


class WindowIPhone : public Window
{
public:
    DECLARE_CLASS_INFO(WindowIPhone,Window,GUI)

	//! constructor
    WindowIPhone(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
	//! show method, display the window
    void  Show();
	//! update method, call rendering screen update and manage messages
   virtual void Update(const Timer&  timer, void* /*addParam*/);
	//! show or hide system mouse
	void ShowMouse(bool bOn);
    
	//! callback called when window is resized
	//static void	Resize(HWND hWnd, int width,int height);

	//! Set parent Window for creation
	//virtual void SetParentWindow(HWND hParent);
    
    virtual void	GetMousePosInWindow(int posx,int posy,kfloat& wposx,kfloat& wposy);
	virtual void	GetMousePosInDesignWindow(int posx,int posy,kfloat& wposx,kfloat& wposy);
protected:
    
	//! destructor
	virtual ~WindowIPhone();
 
	//! os dependant Window creation, or fullscreen mode setup
    void  ProtectedInit();
	
	//UIView*	myView; // can be a UIView or an EAGLView
};    

#endif //_WINDOWIPHONE_H_
