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


class DisplayDeviceCapsAndroid : public DisplayDeviceCaps
{
public:
    DECLARE_CLASS_INFO(DisplayDeviceCapsAndroid,DisplayDeviceCaps,GUI)

	//! constructor
    DisplayDeviceCapsAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
protected:
    
	//! destructor
	virtual ~DisplayDeviceCapsAndroid();
 
};    

#endif //_DISPLAYDEVICECAPSANDROID_H_
