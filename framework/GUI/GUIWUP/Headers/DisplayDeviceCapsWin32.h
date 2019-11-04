#ifndef _DISPLAYDEVICECAPSWIN32_H_
#define _DISPLAYDEVICECAPSWIN32_H_

#include "DisplayDeviceCaps.h"


// ****************************************
// * DisplayDeviceCapsWin32 class
// * --------------------------------------
/*!  \class DisplayDeviceCapsWin32
     a win32 DisplayDeviceCaps, used to get display device list and capacities
     \ingroup GUIWindowsModule
*/
// ****************************************


class DisplayDeviceCapsWin32 : public DisplayDeviceCaps
{
public:
    DECLARE_CLASS_INFO(DisplayDeviceCapsWin32,DisplayDeviceCaps,GUI)

	//! constructor
    DisplayDeviceCapsWin32(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
protected:
    
	//! destructor
	virtual ~DisplayDeviceCapsWin32();
 
};    

#endif //_DISPLAYDEVICECAPSWIN32_H_
