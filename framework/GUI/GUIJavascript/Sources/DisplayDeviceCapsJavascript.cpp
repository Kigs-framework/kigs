#include "DisplayDeviceCapsJavascript.h"
#include "Core.h"

using namespace Kigs::Gui;

extern "C" int	GetHTMLBodySizeX();
extern "C" int	GetHTMLBodySizeY();
extern "C" int	GetHTMLParentSizeX();
extern "C" int	GetHTMLParentSizeY();

IMPLEMENT_CLASS_INFO(DisplayDeviceCapsJavascript)

DisplayDeviceCapsJavascript::DisplayDeviceCapsJavascript(const std::string& name,CLASS_NAME_TREE_ARG) : DisplayDeviceCaps(name,PASS_CLASS_NAME_TREE_ARG)
{

	mDisplayDeviceList.clear();

	DisplayDevice toAdd;

	// only one screen
	toAdd.mName="SCREEN";
	toAdd.mMain=true;
	
	DisplayDeviceCapacity modeToAdd;
	
	Point2DI screenSize (GetHTMLBodySizeX(), GetHTMLBodySizeY());

	modeToAdd.mWidth= screenSize.x;
	modeToAdd.mHeight= screenSize.y;
	modeToAdd.mBitPerPixel=16;
	modeToAdd.mIsCurrent=true;

	toAdd.mCapacityList.push_back(modeToAdd);
	mDisplayDeviceList[toAdd.mName]=toAdd;

	// add device PARENT 
	DisplayDevice Parent;
	Parent.mName = "PARENT";
	Parent.mMain = false;

	Point2DI parentSize(GetHTMLParentSizeX(), GetHTMLParentSizeY());

	printf("Parent Size : %d %d\n", parentSize.x, parentSize.y);

	modeToAdd.mWidth = parentSize.x;
	modeToAdd.mHeight = parentSize.y;
	modeToAdd.mBitPerPixel = 16;
	modeToAdd.mIsCurrent = true;

	Parent.mCapacityList.push_back(modeToAdd);
	mDisplayDeviceList[Parent.mName] = Parent;

}

//! destructor
DisplayDeviceCapsJavascript::~DisplayDeviceCapsJavascript()
{  
}