#pragma once

#include "Window.h"
#include <d3d9.h>

#define PSEUDO_FULLSCREEN

namespace Kigs
{
	namespace Gui
	{

		typedef LRESULT(*msgProcCallBack)(HWND hWnd, ::UINT msg, WPARAM wParam, LPARAM lParam);
		// ****************************************
		// * WindowWin32 class
		// * --------------------------------------
		/**
		* \file	WindowWin32.h
		* \class	WindowWin32
		* \ingroup GUIModule
		* \brief	Specific Win32 Window.
		*/
		// ****************************************
		class WindowWin32 : public Window
		{
		public:
			DECLARE_CLASS_INFO(WindowWin32, Window, GUI)

				//! constructor
				WindowWin32(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			//! destructor
			virtual ~WindowWin32();

			//! show method, display the window
			void  Show();
			//! update method, call rendering screen update and manage messages
			virtual void Update(const Time::Timer& timer, void* /*addParam*/);
			//! show or hide system mouse
			void ShowMouse(bool bOn);

			//! callback called when window is resized
			//static void	Resize(HWND hWnd, int width,int height);
			//static void	MoveWindow(HWND hWnd);

			//! Set parent Window for creation
			virtual void SetParentWindow(HWND hParent);

			virtual void	GetMousePosInWindow(int posx, int posy, float& wposx, float& wposy);
			virtual void	GetMousePosInDesignWindow(int posx, int posy, float& wposx, float& wposy);

			static void	AddCallback(msgProcCallBack callback);
			static void	RemoveCallback(msgProcCallBack callback);

			virtual void ChangeWindowText(const char* txt)
			{
				SetWindowTextA((HWND)mHandle, txt);
			}

			void SetCurrentCursor(const char* cursorName) override;

			void ShowBorder(bool show) override;

			void SetWindowPosition(v2i pos, v2i size, bool force_topmost = false) override;

			bool IsPlatformClipboardSupported() override { return true; }
			const std::string& GetClipboardText() override;
			void SetClipboardText(const std::string& txt) override;

		protected:
			//! os dependant Window creation, or fullscreen mode setup
			virtual void  ProtectedInit();

			//! message management callback
			static LRESULT WINAPI MsgProc(HWND hWnd, ::UINT msg, WPARAM wParam, LPARAM lParam);

			//! win32 handle
			WNDCLASSEX     mWC;

			//!Parent window at creation
			HWND mParent;


			//! stored msg
			MSG            mMsg;

			//! manage screensaver desactivation
			bool		   mScreenSaverActive;

			HCURSOR		   mCursor;

			static std::map<msgProcCallBack, msgProcCallBack>* mCallBackMap;
		};

	}
}
