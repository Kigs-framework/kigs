#include "WindowJavascript.h"
#include "RenderingScreen.h"
#include "ModuleGUIJavascript.h"
#include <stdio.h>
#include <stdlib.h>

#include "Core.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include "CoreModifiable.h"

using namespace Kigs::Gui;

IMPLEMENT_CLASS_INFO(WindowJavascript)

extern "C" int	GetCanvasPositionX(const char* window);
extern "C" int	GetCanvasPositionY(const char* window);
extern "C" void	SetCanvasPos(const char* window,int posX,int posY);
extern "C" void	SetCanvasSize(const char* window, int sx, int sy);
extern "C" int	GetHTMLBodySizeX();
extern "C" int	GetHTMLBodySizeY();
extern "C" void CreateCanvas(const char* window, int isfirst);
extern "C" void SetAbsolutePos(const char* window,int abs);

WindowJavascript*	WindowJavascript::mFirstWindow=0;

//! constructor
WindowJavascript::WindowJavascript(const std::string& name,CLASS_NAME_TREE_ARG) : 
Window(name,PASS_CLASS_NAME_TREE_ARG)
{
   mScreen=0;
   if (mFirstWindow == 0)
   {
	   mFirstWindow = this;
   }
}

//! destructor
WindowJavascript::~WindowJavascript()
{  
	//! destroy rendering screen
	
	mScreen = nullptr;

}    

//! init the window, and if OK, show it
void WindowJavascript::ProtectedInit()
{
	
	if (mFirstWindow == this)
	{
		CreateCanvas(getName().c_str(), 1);
	}
	else
	{
		CreateCanvas(getName().c_str(), 0);
	}

	SetAbsolutePos(getName().c_str(), mAbsolutePos?1:0);

	printf("window pos : %d %d \n", (int)mPosition[0], (int)mPosition[1]);

	if (mFullScreen)
	{
		Point2DI screenSize(GetHTMLBodySizeX(), GetHTMLBodySizeY());
		SetCanvasSize(getName().c_str(),screenSize.x, screenSize.y);
		mSize[0] = screenSize.x;
		mSize[1] = screenSize.y;
		SetCanvasPos(getName().c_str(),0, 0);
		mPosition = v2f(0,0);
	}
	else
	{
		SetCanvasSize(getName().c_str(),mSize[0], mSize[1]);
		SetCanvasPos(getName().c_str(),mPosition[0], mPosition[1]);
	}

	Show();
}

//! show window
void  WindowJavascript::Show()
{
	if(mScreen && !mScreen->IsInit())
	{
		mScreen->setValue("ParentWindowName",getName());
		mScreen->Init();
	}

}    

//! update window
void  WindowJavascript::Update(const Time::Timer&  timer, void* addParam)
{
	//! call screen update
	if(mScreen)
	{
		// check if size has changed
		if (mFullScreen)
		{
			Point2DI screenSize(GetHTMLBodySizeX(), GetHTMLBodySizeY());

			if ((screenSize.x != mSize[0]) || (screenSize.y != mSize[1]))
			{
				mSize[0] = screenSize.x;
				mSize[1] = screenSize.y;
				SetCanvasSize(getName().c_str(),screenSize.x, screenSize.y);
				mScreen->as<Draw::RenderingScreen>()->Resize((int)mSize[0], (int)mSize[1]);
			}
		}

		mPosition[0] = GetCanvasPositionX(getName().c_str());
		mPosition[1] = GetCanvasPositionY(getName().c_str());
		mScreen->CallUpdate(timer, addParam);
	}

   Window::Update(timer, addParam);
}    


void	WindowJavascript::GetMousePosInWindow(int posx,int posy,float& wposx,float& wposy)
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

	/*if(mScreen)
	{
		mScreen->GetMousePosInScreen(posx,posy,wposx,wposy);
	}
	else
	{*/
		wposx=(float)posx;
		wposy=(float)posy;
//	}

}

void	WindowJavascript::GetMousePosInDesignWindow(int posx,int posy,float& wposx,float& wposy)
{
	posx -= (int)mPosition[0];
	/*if(posx >= (int)mSizeX)
		posx = ((int)mSizeX)-1;
	if(posx < 0)
		posx = 0;*/

	posy -= (int)mPosition[1];
	/*if(posy>=(int)mSizeY)
		posy = ((int)mSizeY)-1;
	if(posy < 0)
		posy = 0;*/

	if(mScreen)
	{
		mScreen->as<Draw::RenderingScreen>()->GetMousePosInDesignScreen(posx,posy,wposx,wposy);
	}
	else
	{
		wposx=(float)posx;
		wposy=(float)posy;
	}
}


void WindowJavascript::ShowMouse(bool bOn)
{
	
}
