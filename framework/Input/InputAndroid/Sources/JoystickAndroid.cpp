#include "JoystickAndroid.h"
#include "DeviceItem.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(JoystickAndroid)

JoystickAndroid::JoystickAndroid(const kstl::string& name,CLASS_NAME_TREE_ARG) : JoystickDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	
	jclass  pMaClasse =g_env->FindClass("com/kigs/kigsmain/kigsmainActivity");
	
	mActivityClass=(jclass)g_env->NewGlobalRef(pMaClasse);
	
	mGetBackKey=g_env->GetStaticMethodID(mActivityClass, "GetBackKeyState", "()Z");
	mGetMenuKey=g_env->GetStaticMethodID(mActivityClass, "GetMenuKeyState", "()Z");
}

JoystickAndroid::~JoystickAndroid()
{  
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	
	g_env->DeleteGlobalRef(mActivityClass);
	mActivityClass=0;
}    

void	JoystickAndroid::UpdateDevice()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	int currentDevice=0;

	bool keystate=(bool)g_env->CallStaticBooleanMethod(mActivityClass,mGetBackKey);
	
	if(keystate)
	{
		// back key
		mDeviceItems[currentDevice++]->getState()->SetValue((int)1);
	}
	else
	{
		mDeviceItems[currentDevice++]->getState()->SetValue((int)0);
	}
		
	keystate=(bool)g_env->CallStaticBooleanMethod(mActivityClass,mGetMenuKey);
	
	if(keystate)
	{
		// menu key
		mDeviceItems[currentDevice++]->getState()->SetValue((int)1);
	}
	else
	{
		mDeviceItems[currentDevice++]->getState()->SetValue((int)0);
	}

}

void	JoystickAndroid::DoInputDeviceDescription()
{

	// manage back and menu keys
	mButtonsCount=2;
	mPovCount=0;
	mAxisCount=0;
	
	mDeviceItemsCount=(unsigned int)(mButtonsCount+mPovCount+mAxisCount);

	DeviceItem**	devicearray=new DeviceItem*[mDeviceItemsCount];
	
	int currentDevice=0;

	int currentButton;
	for(currentButton=0;currentButton<mButtonsCount;currentButton++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<int>(0));
	}

	int currentAxis;
	for(currentAxis=0;currentAxis<mAxisCount;currentAxis++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0)));
	}

	int currentPOV;
	for(currentPOV=0;currentPOV<mPovCount;currentPOV++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0)));
	}

	InitItems(mDeviceItemsCount,devicearray);

	for(currentButton=0;currentButton<mDeviceItemsCount;currentButton++)
	{
		delete devicearray[currentButton];
	}

	delete[] devicearray;

}
