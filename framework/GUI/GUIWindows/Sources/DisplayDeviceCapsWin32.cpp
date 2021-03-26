#include "PrecompiledHeaders.h"
#include "DisplayDeviceCapsWin32.h"
#include "Core.h"
#include <Windows.h>
#include <shellscalingapi.h>

#pragma comment(lib, "Shcore.lib")

IMPLEMENT_CLASS_INFO(DisplayDeviceCapsWin32)

BOOL EnumDisplayMonitorsCallback(HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM result_ptr)
{
	*(HMONITOR*)result_ptr = monitor;
	return false;
}

DisplayDeviceCapsWin32::DisplayDeviceCapsWin32(const kstl::string& name,CLASS_NAME_TREE_ARG) : DisplayDeviceCaps(name,PASS_CLASS_NAME_TREE_ARG)
{
	// use    EnumDisplaySettings and EnumDisplayDevices

	mDisplayDeviceList.clear();

	DISPLAY_DEVICE	displayDeviceDesc;
	displayDeviceDesc.cb=sizeof(DISPLAY_DEVICE);
	int index=0;
	RECT testrect;
	testrect.top = 0;
	testrect.bottom = 1;
	testrect.left = 0;
	testrect.right = 1;
	HMONITOR result=nullptr;
	EnumDisplayMonitors(NULL, &testrect, EnumDisplayMonitorsCallback, (LPARAM)&result);
	float scaling = 1.0f;
	if (result)
	{
		if (SetProcessDpiAwareness(PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE) == S_OK)
		{
			v2u eff_dpi(0u, 0u);
			if (GetDpiForMonitor(result, MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI, &eff_dpi.x, &eff_dpi.y) == S_OK)
			{
				if (eff_dpi.x != 0)
				{
					scaling = (float)eff_dpi.x / 96.0f;
				}
			}
			//SetProcessDpiAwareness(PROCESS_DPI_AWARENESS::PROCESS_DPI_UNAWARE);
		}
	}

	while(EnumDisplayDevices(NULL,index,&displayDeviceDesc,EDD_GET_DEVICE_INTERFACE_NAME))
	{
		if(displayDeviceDesc.StateFlags&DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
		{			
			DisplayDevice toAdd;

			toAdd.mName=displayDeviceDesc.DeviceName;
			toAdd.mMain=false;

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
				toAdd.mScaling = scaling;
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