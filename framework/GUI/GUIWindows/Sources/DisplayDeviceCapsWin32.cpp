#include "PrecompiledHeaders.h"
#include "DisplayDeviceCapsWin32.h"
#include "Core.h"
#include <Windows.h>

IMPLEMENT_CLASS_INFO(DisplayDeviceCapsWin32)

DisplayDeviceCapsWin32::DisplayDeviceCapsWin32(const kstl::string& name,CLASS_NAME_TREE_ARG) : DisplayDeviceCaps(name,PASS_CLASS_NAME_TREE_ARG)
{
	// use    EnumDisplaySettings and EnumDisplayDevices

	mDisplayDeviceList.clear();

	DISPLAY_DEVICE	displayDeviceDesc;
	displayDeviceDesc.cb=sizeof(DISPLAY_DEVICE);
	int index=0;

	while(EnumDisplayDevices(NULL,index,&displayDeviceDesc,EDD_GET_DEVICE_INTERFACE_NAME))
	{
		if(displayDeviceDesc.StateFlags&DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
		{			
			DisplayDevice toAdd;

			toAdd.mName=displayDeviceDesc.DeviceName;
			toAdd.mMain=false;
			
			// if first found, then it's the main one
			/*if(mDisplayDeviceList.size()==0)
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
				toAdd.mMain = true;
			}

			int indexMode=0;
			while(EnumDisplaySettings(displayDeviceDesc.DeviceName,indexMode,&devModeDesc))
			{
				
				DisplayDeviceCapacity modeToAdd;
				modeToAdd.mWidth=devModeDesc.dmPelsWidth;
				modeToAdd.mHeight=devModeDesc.dmPelsHeight;
				modeToAdd.mBitPerPixel=devModeDesc.dmBitsPerPel ;
				modeToAdd.mIsCurrent=false;
				if(	(currentBBP==modeToAdd.mBitPerPixel) &&
					(currentWidth==modeToAdd.mWidth) &&
					(currentHeight==modeToAdd.mHeight) )
				{
					modeToAdd.mIsCurrent=true;
				}
				toAdd.mCapacityList.push_back(modeToAdd);

				indexMode++;
				
			}

			mDisplayDeviceList[toAdd.mName]=toAdd;
		}
		index++;
	}


}

//! destructor
DisplayDeviceCapsWin32::~DisplayDeviceCapsWin32()
{  
}