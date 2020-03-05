/*#import <UIKit/UIKit.h>
#import <UIKit/UIScreen.h>
#import <GLKit/GLKit.h>
*/
#include "PrecompiledHeaders.h"
#include "DisplayDeviceCapsIPhone.h"
#include "Core.h"

/****** resolution ******/
/*   true  -> 640*960   */
/*   false -> 320*480   */
extern bool highDefinition;
/************************/

extern unsigned int		globalScreenSizeX;
extern unsigned int		globalScreenSizeY;

IMPLEMENT_CLASS_INFO(DisplayDeviceCapsIPhone)

DisplayDeviceCapsIPhone::DisplayDeviceCapsIPhone(const kstl::string& name,CLASS_NAME_TREE_ARG) : DisplayDeviceCaps(name,PASS_CLASS_NAME_TREE_ARG)
{   
	myDisplayDeviceList.clear();
	
	DisplayDevice toAdd;

	// only one screen
	toAdd.myName="SCREEN";
	toAdd.myMain=true;
	
	DisplayDeviceCapacity modeToAdd;

	modeToAdd.myWidth= globalScreenSizeX;
	modeToAdd.myHeight= globalScreenSizeY;
	modeToAdd.myBitPerPixel=16;
	modeToAdd.myIsCurrent=true;	

	toAdd.myCapacityList.push_back(modeToAdd);
	myDisplayDeviceList[toAdd.myName]=toAdd;
	
}

//! destructor
DisplayDeviceCapsIPhone::~DisplayDeviceCapsIPhone()
{  
}