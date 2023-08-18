#include "MultiTouchAndroid.h"
#include "Core.h"
#include "DeviceItem.h"
#include "NotificationCenter.h"

using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(MultiTouchAndroid)

IMPLEMENT_CONSTRUCTOR(MultiTouchAndroid)
{
	/*JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jclass  pMaClasse =g_env->FindClass("com/kigs/kigsmain/KigsGLSurfaceView");

	jmethodID getMaxTouch = g_env->GetStaticMethodID(pMaClasse, "getMaxTouch", "()I");
	mMaxTouch = g_env->CallStaticIntMethod(pMaClasse, getMaxTouch);
	mTouchList.resize(mMaxTouch);

	for (int i = 0; i < mMaxTouch; i++)
	{

		jmethodID getTouchClass = g_env->GetStaticMethodID(pMaClasse, "getTouchList", "(I)Lcom/kigs/input/KigsTouchEventList;");
		jobject pobjet = g_env->CallStaticObjectMethod(pMaClasse, getTouchClass, i);
		mTouchList[i] = g_env->NewGlobalRef(pobjet);
	}
	
	pMaClasse =g_env->FindClass("com/kigs/input/KigsTouchEventList");

	mGetEventCount=g_env->GetMethodID(pMaClasse, "getEventCount", "()I");
	mGetEvent=g_env->GetMethodID(pMaClasse, "getEvent", "(I)Lcom/kigs/input/KigsTouchEvent;");
	mClearEventList=g_env->GetMethodID(pMaClasse, "clearEventList", "()V");

	pMaClasse =g_env->FindClass("com/kigs/input/KigsTouchEvent");
	mEventGetX = g_env->GetMethodID(pMaClasse, "getX", "()F");
	mEventGetY = g_env->GetMethodID(pMaClasse, "getY", "()F");
	mEventGetAction = g_env->GetMethodID(pMaClasse, "getAction", "()I");
	*/
	KigsCore::GetNotificationCenter()->addObserver(this, "ReinitCB", "ResetContext");
}

MultiTouchAndroid::~MultiTouchAndroid()
{  
	/*JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	for (int i=0;i<mMaxTouch;i++) g_env->DeleteGlobalRef(mTouchList[i]);*/
}    


void	MultiTouchAndroid::UpdateDevice()
{
	/*JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	int currentDevice = 0;

	for (int touch = 0; touch < mMaxTouch; touch++)
	{
		int event_Count = g_env->CallIntMethod(mTouchList[touch], mGetEventCount);

		if (event_Count)
		{
			int i;

			// manage event
			for (i = 0; i < event_Count; i++)
			{
				jobject event = g_env->CallObjectMethod(mTouchList[touch], mGetEvent, i);

				int action = (int)g_env->CallIntMethod(event, mEventGetAction);
				float x = (float)g_env->CallFloatMethod(event, mEventGetX);
				float y = (float)g_env->CallFloatMethod(event, mEventGetY);
				currentDevice = touch*3;

				if (action == 1) // released
				{
					mDeviceItems[currentDevice++]->getState()->SetValue(x);
					mDeviceItems[currentDevice++]->getState()->SetValue(y);
					mDeviceItems[currentDevice++]->getState()->SetValue((int)0x00);
				}
				else
				{
					mDeviceItems[currentDevice++]->getState()->SetValue(x);
					mDeviceItems[currentDevice++]->getState()->SetValue(y);
					mDeviceItems[currentDevice++]->getState()->SetValue((int)0x80);
				}
			}

			g_env->CallVoidMethod(mTouchList[touch], mClearEventList);
		}

	}
	*/
	// call father update
	ParentClassType::UpdateDevice();
}

void	MultiTouchAndroid::DoInputDeviceDescription()
{

	mDeviceItemsCount=3 * mMaxTouch; // (posx + posy + state) * TouchCount

	DeviceItem**	devicearray=new DeviceItem*[mDeviceItemsCount];
	
	int currentDevice=0;

	for (int i = 0; i < mMaxTouch; i++)
	{
		devicearray[currentDevice++] = new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0)));
		devicearray[currentDevice++] = new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0)));
		devicearray[currentDevice++] = new DeviceItem(DeviceItemState<int>(0));
	}

	InitItems(mDeviceItemsCount,devicearray);

	for(currentDevice=0;currentDevice<mDeviceItemsCount;currentDevice++)
	{
		delete devicearray[currentDevice];
	}

	delete[] devicearray;

}

DEFINE_METHOD(MultiTouchAndroid, ReinitCB)
{
	printf("reinit MultiTouchAndroid\n");
	
	/*JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	for (int i=0;i<mMaxTouch;i++) 
		g_env->DeleteGlobalRef(mTouchList[i]);
	
	jclass  pMaClasse = g_env->FindClass("com/kigs/kigsmain/KigsGLSurfaceView");	
	jmethodID getMaxTouch = g_env->GetStaticMethodID(pMaClasse, "getMaxTouch", "()I");
	mMaxTouch = g_env->CallStaticIntMethod(pMaClasse, getMaxTouch);
	mTouchList.resize(mMaxTouch);

	for (int i = 0; i < mMaxTouch; i++)
	{
		jmethodID getTouchClass = g_env->GetStaticMethodID(pMaClasse, "getTouchList", "(I)Lcom/kigs/input/KigsTouchEventList;");
		jobject pobjet = g_env->CallStaticObjectMethod(pMaClasse, getTouchClass, i);
		mTouchList[i] = g_env->NewGlobalRef(pobjet);
	}
	*/
	return false;
}

