#include "MouseJavascript.h"
#include "Core.h"
#include "DeviceItem.h"

#include <emscripten.h>

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


MouseJavascript::MouseJavascript(const kstl::string& name,CLASS_NAME_TREE_ARG) : 
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
		myPosX = 0;
		myPosY = 0;
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
	
	myDeviceItems[0]->getState()->SetValue(0);
	myDeviceItems[1]->getState()->SetValue(0);
	
	myPreviousDZ = myDZ;
	
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
			myDeviceItems[currentDevice++]->getState()->SetValue(PosX-(int)(kfloat)myPosX);
			myDeviceItems[currentDevice++]->getState()->SetValue(PosY-(int)(kfloat)myPosY);
		}
		
		if(ButtonType == 1) // left
		{
			if(EventType == 1) // Pressed
			myDeviceItems[2]->getState()->SetValue(0x80);
			else
				myDeviceItems[2]->getState()->SetValue(0x00);
		}
		
		else if(ButtonType == 3) // right
		{
			if(EventType == 1) // Pressed
			myDeviceItems[3]->getState()->SetValue(0x80);
			else
				myDeviceItems[3]->getState()->SetValue(0x00);
		}
		
		else if(ButtonType == 2) // wheel
		{
			if(EventType == 1) // Pressed
			myDeviceItems[4]->getState()->SetValue(0x80);
			else if(EventType == 2)
				myDeviceItems[4]->getState()->SetValue(0x00);
			else// wheel roll
			myDZ += PosX;
		}
	}
	
	//clear input list
	ClearMouseList();
	// call father update
	MouseDevice::UpdateDevice();
}

void	MouseJavascript::DoInputDeviceDescription()
{
	
	myButtonsCount=3;
	myDeviceItemsCount=5; // 2 for posx and posy
	
	DeviceItem**	devicearray=new DeviceItem*[myDeviceItemsCount];
	
	int currentDevice=0;
	
	devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0)));
	devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0)));
	
	int currentButton;
	for(currentButton=0;currentButton<myButtonsCount;currentButton++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<int>(0));
	}
	
	InitItems(myDeviceItemsCount,devicearray);
	
	for(currentDevice=0;currentDevice<myDeviceItemsCount;currentDevice++)
	{
		delete devicearray[currentDevice];
	}
	
	delete[] devicearray;
	
}
