#include "PrecompiledHeaders.h"
#include "DisplayDeviceCapsAndroid.h"
#include "Core.h"



IMPLEMENT_CLASS_INFO(DisplayDeviceCapsAndroid)

DisplayDeviceCapsAndroid::DisplayDeviceCapsAndroid(const kstl::string& name,CLASS_NAME_TREE_ARG) : DisplayDeviceCaps(name,PASS_CLASS_NAME_TREE_ARG)
{

	myDisplayDeviceList.clear();

	DisplayDevice toAdd;

	// only one screen
	toAdd.myName="SCREEN";
	toAdd.myMain=true;
	
	DisplayDeviceCapacity modeToAdd;
	
	JNIEnv* pEnv=KigsJavaIDManager::getEnv(pthread_self());

	modeToAdd.myWidth=(int)pEnv->CallStaticIntMethod(KigsJavaIDManager::Renderer_class,  KigsJavaIDManager::GetResolutionX);
	modeToAdd.myHeight=(int)pEnv->CallStaticIntMethod(KigsJavaIDManager::Renderer_class,  KigsJavaIDManager::GetResolutionY);
	modeToAdd.myBitPerPixel=16;
	modeToAdd.myIsCurrent=true;	

	toAdd.myCapacityList.push_back(modeToAdd);
	myDisplayDeviceList[toAdd.myName]=toAdd;

}

//! destructor
DisplayDeviceCapsAndroid::~DisplayDeviceCapsAndroid()
{  
}