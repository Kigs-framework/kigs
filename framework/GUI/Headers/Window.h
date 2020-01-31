#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "Timer.h"
class   RenderingScreen;

// ****************************************
// * Window class
// * --------------------------------------
/*!  \class Window
     base class for GUI window (rendering window)

	 exposed parameters :

	 "FullScreen" (boolean) : if true, init a fullscreen window
	 "PositionX" (int) : position of the upper left corner on screen 
	 "PositionY" (int) : position of the upper left corner on screen 
     "SizeX" (int) : rendering zone width in pixel
     "SizeY" (int) : rendering zone height in pixel
     "IsMainWindow" (boolean) : for keyboard / mouse input, is this window the main window
	 "Mouse Cursor" (boolean) : obsolete 

     \ingroup GUIModule
*/
// ****************************************

class Window : public CoreModifiable
{
public:
    
    DECLARE_ABSTRACT_CLASS_INFO(Window,CoreModifiable,GUI)

    //! Typedefs for callbacks for the window events. They are not used by other projects at the moment
	typedef void (*ClickMessageCallbackFn)(CoreModifiable *pWindow, int buttonId, kfloat X, kfloat Y,bool isDown);
    //! Typedefs for callbacks for the window events. They are not used by other projects at the moment
	typedef void (*KeyDownCallbackFn)(Window *pWindow, char C, int VirtualKeyCode);
    //! Typedefs for callbacks for the window events. They are not used by other projects at the moment
	typedef void (*DestroyCallbackFn)(CoreModifiable *pWindow);

	//! constructor
    Window(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/*! pure virtual show. 
		Must be implemented in platform/os dependant class
	*/
    virtual void  Show()=0;
   
	/*! pure virtual showmouse. 
		Must be implemented in platform/os dependant class
	*/
	virtual void ShowMouse(bool bOn)=0;

	virtual void ShowBorder(bool show) {}

	virtual void SetWindowPosition(v2i pos, v2i size, bool force_topmost = false) {}

	//! return the rendering screen associated with this window
	RenderingScreen*	GetRenderingScreen(){return myScreen;}

	//!Set the callback which is called when the user types the given character
	void SetKeyDownCallback(KeyDownCallbackFn Callback) {myKeyDownCallback = Callback;}
	//!Set the callback which is called when the user types the given character (key up)
	void SetKeyUpCallback(KeyDownCallbackFn Callback) {myKeyUpCallback = Callback;}
	//!Set the callback which is called when the window is destroyed
	void SetDestroyCallback(DestroyCallbackFn Callback) {myDestroyCallback = Callback;}
	//!Set the callback when clicking in the window (X,Y) range in pixels (from left to right, bottom to up)
	void SetClickCallback(ClickMessageCallbackFn Callback) {myClickCallback = Callback;}
	//!Set the callback when double clicking in the window (X,Y) range in pixels (from left to right, bottom to up)
	void SetDoubleClickCallback(ClickMessageCallbackFn Callback) {myDoubleClickCallback = Callback;}

	//! add item. Manage rendering screen
	bool	addItem(const CMSP& item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME) override;
	//! remove item. Manage rendering screen
	bool	removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;

	//! return the window handle
	void*	GetHandle(){return myHandle;}

	Point2DI	GetPos() { return { myPosX, myPosY }; }
	Point2DI    GetSize() { return { mySizeX, mySizeY }; }

	virtual void	GetMousePosInWindow(int posx,int posy,kfloat& wposx,kfloat& wposy)=0;
	virtual void	GetMousePosInDesignWindow(int posx,int posy,kfloat& wposx,kfloat& wposy)=0;

	virtual void ChangeWindowText(const char * txt){}

protected:
	//! destructor
    virtual ~Window();
    
	/*! \brief pure virtual protected init
		called by the InitModifiable method, must be implemented in platform/os dependant classes
	*/
	virtual void  ProtectedInit()=0;  

	//! init method
	void	InitModifiable() override;

	//! parameter for fullscreen window
	maBool	myFullScreen;
	//! flag to show/hide the mouse
	maBool	m_bShowMouse;

	//! flag to decide if the window is the main window. If you close the main window, the application should be terminated
	maBool myIsMainWindow;
	maUInt myDisplayIndex;

	maBool myDirtySize;

	//! window size and position on screen (size is also used if fullscreen)
    maInt     myPosX,myPosY,mySizeX,mySizeY;

	//!	platform independant handle
    void*   myHandle; 

	//! windows rendering screen
	RenderingScreen*	myScreen;

	//! mouse callbacks
	ClickMessageCallbackFn myClickCallback, myDoubleClickCallback;
	//! key callback
	KeyDownCallbackFn myKeyDownCallback,myKeyUpCallback;
	//! destroy callback
	DestroyCallbackFn myDestroyCallback;
};    

#endif
