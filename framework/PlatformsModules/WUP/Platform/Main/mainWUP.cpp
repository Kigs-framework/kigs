// ******************************************************************************
// *   DO NOT MODIFY THIS FILE													*
// *   THIS FILE IS A GENERIC FILE COMMON TO SEVERAL APPLICATION                *
// ******************************************************************************
#pragma once
//! this file must define KIGS_APPLICATION_CLASS with the name of your application class
/*
#define BASE_APP_IMPORT

#define USE_D3D 1
#include "Platform/Main/BaseApp.h"

#ifdef BASE_APP_IMPORT
#define DECL_SPEC __declspec(dllimport)
#else
#define DECL_SPEC __declspec(dllexport)
#endif
*/
__declspec(dllimport) void wupmain();

// The main function creates an IFrameworkViewSource for our app, and runs the app.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	wupmain();
	return 0;
}

