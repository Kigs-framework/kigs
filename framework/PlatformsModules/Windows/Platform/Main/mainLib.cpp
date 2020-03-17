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

namespace Kigs
{

#define ApplicationName(a) Stringify(a)
#define Stringify(a) #a

	volatile sig_atomic_t quit=0;

	void signal_handler(int sig)
	{
		signal(sig, signal_handler);
		quit = 1;
	}

	CoreBaseApplication*	myApp = 0;

	bool IsKigsInit = false;

	void InitKigs()
	{
		setlocale(LC_NUMERIC, "C");

		signal(SIGINT, signal_handler);
		signal(SIGTERM, signal_handler);
#ifdef SIGBREAK
		signal(SIGBREAK, signal_handler);
#endif

		//! First thing to do
		KigsCore::Init(false);

		// no need to register app to factory
		DECLARE_CLASS_INFO_WITHOUT_FACTORY(KIGS_APPLICATION_CLASS, ApplicationName(KIGS_APPLICATION_CLASS));
		myApp = (CoreBaseApplication*)KIGS_APPLICATION_CLASS::CreateInstance(ApplicationName(KIGS_APPLICATION_CLASS));


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

		KIGS_DUMP_MESSAGES

			IsKigsInit = true;
	}


	void CloseKigs()
	{
		//! close
		myApp->CloseApp();

		//! delete
		myApp->Destroy();

		//! last thing to do
		KigsCore::Close();
	}


	// return true if exit is asked
	bool UpdateKigs()
	{
		if (myApp == 0)
			return false;

		if (!myApp->NeedExit() && !quit)
		{
			myApp->UpdateApp();
			return false;
		}

		return true;
	}

};
