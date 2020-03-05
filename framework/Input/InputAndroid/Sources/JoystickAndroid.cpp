#include "JoystickAndroid.h"
#include "DeviceItem.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(JoystickAndroid)

JoystickAndroid::JoystickAndroid(const kstl::string& name,CLASS_NAME_TREE_ARG) : JoystickDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	
	jclass  pMaClasse =g_env->FindClass("com/kigs/kigsmain/kigsmainActivity");
	
	myActivityClass=(jclass)g_env->NewGlobalRef(pMaClasse);
	
	getBackKey=g_env->GetStaticMethodID(myActivityClass, "GetBackKeyState", "()Z");
	getMenuKey=g_env->GetStaticMethodID(myActivityClass, "GetMenuKeyState", "()Z");
}

JoystickAndroid::~JoystickAndroid()
{  
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	
	g_env->DeleteGlobalRef(myActivityClass);
	myActivityClass=0;
}    

void	JoystickAndroid::UpdateDevice()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	int currentDevice=0;

	bool keystate=(bool)g_env->CallStaticBooleanMethod(myActivityClass,getBackKey);
	
	if(keystate)
	{
		// back key
		myDeviceItems[currentDevice++]->getState()->SetValue((int)1);
	}
	else
	{
		myDeviceItems[currentDevice++]->getState()->SetValue((int)0);
	}
		
	keystate=(bool)g_env->CallStaticBooleanMethod(myActivityClass,getMenuKey);
	
	if(keystate)
	{
		// menu key
		myDeviceItems[currentDevice++]->getState()->SetValue((int)1);
	}
	else
	{
		myDeviceItems[currentDevice++]->getState()->SetValue((int)0);
	}

}

void	JoystickAndroid::DoInputDeviceDescription()
{

	// manage back and menu keys
	myButtonsCount=2;
	myPovCount=0;
	myAxisCount=0;
	
	myDeviceItemsCount=(unsigned int)(myButtonsCount+myPovCount+myAxisCount);

	DeviceItem**	devicearray=new DeviceItem*[myDeviceItemsCount];
	
	int currentDevice=0;

	int currentButton;
	for(currentButton=0;currentButton<myButtonsCount;currentButton++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<int>(0));
	}

	int currentAxis;
	for(currentAxis=0;currentAxis<myAxisCount;currentAxis++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0)));
	}

	int currentPOV;
	for(currentPOV=0;currentPOV<myPovCount;currentPOV++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0)));
	}

	InitItems(myDeviceItemsCount,devicearray);

	for(currentButton=0;currentButton<myDeviceItemsCount;currentButton++)
	{
		delete devicearray[currentButton];
	}

	delete[] devicearray;

}
