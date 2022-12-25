

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

using namespace Kigs::Gui;

IMPLEMENT_CLASS_INFO(WindowWin32)

std::map<msgProcCallBack, msgProcCallBack>*	WindowWin32::mCallBackMap = 0;

void	WindowWin32::AddCallback(msgProcCallBack callback)
{
	if (!mCallBackMap)
	{
		mCallBackMap = new std::map<msgProcCallBack, msgProcCallBack>;
	}

	(*mCallBackMap)[callback] = callback;
}

void	WindowWin32::RemoveCallback(msgProcCallBack callback)
{
	if (mCallBackMap)
	{
		std::map<msgProcCallBack, msgProcCallBack>::iterator	itmap = (*mCallBackMap).find(callback);

		if (itmap != (*mCallBackMap).end())
		{
			(*mCallBackMap).erase(itmap);
			if ((*mCallBackMap).size() == 0)
			{
				delete mCallBackMap;
				mCallBackMap = 0;
			}
		}
	}
}


const std::string& WindowWin32::GetClipboardText()
{
	if (OpenClipboard(nullptr))
	{
		HANDLE hData = GetClipboardData(CF_UNICODETEXT);
		wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData));
		mClipboard = to_utf8(pszText);
		GlobalUnlock(hData);
		CloseClipboard();
	}
	return mClipboard;
}

void WindowWin32::SetClipboardText(const std::string& txt)
{
	if (OpenClipboard(nullptr))
	{
		auto wtxt = to_wchar(txt);
		auto len = (wtxt.size() + 1) * sizeof(wchar_t);
		auto hMem = GlobalAlloc(GMEM_MOVEABLE, len);
		auto str_ptr = GlobalLock(hMem);
		memcpy(str_ptr, wtxt.data(), len);
		GlobalUnlock(hMem);
		EmptyClipboard();
		SetClipboardData(CF_UNICODETEXT, hMem);
		CloseClipboard();
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
	case WM_CHAR:
	{
		if (wParam > 0 && wParam < 0x10000)
		{
			auto utf16_char = (unsigned short)wParam;
			pWindow->EmitSignal("WM_CHAR", utf16_char);
		}	
		messageTreated = true;
	}
	break;
		// lose focus / reset click position
	case WM_KILLFOCUS:
	{
		bool IsMainWnd;
		pWindow->getValue("IsMainWindow", IsMainWnd);
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
		pWindow->getValue("IsMainWindow", IsMainWnd);
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
		SetCursor(pWindow->mCursor);
		break;

		//Character typed
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		if (pWindow->mKeyDownCallback)
		{
			unsigned int VirtualKeyCode, ScanCode;
			unsigned char KeyboardState[256];
			WORD ASCIIOutput;

			VirtualKeyCode = wParam;
			ScanCode = (lParam >> 16) & 0xFF;
			GetKeyboardState(KeyboardState);

			if (1 != ToAsciiEx(VirtualKeyCode, ScanCode, KeyboardState, &ASCIIOutput, 0, GetKeyboardLayout(GetWindowThreadProcessId(hWnd, NULL))))
				ASCIIOutput = 0;

			pWindow->mKeyDownCallback(pWindow, (char)ASCIIOutput, (int)VirtualKeyCode);
		}
		break;
	}

	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		if (pWindow->mKeyUpCallback)
		{
			unsigned int VirtualKeyCode, ScanCode;
			unsigned char KeyboardState[256];
			WORD ASCIIOutput;

			VirtualKeyCode = wParam;
			ScanCode = (lParam >> 16) & 0xFF;
			GetKeyboardState(KeyboardState);

			if (1 != ToAsciiEx(VirtualKeyCode, ScanCode, KeyboardState, &ASCIIOutput, 0, GetKeyboardLayout(GetWindowThreadProcessId(hWnd, NULL))))
				ASCIIOutput = 0;

			pWindow->mKeyUpCallback(pWindow, (char)ASCIIOutput, (int)VirtualKeyCode);
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
		if (pWindow->mClickCallback != NULL)
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

			float mX = (float)X / pWindow->mSize[0];
			float mY = (float)Y / pWindow->mSize[1];
			pWindow->mClickCallback(pWindow, buttonId, mX, mY, isDown);
			return 0;
		}
		break;
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
		if (pWindow->mDoubleClickCallback != NULL)
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

			pWindow->mDoubleClickCallback(pWindow, buttonId, (float)X, (float)Y, true);
		}
		break;
		//Destroy
	case WM_CLOSE:
		bool IsMainWnd;
		pWindow->getValue("IsMainWindow", IsMainWnd);
		if (IsMainWnd)
		{
			if (KigsCore::GetCoreApplication())
			{
				KigsCore::GetCoreApplication()->ExternAskExit();
				if(KigsCore::GetCoreApplication()->NeedExit())
					DestroyWindow(hWnd);
				return 0;
			}
		}
		break;
	case WM_DESTROY:
	{
		bool IsMainWnd;
		pWindow->getValue("IsMainWindow", IsMainWnd);
		if (IsMainWnd)
		{
			return 0;
		}
		if (pWindow->mDestroyCallback != NULL)
			pWindow->mDestroyCallback(pWindow);
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

		pWindow->setValue("DirtySize",true);
		return 0;
	}

	if (mCallBackMap)
	{
		std::map<msgProcCallBack, msgProcCallBack>::iterator	itmap = (*mCallBackMap).begin();

		while (itmap != (*mCallBackMap).end())
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
WindowWin32::WindowWin32(const std::string& name, CLASS_NAME_TREE_ARG) : Window(name, PASS_CLASS_NAME_TREE_ARG)
{
	mScreenSaverActive = false;
	mDirtySize = false;
	mParent = NULL;
	mCursor = LoadCursor(NULL, IDC_ARROW);
}

//
void WindowWin32::SetParentWindow(HWND hParent)
{
	mParent = hParent;
}

void WindowWin32::SetCurrentCursor(const char* cursorName)
{
	if (!cursorName)
		mCursor = NULL;
	else
		mCursor = LoadCursor(NULL, cursorName);
	SetCursor(mCursor);
}

//! destructor
WindowWin32::~WindowWin32()
{
	if (mFullScreen)								// Are We In Fullscreen Mode?
	{
		//! if fullscreen mode, switch back to init settings
		ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
	}
	//! win32 stuff
	UnregisterClassA("Kigs Window", mWC.hInstance);

	//! enable again screen saver if needed

	if (mScreenSaverActive)
	{
		// enable here
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, 0);
	}

	SetTouchDisableProperty((HWND)mHandle, false);

	DestroyWindow((HWND)mHandle);
}

//! init the window, and if OK, show it
void WindowWin32::ProtectedInit()
{
	unsigned int i = mDisplayIndex;

	SP<DisplayDeviceCaps>	L_displaycaps = KigsCore::GetInstanceOf("getdisplaycaps", "DisplayDeviceCaps");
	auto device = L_displaycaps->Get_DisplayDevice(i);
	if (device == nullptr)
		device = L_displaycaps->Get_DisplayDevice(-1); // use main if not found

	DEVMODE devModeDesc;
	devModeDesc.dmSize = sizeof(DEVMODE);

	// get current settings
	EnumDisplaySettings(device->mName.c_str(), ENUM_CURRENT_SETTINGS, &devModeDesc);
	
	RECT WindowRect;

	DWORD		dwExStyle;
	DWORD		dwStyle;

	//! if fullscreen mode, try to have fullscreen settings with given sizex and sizey, and with given rendering screen bitsPerPixel
	if (mFullScreen)
	{
		SetCursor(NULL);
		u32 bpp;

		mScreen->getValue("BitsPerPixel", bpp);

		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = mSize[0];
		dmScreenSettings.dmPelsHeight = mSize[1];

		dmScreenSettings.dmBitsPerPel = bpp;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//if (myDisplayIndex != 0)
		{

			// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
			if (ChangeDisplaySettingsEx(device->mName.c_str(),&dmScreenSettings, NULL, CDS_FULLSCREEN,NULL) != DISP_CHANGE_SUCCESSFUL)
			{
				mFullScreen = false;
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
		WindowRect.right=(long)mSize[0];
		WindowRect.top=0;
		WindowRect.bottom = (long)mSize[1];
#endif
	}
	else
	{
		//! create windowed window (:o)
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		if (mIsMainWindow)
			dwStyle = WS_OVERLAPPEDWINDOW;
		else
			dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;	// Windows Style

		WindowRect.left = devModeDesc.dmPosition.x + (long)mPosition[0];
		WindowRect.top  = devModeDesc.dmPosition.y + (long)mPosition[1];
		WindowRect.right = WindowRect.left + (long)mSize[0];
#ifdef FRAPS_CAPTURE_MODE
		WindowRect.bottom = WindowRect.top + (long)mSize[1] + 96;
#else
		WindowRect.bottom = WindowRect.top + (long)mSize[1];
#endif
	}
	
	//! win32 stuff
	WNDCLASSEX tmpwc = { sizeof(WNDCLASSEX), CS_OWNDC | CS_DBLCLKS, MsgProc, 0L, 0L,
		GetModuleHandleA(NULL), NULL, NULL, NULL, NULL,
		"Kigs Window", NULL };
	
	tmpwc.hIcon = LoadIcon(GetModuleHandleA(NULL), "ICON_KIGS_APP");

	// Register the window class
	mWC = tmpwc;

	RegisterClassEx(&mWC);

	//! resize window to fit size
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	//! Create the window
	HWND hWnd = CreateWindowEx(dwExStyle, "Kigs Window", getName().c_str(),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WindowRect.left, WindowRect.top, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top,
		mParent, NULL, mWC.hInstance, (void*)this);

#ifdef PSEUDO_FULLSCREEN
	if (mFullScreen)
	{	// remove caption
		SetWindowLong(hWnd, GWL_STYLE, WS_BORDER);
	}
#endif
	mHandle = (void*)hWnd;



	//! init msg buffer
	ZeroMemory(&mMsg, sizeof(mMsg));

	if (mShowMouseCursor == false)
	{
		SetCursorPos(mSize[0], mSize[1]);
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

	SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &mScreenSaverActive, 0);
	if (mScreenSaverActive)
	{
		// disable here
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, 0);
	}

}

void	WindowWin32::GetMousePosInWindow(int posx, int posy, float& wposx, float& wposy)
{
	
	posx -= (int)mPosition[0];
	posy -= (int)mPosition[1];
	

	wposx = (float)posx;
	wposy = (float)posy;

}

void	WindowWin32::GetMousePosInDesignWindow(int posx, int posy, float& wposx, float& wposy)
{
	posx -= (int)mPosition[0];
	posy -= (int)mPosition[1];


	if (mScreen)
	{
		mScreen->as<RenderingScreen>()->GetMousePosInDesignScreen(posx, posy, wposx, wposy);
	}
	else
	{
		wposx = (float)posx;
		wposy = (float)posy;
	}
}


//! show window
void  WindowWin32::Show()
{
	//! init rendering screen after first show window

	//! win32 stuff
	HWND hWnd = (HWND)mHandle;

	SetTouchDisableProperty(hWnd, true);

#ifdef PSEUDO_FULLSCREEN
	if (mFullScreen)
	{
		// resize 
		RECT rcClient;
		GetWindowRect(GetDesktopWindow(), &rcClient);
		AdjustWindowRectEx(&rcClient, GetWindowLong(hWnd, GWL_STYLE), false, GetWindowLong(hWnd, GWL_EXSTYLE));
		SetWindowPos(hWnd, HWND_TOP, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, SWP_NOCOPYBITS);
	}
#endif
	if (mShow)
	{
		// Show the window
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);
	}
	if (mScreen && !mScreen->IsInit())
	{
		mScreen->setValue("ParentWindowName", getName());
		mScreen->Init();
	}
}

//! update window
void  WindowWin32::Update(const Timer&  timer, void* addParam)
{
	if (mDirtySize)
	{
		POINT scrPos;
		scrPos.x = 0;
		scrPos.y = 0;
		ClientToScreen((HWND)mHandle, &scrPos);
		mPosition = v2f(scrPos.x,scrPos.y);

		RECT rect;
		GetClientRect((HWND)mHandle, &rect);
		mSize = v2f(rect.right,rect.bottom);

		//printf("%s : %d %d :: %d %d\n", getName().c_str(), mPositionX, mPositionY, mySizeX, mySizeY);

		if (mScreen)
			mScreen->as<RenderingScreen>()->Resize((float)mSize[0], (float)mSize[1]);

		mDirtySize = false;

	}

	//! call screen update
	if (mScreen)
		mScreen->CallUpdate(timer, addParam);

	//! manage messages
	if (PeekMessage(&mMsg, NULL, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(&mMsg);
		DispatchMessage(&mMsg);
	}
}

void WindowWin32::ShowMouse(bool bOn)
{
	if (mShowMouseCursor != bOn) mShowMouseCursor = bOn;

	if (mShowMouseCursor == false)
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
	HWND handle = (HWND)mHandle;
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
	HWND handle = (HWND)mHandle;
	SetWindowPos(handle, HWND_TOPMOST, pos.x, pos.y, size.x, size.y, force_topmost ? 0 : SWP_NOZORDER | SWP_NOOWNERZORDER);
}