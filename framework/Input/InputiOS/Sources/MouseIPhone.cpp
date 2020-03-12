
#include "MouseIPhone.h"
#include "Core.h"
#include "DeviceItem.h"

#include <pthread.h>

#include "Platform/Core/PlatformCore.h"

extern pthread_mutex_t	eventMutex;	


IMPLEMENT_CLASS_INFO(MouseIPhone)

MouseIPhone::MouseIPhone(const kstl::string& name,CLASS_NAME_TREE_ARG) : MouseDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
	
	myPosX = 0;
	myPosY = 0;	
	m_FrameCountSinceLastValidTouched = 1000; //Any big value
}

MouseIPhone::~MouseIPhone()
{  
	
}    

bool	MouseIPhone::Aquire()
{
	if (MouseDevice::Aquire())
	{
		myPosX = 0;
		myPosY = 0;
		m_FrameCountSinceLastValidTouched = 1000; //Any big value
		return true;
	}
	return false;
}

bool	MouseIPhone::Release()
{
	return MouseDevice::Release();
}

void	MouseIPhone::UpdateDevice()
{
	pthread_mutex_lock( &eventMutex );
	
    structCoord& clicCoord = listCoordinate[0];
    if(clicCoord.Touch)
    {
        int posX = clicCoord.mouseCoordinate[0];
        int posY = clicCoord.mouseCoordinate[1];
		
        int currentDevice=0;
		
        myDeviceItems[currentDevice++]->getState()->SetValue((int)posX-(int)(kfloat)myPosX);
        myDeviceItems[currentDevice++]->getState()->SetValue((int)posY-(int)(kfloat)myPosY);
        myDeviceItems[currentDevice++]->getState()->SetValue((listCoordinate[0].active)?0x80:0x00);
		
		
        clicCoord.tapCount = 0;
		
        if(!clicCoord.active)
            listCoordinate[0].Touch = NULL;
    }
    // call father update
    MouseDevice::UpdateDevice();
	pthread_mutex_unlock( &eventMutex );
}

void	MouseIPhone::DoInputDeviceDescription()
{
	
	myButtonsCount=1;
	myDeviceItemsCount=3; // 2 for posx and posy
	
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



