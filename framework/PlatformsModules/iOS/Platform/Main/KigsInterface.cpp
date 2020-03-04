#include "KigsInterface.h"

#include <KigsApplication.h>
#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "OpenGLRenderingScreen.h"

/**** accelerometre ****/
float	newAcc[3];
/***********************/

bool					userTouchEnabled=true;
CoreBaseApplication*	myApp=0;
volatile bool           mybUpdating=false;
volatile bool			myIsPaused = false;

#define ApplicationName(a) Stringify(a)
#define Stringify(a) #a

extern "C" int	AppInit()
{
	 //! First thing to do
    KigsCore::Init(false);

	// no need to register app to factory
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(KIGS_APPLICATION_CLASS, ApplicationName(KIGS_APPLICATION_CLASS));
	myApp = (CoreBaseApplication*)KIGS_APPLICATION_CLASS::CreateInstance(ApplicationName(KIGS_APPLICATION_CLASS));
			
    myApp->Init(BASE_DATA_PATH,true);
        
	mybUpdating = true;
	return true;
}
extern "C" int	AppUpdate()
{
	if(!mybUpdating)
        return true;
    mybUpdating = true;
    if(!myApp->NeedExit())
    {
        if(!myIsPaused)
            myApp->Update();
    }
    else
    {
        myApp->Close();
        delete myApp;
        KigsCore::Close();
        mybUpdating = false;
        exit(0);
    }
	return true;
}
extern "C" int	AppClose()
{
	myApp->Close();
    delete myApp;
    KigsCore::Close();
	return true;
}

extern "C" int AppSleep()
{
	if (!myIsPaused)
	{
		myApp->Sleep();
		myIsPaused = true;
	}
}

extern "C" int AppResume()
{
	if (myIsPaused)
	{
		myApp->Resume();
		myIsPaused = false;
	}
}