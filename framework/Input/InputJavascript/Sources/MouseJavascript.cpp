#include "MouseJavascript.h"
#include "Core.h"
#include "DeviceItem.h"

#include <emscripten.h>

using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(MouseJavascript)

/*
Methods:
callfunc = 0  =>  AddMouseListeners
callfunc = 1  =>  Get_InputCount
callfunc = 2  =>  Get_EventType
callfunc = 3  =>  Get_ButtonType
callfunc = 4  =>  Get_MousePosX
callfunc = 5  =>  Get_MousePosY
callfunc = 6  =>  ClearInputList
*/


extern "C" void		AddListenerToMouse();
extern "C" int		Get_MouseEventCount();
extern "C" int		Get_MouseEvent(int index);
extern "C" int		Get_MouseButton(int index);
extern "C" int		Get_MousePosX(int index);
extern "C" int		Get_MousePosY(int index);
extern "C" void		ClearMouseList();


MouseJavascript::MouseJavascript(const std::string& name,CLASS_NAME_TREE_ARG) : 
MouseDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
	AddListenerToMouse();
}

MouseJavascript::~MouseJavascript()
{  
	
}    

bool	MouseJavascript::Aquire()
{
	if (MouseDevice::Aquire())
	{
		mPosX = 0;
		mPosY = 0;
		return true;
	}
	return false;
}

bool	MouseJavascript::Release()
{
	return MouseDevice::Release();
}

void	MouseJavascript::UpdateDevice()
{
	//Get event count
	int value = Get_MouseEventCount();
	
	mDeviceItems[0]->getState()->SetValue(0);
	mDeviceItems[1]->getState()->SetValue(0);
	
	mPreviousDZ = mDZ;
	
	for(int i = 0; i < value; i++)
	{
		int PosX,PosY,ButtonType, EventType;
		
		EventType = Get_MouseEvent(i);
		ButtonType = Get_MouseButton(i);
		PosX = Get_MousePosX(i);
		PosY = Get_MousePosY(i);
		
		int currentDevice=0;
		
		if(i == value-1)
		{
			// posX and posY
			mDeviceItems[currentDevice++]->getState()->SetValue(PosX-(int)(float)mPosX);
			mDeviceItems[currentDevice++]->getState()->SetValue(PosY-(int)(float)mPosY);
		}
		
		if(ButtonType == 1) // left
		{
			if(EventType == 1) // Pressed
			mDeviceItems[2]->getState()->SetValue(0x80);
			else
				mDeviceItems[2]->getState()->SetValue(0x00);
		}
		
		else if(ButtonType == 3) // right
		{
			if(EventType == 1) // Pressed
			mDeviceItems[3]->getState()->SetValue(0x80);
			else
				mDeviceItems[3]->getState()->SetValue(0x00);
		}
		
		else if(ButtonType == 2) // wheel
		{
			if(EventType == 1) // Pressed
			mDeviceItems[4]->getState()->SetValue(0x80);
			else if(EventType == 2)
				mDeviceItems[4]->getState()->SetValue(0x00);
			else// wheel roll
			mDZ += PosX;
		}
	}
	
	//clear input list
	ClearMouseList();
	// call father update
	MouseDevice::UpdateDevice();
}

void	MouseJavascript::DoInputDeviceDescription()
{
	
	mButtonsCount=3;
	mDeviceItemsCount=5; // 2 for posx and posy
	
	DeviceItem**	devicearray=new DeviceItem*[mDeviceItemsCount];
	
	int currentDevice=0;
	
	devicearray[currentDevice++]=new DeviceItem(DeviceItemState<float>(0.0f));
	devicearray[currentDevice++]=new DeviceItem(DeviceItemState<float>(0.0f));
	
	int currentButton;
	for(currentButton=0;currentButton<mButtonsCount;currentButton++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<int>(0));
	}
	
	InitItems(mDeviceItemsCount,devicearray);
	
	for(currentDevice=0;currentDevice<mDeviceItemsCount;currentDevice++)
	{
		delete devicearray[currentDevice];
	}
	
	delete[] devicearray;
	
}
