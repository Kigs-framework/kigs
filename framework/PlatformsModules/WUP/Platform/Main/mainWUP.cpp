// ******************************************************************************
// *   DO NOT MODIFY THIS FILE													*
// *   THIS FILE IS A GENERIC FILE COMMON TO SEVERAL APPLICATION                *
// ******************************************************************************
#pragma once

__declspec(dllimport) void wupmain();

#include <Windows.h>

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	wupmain();
	return 0;
}

