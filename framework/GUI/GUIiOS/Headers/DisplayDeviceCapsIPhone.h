#ifndef _DISPLAYDEVICECAPSANDROID_H_
#define _DISPLAYDEVICECAPSANDROID_H_

#include "DisplayDeviceCaps.h"


// ****************************************
// * DisplayDeviceCapsAndroid class
// * --------------------------------------
/*!  \class DisplayDeviceCapsAndroid
     an android DisplayDeviceCaps, used to get display device list and capacities
     \ingroup GUIWindowsModule
*/
// ****************************************


class DisplayDeviceCapsIPhone : public DisplayDeviceCaps
{
public:
    DECLARE_CLASS_INFO(DisplayDeviceCapsIPhone,DisplayDeviceCaps,GUI)

	//! constructor
    DisplayDeviceCapsIPhone(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
protected:
    
	//! destructor
	virtual ~DisplayDeviceCapsIPhone();
 
};    

#endif //_DISPLAYDEVICECAPSANDROID_H_
