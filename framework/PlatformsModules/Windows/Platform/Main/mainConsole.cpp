// ******************************************************************************
// *   DO NOT MODIFY THIS FILE													*
// *   THIS FILE IS A GENERIC FILE COMMON TO SEVERAL APPLICATION                *
// ******************************************************************************

//! this file must define KIGS_APPLICATION_CLASS with the name of your application class
#include "KigsApplication.h"

#include <Core.h>
#include "Timer.h"

#include <locale.h>
#include <csignal>
#include <windows.h>

#ifdef KIGS_TOOLS
#include <stdlib.h>
#include <crtdbg.h>
#endif

#define ApplicationName(a) Stringify(a)
#define Stringify(a) #a

volatile sig_atomic_t quit;

void signal_handler(int sig)
{
	signal(sig, signal_handler);
	quit = 1;
}

int main(int argc, char *argv[])
{
	
#ifdef KIGS_TOOLS
	// print leaks after program exits
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// use this to add a breakpoint on a given alloc number 
//	_CrtSetBreakAlloc(1262);
#endif
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
	CoreBaseApplication*	myApp = (CoreBaseApplication*)KIGS_APPLICATION_CLASS::CreateInstance(ApplicationName(KIGS_APPLICATION_CLASS));

	int i;
	for(i=0;i<argc;i++)
	{
		myApp->PushArg(argv[i]);
	}


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
	while(!myApp->NeedExit() && !quit)
	{
		myApp->UpdateApp();
	}

	//! close
	myApp->CloseApp();

	//! delete
	myApp->Destroy();

	//! last thing to do
	KigsCore::Close();

	return 0;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	setlocale(LC_NUMERIC, "C");

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
#ifdef SIGBREAK
	signal(SIGBREAK, signal_handler);
#endif

	//! First thing to do
	KigsCore::Init();

	// no need to register app to factory
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(KIGS_APPLICATION_CLASS, ApplicationName(KIGS_APPLICATION_CLASS));
	CoreBaseApplication*	myApp = (CoreBaseApplication*)KIGS_APPLICATION_CLASS::CreateInstance(ApplicationName(KIGS_APPLICATION_CLASS));
	   
#ifdef INIT_DEFAULT_MODULES
#ifdef BASE_DATA_PATH
	//! then init
	myApp->InitApp(BASE_DATA_PATH, true);
#else
	//! then init
	myApp->InitApp(0, true);
#endif //BASE_DATA_PATH

#else
	//! then init
	myApp->InitApp(0, false);

#endif //INIT_DEFAULT_MODULES

	//! update
	while (!myApp->NeedExit() && !quit)
	{
		myApp->UpdateApp();
	}

	//! close
	myApp->CloseApp();

	//! delete
	myApp->Destroy();

	//! last thing to do
	KigsCore::Close();
	return 0;
}

