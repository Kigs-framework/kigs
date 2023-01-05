// ******************************************************************************
// *   DO NOT MODIFY THIS FILE													*
// *   THIS FILE IS A GENERIC FILE COMMON TO SEVERAL APPLICATION                *
// ******************************************************************************


//! this file must define KIGS_APPLICATION_CLASS with the name of your application class
#include "KigsApplication.h"
#include "emscripten.h"
#include "Core.h"

#define ApplicationName(a) Stringify(a)
#define Stringify(a) #a

using namespace Kigs;
using namespace Kigs::Core;

SP<CoreBaseApplication>	myApp=nullptr;

void	MainLoop()
{
	myApp->UpdateApp();
	if(myApp->NeedExit())
	{

		//! close
		myApp->CloseApp();

		//! delete
		myApp = nullptr;

		//! last thing to do
		KigsCore::Close();		
		
		emscripten_cancel_main_loop();
	}
}

int main( int argc, const char* argv[] )
//int mainJavascript()
{
	//! First thing to do
	KigsCore::Init();
	// no need to register app to factory
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(KIGS_APPLICATION_CLASS, ApplicationName(KIGS_APPLICATION_CLASS));
	myApp = KIGS_APPLICATION_CLASS::CreateInstance(ApplicationName(KIGS_APPLICATION_CLASS));

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
	emscripten_set_main_loop(MainLoop,0,1);

}



