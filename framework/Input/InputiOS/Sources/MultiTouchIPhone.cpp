
#include "MultiTouchIPhone.h"
#include "Core.h"
#include "DeviceItem.h"
#include <pthread.h>

#include "Platform/Core/PlatformCore.h"
/***********************/

extern pthread_mutex_t	eventMutex;

IMPLEMENT_CLASS_INFO(MultiTouchIPhone)

MultiTouchIPhone::MultiTouchIPhone(const kstl::string& name,CLASS_NAME_TREE_ARG) : MultiTouchDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
	myMaxTouch = MAXTOUCH;
}

MultiTouchIPhone::~MultiTouchIPhone()
{  
	
}    

void	MultiTouchIPhone::UpdateDevice()
{
	pthread_mutex_lock( &eventMutex );
	
    int currentDevice;
	for(int i = 1; i < myMaxTouch; i++)
	{
		structCoord & clicCoord = listCoordinate[i];
        if(clicCoord.Touch != NULL)
        {
            currentDevice = i * 3;
			
            myDeviceItems[currentDevice++]->getState()->SetValue((int)clicCoord.mouseCoordinate[0]);
            myDeviceItems[currentDevice++]->getState()->SetValue((int)clicCoord.mouseCoordinate[1]);
            myDeviceItems[currentDevice++]->getState()->SetValue((clicCoord.active)?0x80:0x00);
			
            
            clicCoord.tapCount = 0;
            if(!clicCoord.active)
                listCoordinate[i].Touch = NULL;
            
        }
	}
	
	MultiTouchDevice::UpdateDevice();
	pthread_mutex_unlock( &eventMutex );
}

void	MultiTouchIPhone::DoInputDeviceDescription()
{
	myDeviceItemsCount = 3 * myMaxTouch; // (posx + posy + state) * TouchCount
	
	DeviceItem** devicearray = new DeviceItem*[myDeviceItemsCount];
	
	int currentDevice=0;
	
	for (int i = 0; i < myMaxTouch; i++)
	{
		devicearray[currentDevice++] = new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0)));
		devicearray[currentDevice++] = new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0)));
		devicearray[currentDevice++] = new DeviceItem(DeviceItemState<int>(0));
	}
	
	InitItems(myDeviceItemsCount, devicearray);
	
	for(currentDevice = 0; currentDevice < myDeviceItemsCount; currentDevice++)
	{
		delete devicearray[currentDevice];
	}
	
	delete[] devicearray;
}
