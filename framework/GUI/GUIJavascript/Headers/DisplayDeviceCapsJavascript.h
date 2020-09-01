#ifndef _DISPLAYDEVICECAPSJAVASCRIPT_H_
#define _DISPLAYDEVICECAPSJAVASCRIPT_H_

#include "DisplayDeviceCaps.h"


// ****************************************
// * DisplayDeviceCapsJavascript class
// * --------------------------------------
/**
* \file	DisplayDeviceCapsJavascript.h
* \class	DisplayDeviceCapsJavascript
* \ingroup GUIModule
* \brief	Javascript display device capacities.
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
