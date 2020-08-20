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
extern "C" void SetAbsolutePos(const char* window,int abs);

WindowJavascript*	WindowJavascript::mFirstWindow=0;

//! constructor
WindowJavascript::WindowJavascript(const kstl::string& name,CLASS_NAME_TREE_ARG) : 
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
	if(mScreen)
		mScreen->Destroy();

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

	printf("window pos : %d %d \n", mPositionX.const_ref(), mPositionY.const_ref());

	if (mFullScreen)
	{
		Point2DI screenSize(GetHTMLBodySizeX(), GetHTMLBodySizeY());
		SetCanvasSize(getName().c_str(),screenSize.x, screenSize.y);
		mSizeX = screenSize.x;
		mSizeY = screenSize.y;
		SetCanvasPos(getName().c_str(),0, 0);
		mPositionX = 0;
		mPositionY = 0;
	}
	else
	{
		SetCanvasSize(getName().c_str(),mSizeX, mSizeY);
		SetCanvasPos(getName().c_str(),mPositionX, mPositionY);
	}

	Show();
}

//! show window
void  WindowJavascript::Show()
{
	if(mScreen && !mScreen->IsInit())
	{
		mScreen->setValue(LABEL_TO_ID(ParentWindowName),getName());
		mScreen->Init();
	}

}    

//! update window
void  WindowJavascript::Update(const Timer&  timer, void* addParam)
{
	//! call screen update
	if(mScreen)
	{
		// check if size has changed
		if (mFullScreen)
		{
			Point2DI screenSize(GetHTMLBodySizeX(), GetHTMLBodySizeY());

			if ((screenSize.x != mSizeX) || (screenSize.y != mSizeY))
			{
				mSizeX = screenSize.x;
				mSizeY = screenSize.y;
				SetCanvasSize(getName().c_str(),screenSize.x, screenSize.y);
				mScreen->Resize((int)mSizeX, (int)mSizeY);
			}
		}

		mPositionX = GetCanvasPositionX(getName().c_str());
		mPositionY = GetCanvasPositionY(getName().c_str());
		mScreen->CallUpdate(timer, addParam);
	}

   Window::Update(timer, addParam);
}    


void	WindowJavascript::GetMousePosInWindow(int posx,int posy,kfloat& wposx,kfloat& wposy)
{
	posx -= (int)mPositionX;
	if(posx >= (int)mSizeX)
		posx = ((int)mSizeX)-1;
	if(posx < 0)
		posx = 0;

	posy -= (int)mPositionY;
	if(posy>=(int)mSizeY)
		posy = ((int)mSizeY)-1;
	if(posy < 0)
		posy = 0;

	/*if(mScreen)
	{
		mScreen->GetMousePosInScreen(posx,posy,wposx,wposy);
	}
	else
	{*/
		wposx=(kfloat)posx;
		wposy=(kfloat)posy;
//	}

}

void	WindowJavascript::GetMousePosInDesignWindow(int posx,int posy,kfloat& wposx,kfloat& wposy)
{
	posx -= (int)mPositionX;
	/*if(posx >= (int)mSizeX)
		posx = ((int)mSizeX)-1;
	if(posx < 0)
		posx = 0;*/

	posy -= (int)mPositionY;
	/*if(posy>=(int)mSizeY)
		posy = ((int)mSizeY)-1;
	if(posy < 0)
		posy = 0;*/

	if(mScreen)
	{
		mScreen->GetMousePosInDesignScreen(posx,posy,wposx,wposy);
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
