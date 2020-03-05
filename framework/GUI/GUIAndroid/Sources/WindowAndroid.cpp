#include "PrecompiledHeaders.h"

#include "WindowAndroid.h"

#include "ModuleGUIAndroid.h"

#include "RenderingScreen.h"

#include "Core.h"

IMPLEMENT_CLASS_INFO(WindowAndroid)

//! constructor
WindowAndroid::WindowAndroid(const kstl::string& name,CLASS_NAME_TREE_ARG) : Window(name,PASS_CLASS_NAME_TREE_ARG)
{
   myScreen=0;
   // on android only one fullscreen window
   myPosX = 0;
   myPosY = 0;
   
}

//! destructor
WindowAndroid::~WindowAndroid()
{  
	//! destroy rendering screen
	if(myScreen)
		myScreen->Destroy();

}    

//! init the window, and if OK, show it
void WindowAndroid::ProtectedInit()
{
	Show();
}

//! show window
void  WindowAndroid::Show()
{
	if(myScreen && !myScreen->IsInit())
	{
		myScreen->setValue("ParentWindowName",getName());
		myScreen->Init();
	}
}    

//! update window
void  WindowAndroid::Update(const Timer&  timer, void* addParam)
{
	//! call screen update
	if(myScreen)
		myScreen->CallUpdate(timer,addParam);
}    


void	WindowAndroid::GetMousePosInWindow(int posx,int posy,kfloat& wposx,kfloat& wposy)
{
	posx -= (int)myPosX;
	posy -= (int)myPosY;
	wposx = (kfloat)posx;
	wposy = (kfloat)posy;
	return;

	posx -= (int)myPosX;
	if(posx >= (int)mySizeX)
		posx = ((int)mySizeX)-1;
	if(posx < 0)
		posx = 0;

	posy -= (int)myPosY;
	if(posy>=(int)mySizeY)
		posy = ((int)mySizeY)-1;
	if(posy < 0)
		posy = 0;

	if(myScreen)
	{
		myScreen->GetMousePosInScreen(posx,posy,wposx,wposy);
	}
	else
	{
		wposx=(kfloat)posx;
		wposy=(kfloat)posy;
	}
}

void	WindowAndroid::GetMousePosInDesignWindow(int posx,int posy,kfloat& wposx,kfloat& wposy)
{
	posx -= (int)myPosX;
	if(posx >= (int)mySizeX)
		posx = ((int)mySizeX)-1;
	if(posx < 0)
		posx = 0;

	posy -= (int)myPosY;
	if(posy>=(int)mySizeY)
		posy = ((int)mySizeY)-1;
	if(posy < 0)
		posy = 0;

	if(myScreen)
	{
		myScreen->GetMousePosInDesignScreen(posx,posy,wposx,wposy);
	}
	else
	{
		wposx=(kfloat)posx;
		wposy=(kfloat)posy;
	}
}

void WindowAndroid::ShowMouse(bool bOn)
{
}
