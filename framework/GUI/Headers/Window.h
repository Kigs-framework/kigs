#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "Timer.h"
class   RenderingScreen;


// ****************************************
// * Window class
// * --------------------------------------
/**
* \file	Window.h
* \class	Window
* \ingroup GUIModule
* \brief	Base class for Window (rendering window).
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
	RenderingScreen*	GetRenderingScreen(){return mScreen;}

	//!Set the callback which is called when the user types the given character
	void SetKeyDownCallback(KeyDownCallbackFn Callback) {mKeyDownCallback = Callback;}
	//!Set the callback which is called when the user types the given character (key up)
	void SetKeyUpCallback(KeyDownCallbackFn Callback) {mKeyUpCallback = Callback;}
	//!Set the callback which is called when the window is destroyed
	void SetDestroyCallback(DestroyCallbackFn Callback) {mDestroyCallback = Callback;}
	//!Set the callback when clicking in the window (X,Y) range in pixels (from left to right, bottom to up)
	void SetClickCallback(ClickMessageCallbackFn Callback) {mClickCallback = Callback;}
	//!Set the callback when double clicking in the window (X,Y) range in pixels (from left to right, bottom to up)
	void SetDoubleClickCallback(ClickMessageCallbackFn Callback) {mDoubleClickCallback = Callback;}

	//! add item. Manage rendering screen
	bool	addItem(const CMSP& item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME) override;
	//! remove item. Manage rendering screen
	bool	removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;

	//! return the window handle
	void*	GetHandle(){return mHandle;}

	Point2DI	GetPos() { return { mPositionX, mPositionY }; }
	Point2DI    GetSize() { return { mSizeX, mSizeY }; }

	virtual void	GetMousePosInWindow(int posx,int posy,kfloat& wposx,kfloat& wposy)=0;
	virtual void	GetMousePosInDesignWindow(int posx,int posy,kfloat& wposx,kfloat& wposy)=0;

	virtual void ChangeWindowText(const char * txt){}

	virtual bool IsPlatformClipboardSupported() { return false; }
	virtual const std::string& GetClipboardText() { return mClipboard; };
	virtual void SetClipboardText(const std::string& txt) { mClipboard = txt; };

	virtual void SetCurrentCursor(const char* cursorName) {};

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
	maBool	mFullScreen;
	//! flag to show/hide the mouse
	maBool	mShowMouseCursor;

	//! flag to decide if the window is the main window. If you close the main window, the application should be terminated
	maBool mIsMainWindow;
	maUInt mDisplayIndex;

	maBool mDirtySize;

	maBool mShow = BASE_ATTRIBUTE(Show, true);

	//! window size and position on screen (size is also used if fullscreen)
    maInt     mPositionX,mPositionY,mSizeX,mSizeY;

	//!	platform independant handle
    void*   mHandle; 

	//! windows rendering screen
	RenderingScreen*	mScreen;

	//! mouse callbacks
	ClickMessageCallbackFn mClickCallback, mDoubleClickCallback;
	//! key callback
	KeyDownCallbackFn mKeyDownCallback,mKeyUpCallback;
	//! destroy callback
	DestroyCallbackFn mDestroyCallback;

	std::string mClipboard;
};    

#endif
