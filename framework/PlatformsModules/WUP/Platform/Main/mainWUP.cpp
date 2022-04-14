// ******************************************************************************
// *   DO NOT MODIFY THIS FILE													*
// *   THIS FILE IS A GENERIC FILE COMMON TO SEVERAL APPLICATION                *
// ******************************************************************************
#pragma once
#define NEXTBIM_EXPLORER_STATIC
#ifdef NEXTBIM_EXPLORER_STATIC
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

