// ******************************************************************************
// *   DO NOT MODIFY THIS FILE													*
// *   THIS FILE IS A GENERIC FILE COMMON TO SEVERAL APPLICATION                *
// ******************************************************************************

//! this file must define KIGS_APPLICATION_CLASS with the name of your application class

#include <windows.h>
#include <io.h>
#include <FCNTL.H>
#include <locale.h>
#include <csignal>

#include "KigsApplication.h"

//#define NO_CONSOLE

#define ApplicationName(a) Stringify(a)
#define Stringify(a) #a

volatile sig_atomic_t quit;

void signal_handler(int sig)
{
	signal(sig, signal_handler);
	quit = 1;
}

CoreBaseApplication*	myApp=0;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
#ifndef NO_CONSOLE
		AllocConsole();

		// init console
		int hCrt, i;
		FILE *hf;
		hCrt = _open_osfhandle((long) GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
		hf = _fdopen( hCrt, "w" );
		*stdout = *hf;
		i = setvbuf( stdout, NULL, _IONBF, 0 ); 
#endif
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
#ifndef NO_CONSOLE
		FreeConsole();
#endif
		break;
	}
	return TRUE;
}

extern "C" void __declspec(dllexport) InitKigs()
{
	setlocale(LC_NUMERIC,"C");	

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
#ifdef SIGBREAK
	signal(SIGBREAK, signal_handler);
#endif

		//! First thing to do
	KigsCore::Init();

	// no need to register app to factory
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(KIGS_APPLICATION_CLASS, ApplicationName(KIGS_APPLICATION_CLASS));
	myApp = (CoreBaseApplication*)KIGS_APPLICATION_CLASS::CreateInstance(ApplicationName(KIGS_APPLICATION_CLASS));


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

	KIGS_DUMP_MESSAGES


}


extern "C" void __declspec(dllexport) CloseKigs()
{
		//! close
	myApp->CloseApp();

	//! delete
	myApp->Destroy();

	//! last thing to do
	KigsCore::Close();
}


// return true if exit is asked
extern "C" bool __declspec(dllexport) UpdateKigs()
{

	if (!myApp->NeedExit() && !quit)
	{
		myApp->UpdateApp();
		return false;
	}

	return true;
}


