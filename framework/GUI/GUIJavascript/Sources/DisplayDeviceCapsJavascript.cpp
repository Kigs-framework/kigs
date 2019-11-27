#include "DisplayDeviceCapsJavascript.h"
#include "Core.h"

extern "C" int	GetHTMLBodySizeX();
extern "C" int	GetHTMLBodySizeY();

IMPLEMENT_CLASS_INFO(DisplayDeviceCapsJavascript)

DisplayDeviceCapsJavascript::DisplayDeviceCapsJavascript(const kstl::string& name,CLASS_NAME_TREE_ARG) : DisplayDeviceCaps(name,PASS_CLASS_NAME_TREE_ARG)
{

	myDisplayDeviceList.clear();

	DisplayDevice toAdd;

	// only one screen
	toAdd.myName="SCREEN";
	toAdd.myMain=true;
	
	DisplayDeviceCapacity modeToAdd;
	
	Point2DI screenSize (GetHTMLBodySizeX(), GetHTMLBodySizeY());

	modeToAdd.myWidth= screenSize.x;
	modeToAdd.myHeight= screenSize.y;
	modeToAdd.myBitPerPixel=16;
	modeToAdd.myIsCurrent=true;

	toAdd.myCapacityList.push_back(modeToAdd);
	myDisplayDeviceList[toAdd.myName]=toAdd;

}

//! destructor
DisplayDeviceCapsJavascript::~DisplayDeviceCapsJavascript()
{  
}