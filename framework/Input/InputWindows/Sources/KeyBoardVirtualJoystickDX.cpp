#include "KeyBoardVirtualJoystickDX.h"
#include "Core.h"
#include "KeyboardDevice.h"


IMPLEMENT_CLASS_INFO(KeyBoardVirtualJoystickDX)

KeyBoardVirtualJoystickDX::KeyBoardVirtualJoystickDX(const std::string& name,CLASS_NAME_TREE_ARG) : JoystickDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
	std::set<RefCountedClass*>	instances;
	std::set<RefCountedClass*>::iterator	it;
	
	GetInstances("KeyboardDevice",instances);
	it=instances.begin();
	if(it!=instances.end())
	{
		myKeyboard=(KeyboardDevice*)*it;
	}
	
}

KeyBoardVirtualJoystickDX::~KeyBoardVirtualJoystickDX()
{  
	
}    

bool	KeyBoardVirtualJoystickDX::Aquire()
{
	myIsAquire=myKeyboard->Aquire();
	return myIsAquire;
}

bool	KeyBoardVirtualJoystickDX::Release()
{
	myKeyboard->Release();
	myIsAquire=false;
	return true;
}

void	KeyBoardVirtualJoystickDX::UpdateDevice()
{
	int currentDevice=0;
	
	// button 1 is space bar
	myDeviceItems[0]->getState()->SetValue((int)myKeyboard->getKeyState(DIK_SPACE).GetIntValue());
	
	// button 2 is altgr key
	myDeviceItems[1]->getState()->SetValue((int)myKeyboard->getKeyState(DIK_LCONTROL).GetIntValue());
	
	kfloat tmpval=0.0;
	// x axis is left and right arrow
	if(myKeyboard->getKeyState(DIK_LEFT).GetIntValue())
	{
		tmpval=-0.9;
	}
	else if(myKeyboard->getKeyState(DIK_RIGHT).GetIntValue())
	{
		tmpval=0.9;
	}
	
	myDeviceItems[2]->getState()->SetFloatValue(tmpval);
	
	// y axis
	if(myKeyboard->getKeyState(DIK_UP).GetIntValue())
	{
		tmpval=-0.9;
	}
	else if(myKeyboard->getKeyState(DIK_DOWN).GetIntValue())
	{
		tmpval=0.9;
	}
	myDeviceItems[3]->getState()->SetFloatValue(tmpval);
	
}

void	KeyBoardVirtualJoystickDX::DoInputDeviceDescription()
{
	myButtonsCount=2;
	myPovCount=0;
	myAxisCount=2;
	
	myDeviceItemsCount=myButtonsCount+myPovCount+myAxisCount;
	
	DeviceItem**	devicearray=new DeviceItem*[myDeviceItemsCount];
	
	int currentDevice=0;
	
	int currentButton;
	for(currentButton=0;currentButton<myButtonsCount;currentButton++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<int>(0));
	}
	
	int currentAxis;
	for(currentAxis=0;currentAxis<myAxisCount;currentAxis++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(0.0));
	}
	
	int currentPOV;
	for(currentPOV=0;currentPOV<myPovCount;currentPOV++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(0.0));
	}
	
	InitItems(myDeviceItemsCount,devicearray);
	
	for(currentButton=0;currentButton<myDeviceItemsCount;currentButton++)
	{
		delete devicearray[currentButton];
	}
	
	delete[] devicearray;
}
