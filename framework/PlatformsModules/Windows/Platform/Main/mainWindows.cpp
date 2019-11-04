// ******************************************************************************
// *   DO NOT MODIFY THIS FILE													*
// *   THIS FILE IS A GENERIC FILE COMMON TO SEVERAL APPLICATION                *
// ******************************************************************************


//! this file must define KIGS_APPLICATION_CLASS with the name of your application class
#include "KigsApplication.h"
#include <locale.h>

#include <windows.h>

#define ApplicationName(a) Stringify(a)
#define Stringify(a) #a


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	setlocale(LC_NUMERIC,"C");	

	//! First thing to do
	Core::Init();
	// no need to register app to factory
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(KIGS_APPLICATION_CLASS, ApplicationName(KIGS_APPLICATION_CLASS));
	CoreBaseApplication*	myApp = (CoreBaseApplication*)KIGS_APPLICATION_CLASS::CreateInstance(ApplicationName(KIGS_APPLICATION_CLASS));


#ifdef INIT_DEFAULT_MODULES
#ifdef BASE_DATA_PATH
		//! then init
		myApp->InitApp(BASE_DATA_PATH,true);
#else
		//! then init
		myApp->InitApp(0,true);
#endif //BASE_DATA_PATH

#else
		//! then init
		myApp->InitApp(0,false);

#endif //INIT_DEFAULT_MODULES

	//! update
	while(!myApp->NeedExit())
	{
		myApp->UpdateApp();
	}

	//! close
	myApp->CloseApp();

	//! delete
	delete myApp;

	//! last thing to do
	Core::Close();
	return 0;
}

