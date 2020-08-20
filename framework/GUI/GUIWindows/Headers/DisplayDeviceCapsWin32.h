#ifndef _DISPLAYDEVICECAPSWIN32_H_
#define _DISPLAYDEVICECAPSWIN32_H_

#include "DisplayDeviceCaps.h"

// ****************************************
// * DisplayDeviceCapsWin32 class
// * --------------------------------------
/**
* \file	DisplayDeviceCapsWin32.h
* \class	DisplayDeviceCapsWin32
* \ingroup GUIModule
* \brief	Specific Win32 DisplayDeviceCaps, used to get display device list and capacities.
*/
// ****************************************
class DisplayDeviceCapsWin32 : public DisplayDeviceCaps
{
public:
    DECLARE_CLASS_INFO(DisplayDeviceCapsWin32,DisplayDeviceCaps,GUI)

	//! constructor
    DisplayDeviceCapsWin32(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	bool SupportWindowedMode() override
	{
		return true; // enable creation of several windows on Win32
	}

    
protected:
    
	//! destructor
	virtual ~DisplayDeviceCapsWin32();
 
};    

#endif //_DISPLAYDEVICECAPSWIN32_H_
