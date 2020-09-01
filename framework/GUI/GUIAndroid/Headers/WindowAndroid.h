#ifndef _WINDOWANDROID_H_
#define _WINDOWANDROID_H_

#include "Window.h"


// ****************************************
// * WindowAndroid class
// * --------------------------------------
/**
* \file	WindowAndroid.h
* \class	WindowAndroid
* \ingroup GUIModule
* \brief Specific Window class for Android platform.
*
*/
// ****************************************
class WindowAndroid : public Window
{
public:
    DECLARE_CLASS_INFO(WindowAndroid,Window,GUI)

	//! constructor
    WindowAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
	//! show method, display the window
    void Show() override;
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

    
protected:
    
	//! destructor
	virtual ~WindowAndroid();
 
	//! os dependant Window creation, or fullscreen mode setup
    void  ProtectedInit() override;
	
	//UIView*	myView; // can be a UIView or an EAGLView
};    

#endif //_WINDOWANDROID_H_
