#include "PrecompiledHeaders.h"
#include "DisplayDeviceCapsWin32.h"
#include "Core.h"
#include <Windows.h>

IMPLEMENT_CLASS_INFO(DisplayDeviceCapsWin32)

DisplayDeviceCapsWin32::DisplayDeviceCapsWin32(const kstl::string& name,CLASS_NAME_TREE_ARG) : DisplayDeviceCaps(name,PASS_CLASS_NAME_TREE_ARG)
{
	// use    EnumDisplaySettings and EnumDisplayDevices

	myDisplayDeviceList.clear();

	DISPLAY_DEVICE	displayDeviceDesc;
	displayDeviceDesc.cb=sizeof(DISPLAY_DEVICE);
	int index=0;

	while(EnumDisplayDevices(NULL,index,&displayDeviceDesc,EDD_GET_DEVICE_INTERFACE_NAME))
	{
		if(displayDeviceDesc.StateFlags&DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
		{			
			DisplayDevice toAdd;

			toAdd.myName=displayDeviceDesc.DeviceName;
			toAdd.myMain=false;
			
			// if first found, then it's the main one
			/*if(myDisplayDeviceList.size()==0)
			{
				toAdd.myMain=true;
			}*/

			DEVMODE devModeDesc;
			devModeDesc.dmSize=sizeof(DEVMODE);

			// get current settings
			
			EnumDisplaySettings(displayDeviceDesc.DeviceName,ENUM_CURRENT_SETTINGS,&devModeDesc);

			int currentWidth=devModeDesc.dmPelsWidth;
			int currentHeight=devModeDesc.dmPelsHeight;
			int currentBBP = devModeDesc.dmBitsPerPel;

			if (devModeDesc.dmPosition.x == 0 && devModeDesc.dmPosition.y == 0)
			{
				toAdd.myMain = true;
			}

			int indexMode=0;
			while(EnumDisplaySettings(displayDeviceDesc.DeviceName,indexMode,&devModeDesc))
			{
				
				DisplayDeviceCapacity modeToAdd;
				modeToAdd.myWidth=devModeDesc.dmPelsWidth;
				modeToAdd.myHeight=devModeDesc.dmPelsHeight;
				modeToAdd.myBitPerPixel=devModeDesc.dmBitsPerPel ;
				modeToAdd.myIsCurrent=false;
				if(	(currentBBP==modeToAdd.myBitPerPixel) &&
					(currentWidth==modeToAdd.myWidth) &&
					(currentHeight==modeToAdd.myHeight) )
				{
					modeToAdd.myIsCurrent=true;
				}
				toAdd.myCapacityList.push_back(modeToAdd);

				indexMode++;
				
			}

			myDisplayDeviceList[toAdd.myName]=toAdd;
		}
		index++;
	}


}

//! destructor
DisplayDeviceCapsWin32::~DisplayDeviceCapsWin32()
{  
}