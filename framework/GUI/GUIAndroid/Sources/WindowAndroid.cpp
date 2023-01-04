#include "PrecompiledHeaders.h"

#include "WindowAndroid.h"

#include "ModuleGUIAndroid.h"

#include "RenderingScreen.h"

#include "Core.h"

using namespace Kigs::Gui;

IMPLEMENT_CLASS_INFO(WindowAndroid)

//! constructor
WindowAndroid::WindowAndroid(const std::string& name,CLASS_NAME_TREE_ARG) : Window(name,PASS_CLASS_NAME_TREE_ARG)
{
   mScreen=0;
   // on android only one fullscreen window
   mPosition = v2f(0,0);
   
}

//! destructor
WindowAndroid::~WindowAndroid()
{  


}    

//! init the window, and if OK, show it
void WindowAndroid::ProtectedInit()
{
	Show();
}

//! show window
void  WindowAndroid::Show()
{
	if(mScreen && !mScreen->IsInit())
	{
		mScreen->setValue("ParentWindowName",getName());
		mScreen->Init();
	}
}    

//! update window
void  WindowAndroid::Update(const Timer&  timer, void* addParam)
{
	//! call screen update
	if(mScreen)
		mScreen->CallUpdate(timer,addParam);
}    


void	WindowAndroid::GetMousePosInWindow(int posx,int posy,kfloat& wposx,kfloat& wposy)
{
	posx -= (int)mPosition[0];
	posy -= (int)mPosition[1];
	wposx = (kfloat)posx;
	wposy = (kfloat)posy;
	return;

}

void	WindowAndroid::GetMousePosInDesignWindow(int posx,int posy,kfloat& wposx,kfloat& wposy)
{
	posx -= (int)mPosition[0];
	if(posx >= (int)mSize[0])
		posx = ((int)mSize[0])-1;
	if(posx < 0)
		posx = 0;

	posy -= (int)mPosition[1];
	if(posy>=(int)mSize[1])
		posy = ((int)mSize[1])-1;
	if(posy < 0)
		posy = 0;

	if(mScreen)
	{
		mScreen->as<Draw::RenderingScreen>()->GetMousePosInDesignScreen(posx, posy, wposx, wposy);
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
