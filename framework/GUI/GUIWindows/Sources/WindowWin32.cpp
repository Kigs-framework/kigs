

#include "PrecompiledHeaders.h"
#include <windows.h>
#include <Shellapi.h>
#include <propsys.h>
#include <propkey.h>
#include "MouseDevice.h"
#include "DisplayDeviceCaps.h"

// before everything else
HRESULT SetTouchDisableProperty(HWND hwnd, BOOL fDisableTouch)
{
	if (fDisableTouch)
		RegisterTouchWindow(hwnd, TWF_WANTPALM);
	else
		UnregisterTouchWindow(hwnd);

	IPropertyStore* pPropStore;
	HRESULT hrReturnValue = SHGetPropertyStoreForWindow(hwnd, IID_PPV_ARGS(&pPropStore));
	if (SUCCEEDED(hrReturnValue))
	{
		PROPVARIANT var;
		var.vt = VT_BOOL;
		var.boolVal = fDisableTouch ? VARIANT_TRUE : VARIANT_FALSE;
		hrReturnValue = pPropStore->SetValue(PKEY_EdgeGesture_DisableTouchWhenFullscreen, var);
		pPropStore->Release();
	}
	return hrReturnValue;
}




#include "WindowWin32.h"
#include "RendererIncludes.h"
#include "ModuleGUIWindows.h"
#include "Core.h"
#include "CoreBaseApplication.h"
#include <windowsx.h>

IMPLEMENT_CLASS_INFO(WindowWin32)

std::map<msgProcCallBack, msgProcCallBack>*	WindowWin32::myCallBackMap = 0;

void	WindowWin32::AddCallback(msgProcCallBack callback)
{
	if (!myCallBackMap)
	{
		myCallBackMap = new std::map<msgProcCallBack, msgProcCallBack>;
	}

	(*myCallBackMap)[callback] = callback;
}

void	WindowWin32::RemoveCallback(msgProcCallBack callback)
{
	if (myCallBackMap)
	{
		std::map<msgProcCallBack, msgProcCallBack>::iterator	itmap = (*myCallBackMap).find(callback);

		if (itmap != (*myCallBackMap).end())
		{
			(*myCallBackMap).erase(itmap);
			if ((*myCallBackMap).size() == 0)
			{
				delete myCallBackMap;
				myCallBackMap = 0;
			}
		}
	}
}




//! manage some messages (destroy and resize)
LRESULT WINAPI WindowWin32::MsgProc(HWND hWnd, ::UINT msg, WPARAM wParam, LPARAM lParam)
{
	bool messageTreated = false;
	//static HCURSOR hStdCursor = NULL;
	WindowWin32 *pWindow = (WindowWin32*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	switch (msg)
	{
		// lose focus / reset click position
	case WM_KILLFOCUS:
	{
		bool IsMainWnd;
		pWindow->getValue(LABEL_TO_ID(IsMainWindow), IsMainWnd);
		if (IsMainWnd)
		{
			if (KigsCore::GetCoreApplication())
				KigsCore::GetCoreApplication()->SetFocus(false);
		}
	}
	break;
	// gain focus (nothing yet)
	case WM_SETFOCUS:
	{
		bool IsMainWnd = true;
		pWindow->getValue(LABEL_TO_ID(IsMainWindow), IsMainWnd);
		if (IsMainWnd)
		{
			CoreBaseApplication* app = KigsCore::GetCoreApplication();
			if (app)
				KigsCore::GetCoreApplication()->SetFocus(true);
		}
	}
	break;
	// window creation
	case WM_CREATE:
	{
		CREATESTRUCT *p = (CREATESTRUCT*)lParam;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (INT_PTR)p->lpCreateParams);
		break;
	}
	case WM_SETCURSOR:
		SetCursor(pWindow->myCursor);
		break;

		//Character typed
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		if (pWindow->myKeyDownCallback)
		{
			unsigned int VirtualKeyCode, ScanCode;
			unsigned char KeyboardState[256];
			WORD ASCIIOutput;

			VirtualKeyCode = wParam;
			ScanCode = (lParam >> 16) & 0xFF;
			GetKeyboardState(KeyboardState);

			if (1 != ToAsciiEx(VirtualKeyCode, ScanCode, KeyboardState, &ASCIIOutput, 0, GetKeyboardLayout(GetWindowThreadProcessId(hWnd, NULL))))
				ASCIIOutput = 0;

			pWindow->myKeyDownCallback(pWindow, (char)ASCIIOutput, (int)VirtualKeyCode);
		}
		break;
	}

	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		if (pWindow->myKeyUpCallback)
		{
			unsigned int VirtualKeyCode, ScanCode;
			unsigned char KeyboardState[256];
			WORD ASCIIOutput;

			VirtualKeyCode = wParam;
			ScanCode = (lParam >> 16) & 0xFF;
			GetKeyboardState(KeyboardState);

			if (1 != ToAsciiEx(VirtualKeyCode, ScanCode, KeyboardState, &ASCIIOutput, 0, GetKeyboardLayout(GetWindowThreadProcessId(hWnd, NULL))))
				ASCIIOutput = 0;

			pWindow->myKeyUpCallback(pWindow, (char)ASCIIOutput, (int)VirtualKeyCode);
		}
		break;
	}
	//Click/double click
	// return X and Y between [0;1]
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		if (pWindow->myClickCallback != NULL)
		{
			messageTreated = true;
			bool isDown;
			MouseDevice::MOUSE_BUTTONS buttonId;
			switch (msg)
			{
			case WM_LBUTTONDOWN:
				buttonId = MouseDevice::LEFT;
				isDown = true;
				break;
			case WM_RBUTTONDOWN:
				buttonId = MouseDevice::RIGHT;
				isDown = true;
				break;
			case WM_MBUTTONDOWN:
				buttonId = MouseDevice::MIDDLE;
				isDown = true;
				break;
			case WM_LBUTTONUP:
				buttonId = MouseDevice::LEFT;
				isDown = false;
				break;
			case WM_RBUTTONUP:
				buttonId = MouseDevice::RIGHT;
				isDown = false;
				break;
			case WM_MBUTTONUP:
				buttonId = MouseDevice::MIDDLE;
				isDown = false;
				break;
			}

			RECT ClientArea;

			GetClientRect(hWnd, &ClientArea);
			int X = GET_X_LPARAM(lParam);
			int Y = ClientArea.bottom - 1 - GET_Y_LPARAM(lParam);

			kfloat mX = (kfloat)X / pWindow->mySizeX;
			kfloat mY = (kfloat)Y / pWindow->mySizeY;
			pWindow->myClickCallback(pWindow, buttonId, mX, mY, isDown);
			return 0;
		}
		break;
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
		if (pWindow->myDoubleClickCallback != NULL)
		{
			RECT ClientArea;

			GetClientRect(hWnd, &ClientArea);
			int X = GET_X_LPARAM(lParam);
			int Y = ClientArea.bottom - 1 - GET_Y_LPARAM(lParam);

			MouseDevice::MOUSE_BUTTONS buttonId;
			switch (msg)
			{
			case WM_RBUTTONDBLCLK:
				buttonId = MouseDevice::RIGHT;
				break;
			case WM_MBUTTONDBLCLK:
				buttonId = MouseDevice::MIDDLE;
				break;
			case WM_LBUTTONDBLCLK:
				buttonId = MouseDevice::LEFT;
				break;
			}

			pWindow->myDoubleClickCallback(pWindow, buttonId, (kfloat)X, (kfloat)Y, true);
		}
		break;
		//Destroy
	case WM_DESTROY:
	{
		bool IsMainWnd;
		pWindow->getValue(LABEL_TO_ID(IsMainWindow), IsMainWnd);
		if (IsMainWnd)
		{
			if (KigsCore::GetCoreApplication())
				KigsCore::GetCoreApplication()->ExternAskExit();
			//else
			//	exit( 0 );
			return 0;
		}

		if (pWindow->myDestroyCallback != NULL)
			pWindow->myDestroyCallback(pWindow);
		break;
	}
	//Resize
	case WM_SIZE:
		/*WindowWin32::Resize(hWnd,LOWORD(lParam),HIWORD(lParam));		// LoWord=Width, HiWord=Height
		return 0;*/
		// move
	case WM_MOVE:
		/*POINT p;
		p.x = LOWORD(lParam);
		p.y = HIWORD(lParam);
		//WindowWin32::MoveWindow(hWnd,LOWORD(lParam),HIWORD(lParam));		// LoWord=Width, HiWord=Height
		ClientToScreen(hWnd, &p);
		WindowWin32::MoveWindow(hWnd, p.x, p.y);*/

		pWindow->setValue(LABEL_TO_ID(DirtySize),true);
		return 0;
	}

	if (myCallBackMap)
	{
		std::map<msgProcCallBack, msgProcCallBack>::iterator	itmap = (*myCallBackMap).begin();

		while (itmap != (*myCallBackMap).end())
		{
			LRESULT	result = (*itmap).second(hWnd, msg, wParam, lParam);
			++itmap;
		}
	}

	if (messageTreated)
	{
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//! constructor
WindowWin32::WindowWin32(const kstl::string& name, CLASS_NAME_TREE_ARG) : Window(name, PASS_CLASS_NAME_TREE_ARG)
{
	myScreenSaverActive = false;
	myDirtySize = false;
	myParent = NULL;
	myCursor = LoadCursor(NULL, IDC_ARROW);
}

//
void WindowWin32::SetParentWindow(HWND hParent)
{
	myParent = hParent;
}

void WindowWin32::SetCurrentCursor(LPCTSTR cursorName) {
	if (cursorName == 0)
		myCursor = NULL;
	else
		myCursor = LoadCursor(NULL, cursorName);
	SetCursor(myCursor);
}

//! destructor
WindowWin32::~WindowWin32()
{
	if (myFullScreen)								// Are We In Fullscreen Mode?
	{
		//! if fullscreen mode, switch back to init settings
		ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
	}

	//! destroy rendering screen
	if (myScreen)
		myScreen->Destroy();

	//! win32 stuff
	UnregisterClassA("Kigs Window", myWC.hInstance);

	//! enable again screen saver if needed

	if (myScreenSaverActive)
	{
		// enable here
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, 0);
	}

	SetTouchDisableProperty((HWND)myHandle, false);

	DestroyWindow((HWND)myHandle);
}

//! init the window, and if OK, show it
void WindowWin32::ProtectedInit()
{
	unsigned int i = myDisplayIndex;

	SP<DisplayDeviceCaps>	L_displaycaps = KigsCore::GetInstanceOf("getdisplaycaps", "DisplayDeviceCaps");
	auto device = L_displaycaps->Get_DisplayDevice(i);
	if (device == nullptr)
		device = L_displaycaps->Get_DisplayDevice(-1); // use main if not found

	DEVMODE devModeDesc;
	devModeDesc.dmSize = sizeof(DEVMODE);

	// get current settings
	EnumDisplaySettings(device->myName.c_str(), ENUM_CURRENT_SETTINGS, &devModeDesc);
	
	RECT WindowRect;

	DWORD		dwExStyle;
	DWORD		dwStyle;

	//! if fullscreen mode, try to have fullscreen settings with given sizex and sizey, and with given rendering screen bitsPerPixel
	if (myFullScreen)
	{
		SetCursor(NULL);
		u32 bpp;

		myScreen->getValue(LABEL_TO_ID(BitsPerPixel), bpp);

		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = mySizeX;
		dmScreenSettings.dmPelsHeight = mySizeY;

		dmScreenSettings.dmBitsPerPel = bpp;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//if (myDisplayIndex != 0)
		{

			// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
			if (ChangeDisplaySettingsEx(device->myName.c_str(),&dmScreenSettings, NULL, CDS_FULLSCREEN,NULL) != DISP_CHANGE_SUCCESSFUL)
			{
				myFullScreen = false;
			}
		}

#ifdef PSEUDO_FULLSCREEN
		dwExStyle = WS_EX_OVERLAPPEDWINDOW;
		dwStyle = WS_DLGFRAME;

		WindowRect.left = (long)devModeDesc.dmPosition.x;
		WindowRect.right = (long)devModeDesc.dmPelsWidth + (long)devModeDesc.dmPosition.x;
		WindowRect.top = (long)devModeDesc.dmPosition.y;
		WindowRect.bottom = (long)devModeDesc.dmPosition.y + (long)devModeDesc.dmPelsHeight;
#else
		dwExStyle=WS_EX_APPWINDOW|WS_EX_TOPMOST;
		dwStyle=WS_POPUP;

		WindowRect.left=0;
		WindowRect.right=(long)mySizeX;
		WindowRect.top=0;
		WindowRect.bottom = (long)mySizeY;
#endif
	}
	else
	{
		//! create windowed window (:o)
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		if (myIsMainWindow)
			dwStyle = WS_OVERLAPPEDWINDOW;
		else
			dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;	// Windows Style

		WindowRect.left = devModeDesc.dmPosition.x + (long)myPosX;
		WindowRect.top  = devModeDesc.dmPosition.y + (long)myPosY;
		WindowRect.right = WindowRect.left + (long)mySizeX;
#ifdef FRAPS_CAPTURE_MODE
		WindowRect.bottom = WindowRect.top + (long)mySizeY + 96;
#else
		WindowRect.bottom = WindowRect.top + (long)mySizeY;
#endif
	}

	//! win32 stuff
	WNDCLASSEX tmpwc = { sizeof(WNDCLASSEX), CS_OWNDC | CS_DBLCLKS, MsgProc, 0L, 0L,
		GetModuleHandleA(NULL), NULL, NULL, NULL, NULL,
		"Kigs Window", NULL };

	// Register the window class
	myWC = tmpwc;

	RegisterClassEx(&myWC);

	//! resize window to fit size
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	//! Create the window
	HWND hWnd = CreateWindowEx(dwExStyle, "Kigs Window", getName().c_str(),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WindowRect.left, WindowRect.top, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top,
		myParent, NULL, myWC.hInstance, (void*)this);

#ifdef PSEUDO_FULLSCREEN
	if (myFullScreen)
	{	// remove caption
		SetWindowLong(hWnd, GWL_STYLE, WS_BORDER);
	}
#endif
	myHandle = (void*)hWnd;



	//! init msg buffer
	ZeroMemory(&myMsg, sizeof(myMsg));

	if (m_bShowMouse == false)
	{
		SetCursorPos(mySizeX, mySizeY);
		volatile int tst = ShowCursor(0);
		while (tst > 0)
		{
			tst = ShowCursor(0);
		}
	}
	else
	{
		volatile int tst = ShowCursor(1);
		while (tst <= 0)
		{
			tst = ShowCursor(1);
		}
	}
#pragma message("-------------Note for the author : It is not good to implement ShowCursor into the Window class, as it is application wide")
	//! and show the window
	Show();

	// disable screen saver
	// first get current screensaver state

	SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &myScreenSaverActive, 0);
	if (myScreenSaverActive)
	{
		// disable here
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, 0);
	}

}

void	WindowWin32::GetMousePosInWindow(int posx, int posy, kfloat& wposx, kfloat& wposy)
{
	/*if(myScreen)
	{
	GetMousePosInDesignWindow(posx,posy,wposx,wposy);
	}
	else
	{*/
	posx -= (int)myPosX;
	//if(posx >= (int)mySizeX)
	//	posx = ((int)mySizeX)-1;
	//if(posx < 0)
	//	posx = 0;

	posy -= (int)myPosY;
	//if(posy>=(int)mySizeY)
	//	posy = ((int)mySizeY)-1;
	//if(posy < 0)
	//	posy = 0;

	wposx = (kfloat)posx;
	wposy = (kfloat)posy;
	//}
}

void	WindowWin32::GetMousePosInDesignWindow(int posx, int posy, kfloat& wposx, kfloat& wposy)
{
	posx -= (int)myPosX;
	//if(posx >= (int)mySizeX)
	//	posx = ((int)mySizeX)-1;
	//if(posx < 0)
	//	posx = 0;

	posy -= (int)myPosY;
	//if(posy>=(int)mySizeY)
	//	posy = ((int)mySizeY)-1;
	//if(posy < 0)
	//	posy = 0;

	if (myScreen)
	{
		myScreen->GetMousePosInDesignScreen(posx, posy, wposx, wposy);
	}
	else
	{
		wposx = (kfloat)posx;
		wposy = (kfloat)posy;
	}
}

/*void	WindowWin32::MoveWindow(HWND hWnd)
{
	//! as I don't know the window to move
	kstl::set<CoreModifiable*>	instances;
	kstl::set<CoreModifiable*>::iterator	it;
	CoreModifiable::GetInstances("Window", instances);

	WindowWin32* localthis = 0;

	for (it = instances.begin(); it != instances.end(); ++it)
	{
		localthis = (WindowWin32*)*it;
		//! if found window has the same handle, then this is the good one to resize
		if (localthis->myHandle == hWnd)
		{
			break;
		}
	}

	if (localthis)
	{
		RECT rect;
		GetWindowRect(hWnd, &rect);
		//printf("%s : %d %d :: %d %d\n", getName().c_str(), rect.top, rect.bottom, rect.left, rect.right);

		localthis->mySizeX = rect.right - rect.left;
		localthis->mySizeY = rect.bottom - rect.top;
		localthis->myPosX = rect.left;
		localthis->myPosY = rect.top;
	}
}*/

/*void	WindowWin32::Resize(HWND hWnd,int width,int height)
{
//! as I don't know the window to resize, search it
kstl::set<CoreModifiable*>	instances;
kstl::set<CoreModifiable*>::iterator	it;
WindowWin32::GetInstances("Window",instances);

WindowWin32* localthis=0;

for(it=instances.begin();it!=instances.end();++it)
{
localthis=(	WindowWin32* ) *it;
//! if found window has the same handle, then this is the good one to resize
if(localthis->myHandle == hWnd)
{
break;
}
}

if(localthis)
{
//! reset my parameters ans also ask screen to resize
localthis->mySizeX=width;
localthis->mySizeY=height;
if(localthis->myScreen)
localthis->myScreen->Resize((kfloat)width,(kfloat)height);
}
}*/

//! show window
void  WindowWin32::Show()
{
	//! init rendering screen after first show window

	//! win32 stuff
	HWND hWnd = (HWND)myHandle;

	SetTouchDisableProperty(hWnd, true);

#ifdef PSEUDO_FULLSCREEN
	if (myFullScreen)
	{
		// resize 
		RECT rcClient;
		GetWindowRect(GetDesktopWindow(), &rcClient);
		AdjustWindowRectEx(&rcClient, GetWindowLong(hWnd, GWL_STYLE), false, GetWindowLong(hWnd, GWL_EXSTYLE));
		SetWindowPos(hWnd, HWND_TOP, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, SWP_NOCOPYBITS);
	}
#endif

	// Show the window
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);

	if (myScreen && !myScreen->IsInit())
	{
		myScreen->setValue(LABEL_TO_ID(ParentWindowName), getName());
		myScreen->Init();
	}
}

//! update window
void  WindowWin32::Update(const Timer&  timer, void* addParam)
{
	if (myDirtySize)
	{
		POINT scrPos;
		scrPos.x = 0;
		scrPos.y = 0;
		ClientToScreen((HWND)myHandle, &scrPos);
		myPosX = scrPos.x;
		myPosY = scrPos.y;

		RECT rect;
		GetClientRect((HWND)myHandle, &rect);
		mySizeX = rect.right;
		mySizeY = rect.bottom;

		//printf("%s : %d %d :: %d %d\n", getName().c_str(), myPosX, myPosY, mySizeX, mySizeY);

		if (myScreen)
			myScreen->Resize((kfloat)mySizeX, (kfloat)mySizeY);

		myDirtySize = false;

	}

	//! call screen update
	if (myScreen)
		myScreen->CallUpdate(timer, addParam);

	//! manage messages
	if (PeekMessage(&myMsg, NULL, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(&myMsg);
		DispatchMessage(&myMsg);
	}
}

void WindowWin32::ShowMouse(bool bOn)
{
	if (m_bShowMouse != bOn) m_bShowMouse = bOn;

	if (m_bShowMouse == false)
	{
		//SetCursorPos(mySizeX, mySizeY);
		volatile int tst = ShowCursor(0);
		while (tst > 0)
		{
			tst = ShowCursor(0);
		}
	}
	else
	{
		volatile int tst = ShowCursor(1);
		while (tst <= 0)
		{
			tst = ShowCursor(1);
		}
	}
}

void WindowWin32::ShowBorder(bool show)
{
	HWND handle = (HWND)myHandle;
	auto style = GetWindowLongPtr(handle, GWL_STYLE);
	auto ex_style = GetWindowLongPtr(handle, GWL_EXSTYLE);
	auto to_change = WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU;
	auto to_change_ex = WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE;
	if (show)
	{
		style |= to_change;
		ex_style |= to_change_ex;
	}
	else
	{
		style &= ~(to_change);
		ex_style &= ~(to_change_ex);
	}
	
	SetWindowLongPtr(handle, GWL_STYLE, style);
	SetWindowLongPtr(handle, GWL_EXSTYLE, ex_style);
	SetWindowPos(handle, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

void WindowWin32::SetWindowPosition(v2i pos, v2i size, bool force_topmost)
{
	HWND handle = (HWND)myHandle;
	SetWindowPos(handle, HWND_TOPMOST, pos.x, pos.y, size.x, size.y, force_topmost ? 0 : SWP_NOZORDER | SWP_NOOWNERZORDER);
}