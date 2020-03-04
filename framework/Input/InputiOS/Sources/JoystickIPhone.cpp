#include "JoystickIPhone.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(JoystickIPhone)

JoystickIPhone::JoystickIPhone(const kstl::string& name,CLASS_NAME_TREE_ARG) : JoystickDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
	
}

JoystickIPhone::~JoystickIPhone()
{  
	
}    


void	JoystickIPhone::UpdateDevice()
{
	// TODO
	/*u16 padvalue=PAD_Read();
	
	int currentDevice=0;
	
	
	myDeviceItems[currentDevice++]->getState()->SetIntValue(padvalue& PAD_BUTTON_A);
	myDeviceItems[currentDevice++]->getState()->SetIntValue(padvalue& PAD_BUTTON_B);
	myDeviceItems[currentDevice++]->getState()->SetIntValue(padvalue& PAD_BUTTON_SELECT);
	myDeviceItems[currentDevice++]->getState()->SetIntValue(padvalue& PAD_BUTTON_START);
	myDeviceItems[currentDevice++]->getState()->SetIntValue(padvalue& PAD_BUTTON_R);
	myDeviceItems[currentDevice++]->getState()->SetIntValue(padvalue& PAD_BUTTON_L);
	myDeviceItems[currentDevice++]->getState()->SetIntValue(padvalue& PAD_BUTTON_X);
	myDeviceItems[currentDevice++]->getState()->SetIntValue(padvalue& PAD_BUTTON_Y);
	myDeviceItems[currentDevice++]->getState()->SetIntValue(padvalue& PAD_BUTTON_DEBUG);
	
	
	// axis X
	if(padvalue&PAD_KEY_RIGHT)
	{
	 myDeviceItems[currentDevice++]->getState()->SetFloatValue(KFLOAT_CONST(1.0));		
	}
	else if(padvalue&PAD_KEY_LEFT)
	{
	 myDeviceItems[currentDevice++]->getState()->SetFloatValue(KFLOAT_CONST(-1.0));					
	}
	else
	{
	 myDeviceItems[currentDevice++]->getState()->SetFloatValue(KFLOAT_CONST(0.0));			
	}
	// axis Y
	if(padvalue&PAD_KEY_UP)
	{
	 myDeviceItems[currentDevice++]->getState()->SetFloatValue(KFLOAT_CONST(1.0));		
	}
	else if(padvalue&PAD_KEY_DOWN)
	{
	 myDeviceItems[currentDevice++]->getState()->SetFloatValue(KFLOAT_CONST(-1.0));					
	}
	else
	{
	 myDeviceItems[currentDevice++]->getState()->SetFloatValue(KFLOAT_CONST(0.0));			
	}
   */
}

void	JoystickIPhone::DoInputDeviceDescription()
{
	
	myButtonsCount=9;
	myPovCount=0;
	myAxisCount=2;
	
	myDeviceItemsCount=(unsigned int)(myButtonsCount+myPovCount+myAxisCount);
	
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
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0)));
	}
	
	int currentPOV;
	for(currentPOV=0;currentPOV<myPovCount;currentPOV++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0)));
	}
	
	InitItems(myDeviceItemsCount,devicearray);
	
	for(currentButton=0;currentButton<myDeviceItemsCount;currentButton++)
	{
		delete devicearray[currentButton];
	}
	
	delete[] devicearray;
	
}
