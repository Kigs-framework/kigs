#ifndef _DISPLAYDEVICECAPSJAVASCRIPT_H_
#define _DISPLAYDEVICECAPSJAVASCRIPT_H_

#include "DisplayDeviceCaps.h"


// ****************************************
// * DisplayDeviceCapsJavascript class
// * --------------------------------------
/*!  \class DisplayDeviceCapsJavascript
     a javascript DisplayDeviceCaps, used to get display device list and capacities
     \ingroup GUIModuleJavascript
*/
// ****************************************


class DisplayDeviceCapsJavascript : public DisplayDeviceCaps
{
public:
    DECLARE_CLASS_INFO(DisplayDeviceCapsJavascript,DisplayDeviceCaps,GUI)

	//! constructor
    DisplayDeviceCapsJavascript(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	bool SupportWindowedMode() override
	{
		return true; // enable creation of several windows in browser ?
	}
    
protected:
    
	//! destructor
	virtual ~DisplayDeviceCapsJavascript();
 
};    

#endif //_DISPLAYDEVICECAPSJAVASCRIPT_H_
