#include "PrecompiledHeaders.h"
#include "DisplayDeviceCapsAndroid.h"
#include "Core.h"



IMPLEMENT_CLASS_INFO(DisplayDeviceCapsAndroid)

DisplayDeviceCapsAndroid::DisplayDeviceCapsAndroid(const kstl::string& name,CLASS_NAME_TREE_ARG) : DisplayDeviceCaps(name,PASS_CLASS_NAME_TREE_ARG)
{

	mDisplayDeviceList.clear();

	DisplayDevice toAdd;

	// only one screen
	toAdd.mName="SCREEN";
	toAdd.mMain=true;
	
	DisplayDeviceCapacity modeToAdd;
	
	JNIEnv* pEnv=KigsJavaIDManager::getEnv(pthread_self());

	modeToAdd.mWidth=(int)pEnv->CallStaticIntMethod(KigsJavaIDManager::Renderer_class,  KigsJavaIDManager::GetResolutionX);
	modeToAdd.mHeight=(int)pEnv->CallStaticIntMethod(KigsJavaIDManager::Renderer_class,  KigsJavaIDManager::GetResolutionY);
	modeToAdd.mBitPerPixel=16;
	modeToAdd.mIsCurrent=true;	

	toAdd.mCapacityList.push_back(modeToAdd);
	mDisplayDeviceList[toAdd.mName]=toAdd;

}

//! destructor
DisplayDeviceCapsAndroid::~DisplayDeviceCapsAndroid()
{  
}