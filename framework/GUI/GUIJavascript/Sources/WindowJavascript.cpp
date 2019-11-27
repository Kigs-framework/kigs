#include "WindowJavascript.h"
//#include "RendererIncludes.h"
#include "RenderingScreen.h"
#include "ModuleGUIJavascript.h"
#include <stdio.h>
#include <stdlib.h>

#include "Core.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include "CoreModifiable.h"

IMPLEMENT_CLASS_INFO(WindowJavascript)

extern "C" int	GetCanvasPositionX(const char* window);
extern "C" int	GetCanvasPositionY(const char* window);
extern "C" void	SetCanvasPos(const char* window,int posX,int posY);
extern "C" void	SetCanvasSize(const char* window, int sx, int sy);
extern "C" int	GetHTMLBodySizeX();
extern "C" int	GetHTMLBodySizeY();
extern "C" void CreateCanvas(const char* window, int isfirst);

WindowJavascript*	WindowJavascript::myFirstWindow=0;

//! constructor
WindowJavascript::WindowJavascript(const kstl::string& name,CLASS_NAME_TREE_ARG) : 
Window(name,PASS_CLASS_NAME_TREE_ARG)
{
   myScreen=0;
   if (myFirstWindow == 0)
   {
	   myFirstWindow = this;
   }
}

//! destructor
WindowJavascript::~WindowJavascript()
{  
	//! destroy rendering screen
	if(myScreen)
		myScreen->Destroy();

}    

//! init the window, and if OK, show it
void WindowJavascript::ProtectedInit()
{
	if (myFirstWindow == this)
	{
		CreateCanvas(getName().c_str(), 1);
	}
	else
	{
		CreateCanvas(getName().c_str(), 0);
	}

	printf("window pos : %d %d \n", myPosX.const_ref(), myPosY.const_ref());

	if (myFullScreen)
	{
		Point2DI screenSize(GetHTMLBodySizeX(), GetHTMLBodySizeY());
		SetCanvasSize(getName().c_str(),screenSize.x, screenSize.y);
		mySizeX = screenSize.x;
		mySizeY = screenSize.y;
		SetCanvasPos(getName().c_str(),0, 0);
		myPosX = 0;
		myPosY = 0;
	}
	else
	{
		SetCanvasSize(getName().c_str(),mySizeX, mySizeY);
		SetCanvasPos(getName().c_str(),myPosX, myPosY);
	}

	Show();
}

//! show window
void  WindowJavascript::Show()
{
	if(myScreen && !myScreen->IsInit())
	{
		myScreen->setValue(LABEL_TO_ID(ParentWindowName),getName());
		myScreen->Init();
	}

}    

//! update window
void  WindowJavascript::Update(const Timer&  timer, void* addParam)
{
	//! call screen update
	if(myScreen)
	{
		// check if size has changed
		if (myFullScreen)
		{
			Point2DI screenSize(GetHTMLBodySizeX(), GetHTMLBodySizeY());

			if ((screenSize.x != mySizeX) || (screenSize.y != mySizeY))
			{
				mySizeX = screenSize.x;
				mySizeY = screenSize.y;
				SetCanvasSize(getName().c_str(),screenSize.x, screenSize.y);
				myScreen->Resize((int)mySizeX, (int)mySizeY);
			}
		}

		myPosX = GetCanvasPositionX(getName().c_str());
		myPosY = GetCanvasPositionY(getName().c_str());
		myScreen->CallUpdate(timer, addParam);
	}

   Window::Update(timer, addParam);
}    


void	WindowJavascript::GetMousePosInWindow(int posx,int posy,kfloat& wposx,kfloat& wposy)
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

	/*if(myScreen)
	{
		myScreen->GetMousePosInScreen(posx,posy,wposx,wposy);
	}
	else
	{*/
		wposx=(kfloat)posx;
		wposy=(kfloat)posy;
//	}

}

void	WindowJavascript::GetMousePosInDesignWindow(int posx,int posy,kfloat& wposx,kfloat& wposy)
{
	posx -= (int)myPosX;
	/*if(posx >= (int)mySizeX)
		posx = ((int)mySizeX)-1;
	if(posx < 0)
		posx = 0;*/

	posy -= (int)myPosY;
	/*if(posy>=(int)mySizeY)
		posy = ((int)mySizeY)-1;
	if(posy < 0)
		posy = 0;*/

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


void WindowJavascript::ShowMouse(bool bOn)
{
	
}
