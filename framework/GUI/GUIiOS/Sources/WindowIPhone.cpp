#include "PrecompiledHeaders.h"

#include "WindowIPhone.h"
#include "RendererIncludes.h"
#include "ModuleGUIIphone.h"


#include "Core.h"

IMPLEMENT_CLASS_INFO(WindowIPhone)

//! constructor
WindowIPhone::WindowIPhone(const kstl::string& name,CLASS_NAME_TREE_ARG) : Window(name,PASS_CLASS_NAME_TREE_ARG)
{
   myScreen=0;
}

//! destructor
WindowIPhone::~WindowIPhone()
{  
	//! destroy rendering screen
	if(myScreen)
		myScreen->Destroy();

}    

//! init the window, and if OK, show it
void WindowIPhone::ProtectedInit()
{
	
	
	Show();
}

//! show window
void  WindowIPhone::Show()
{
	if(myScreen && !myScreen->IsInit())
	{
		myScreen->setValue(_S_2_ID("ParentWindowName"),getName());
		myScreen->Init();
	}

}    

//! update window
void  WindowIPhone::Update(const Timer&  timer, void* addParam)
{
	//! call screen update
	if(myScreen)
		myScreen->Update(timer, addParam);

   Window::Update(timer, addParam);
}    

void WindowIPhone::ShowMouse(bool bOn)
{
	
}

void WindowIPhone::GetMousePosInWindow(int posx, int posy, kfloat &wposx, kfloat &wposy)
{
    posx -=(int)myPosX;
    if(posx >= (int)mySizeX)
        posx=((int)mySizeX)-1;
    if(posx<0)
        posx = 0;
    
    posy-=(int)myPosY;
    if(posy>=(int)mySizeY)
        posy=((int)mySizeY)-1;
    if(posy<0)
        posy=0;
    
    if(myScreen)
        myScreen->GetMousePosInScreen(posx,posy,wposx,wposy);
    else
    {
        wposx=(kfloat)posx;
        wposy=(kfloat)posy;
    }
}

void WindowIPhone::GetMousePosInDesignWindow(int posx, int posy, kfloat &wposx, kfloat &wposy)
{
    posx -=(int)myPosX;
    if(posx >= (int)mySizeX)
        posx=((int)mySizeX)-1;
    if(posx<0)
        posx = 0;
    
    posy-=(int)myPosY;
    if(posy>=(int)mySizeY)
        posy=((int)mySizeY)-1;
    if(posy<0)
        posy=0;
    
    if(myScreen)
        myScreen->GetMousePosInDesignScreen(posx,posy,wposx,wposy);
    else
    {
        wposx=(kfloat)posx;
        wposy=(kfloat)posy;
    }
}