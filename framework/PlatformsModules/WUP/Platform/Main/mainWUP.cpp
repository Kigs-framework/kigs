// ******************************************************************************
// *   DO NOT MODIFY THIS FILE													*
// *   THIS FILE IS A GENERIC FILE COMMON TO SEVERAL APPLICATION                *
// ******************************************************************************
#pragma once
#include "KigsApplication.h"
#ifdef UWP_STATIC_LIB
void wupmain();
#else
__declspec(dllimport) void wupmain();
#endif
#include <Windows.h>

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	wupmain();
	return 0;
}

