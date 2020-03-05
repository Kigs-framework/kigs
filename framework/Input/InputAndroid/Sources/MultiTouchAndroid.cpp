#include "MultiTouchAndroid.h"
#include "Core.h"
#include "DeviceItem.h"
#include "NotificationCenter.h"

IMPLEMENT_CLASS_INFO(MultiTouchAndroid)

IMPLEMENT_CONSTRUCTOR(MultiTouchAndroid)
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jclass  pMaClasse =g_env->FindClass("com/kigs/kigsmain/KigsGLSurfaceView");

	jmethodID getMaxTouch = g_env->GetStaticMethodID(pMaClasse, "getMaxTouch", "()I");
	myMaxTouch = g_env->CallStaticIntMethod(pMaClasse, getMaxTouch);
	myTouchList.resize(myMaxTouch);

	for (int i = 0; i < myMaxTouch; i++)
	{

		jmethodID getTouchClass = g_env->GetStaticMethodID(pMaClasse, "getTouchList", "(I)Lcom/kigs/input/KigsTouchEventList;");
		jobject pobjet = g_env->CallStaticObjectMethod(pMaClasse, getTouchClass, i);
		myTouchList[i] = g_env->NewGlobalRef(pobjet);
	}
	
	pMaClasse =g_env->FindClass("com/kigs/input/KigsTouchEventList");

	getEventCount=g_env->GetMethodID(pMaClasse, "getEventCount", "()I");
	getEvent=g_env->GetMethodID(pMaClasse, "getEvent", "(I)Lcom/kigs/input/KigsTouchEvent;");
	clearEventList=g_env->GetMethodID(pMaClasse, "clearEventList", "()V");

	pMaClasse =g_env->FindClass("com/kigs/input/KigsTouchEvent");
	Event_getX = g_env->GetMethodID(pMaClasse, "getX", "()F");
	Event_getY = g_env->GetMethodID(pMaClasse, "getY", "()F");
	Event_getAction = g_env->GetMethodID(pMaClasse, "getAction", "()I");
	
	KigsCore::GetNotificationCenter()->addObserver(this, "ReinitCB", "ResetContext");
}

MultiTouchAndroid::~MultiTouchAndroid()
{  
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	for (int i=0;i<myMaxTouch;i++) g_env->DeleteGlobalRef(myTouchList[i]);
}    


void	MultiTouchAndroid::UpdateDevice()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	int currentDevice = 0;

	for (int touch = 0; touch < myMaxTouch; touch++)
	{
		int event_Count = g_env->CallIntMethod(myTouchList[touch], getEventCount);

		if (event_Count)
		{
			int i;

			// manage event
			for (i = 0; i < event_Count; i++)
			{
				jobject event = g_env->CallObjectMethod(myTouchList[touch], getEvent, i);

				int action = (int)g_env->CallIntMethod(event, Event_getAction);
				float x = (float)g_env->CallFloatMethod(event, Event_getX);
				float y = (float)g_env->CallFloatMethod(event, Event_getY);
				currentDevice = touch*3;

				if (action == 1) // released
				{
					myDeviceItems[currentDevice++]->getState()->SetValue(x);
					myDeviceItems[currentDevice++]->getState()->SetValue(y);
					myDeviceItems[currentDevice++]->getState()->SetValue((int)0x00);
				}
				else
				{
					myDeviceItems[currentDevice++]->getState()->SetValue(x);
					myDeviceItems[currentDevice++]->getState()->SetValue(y);
					myDeviceItems[currentDevice++]->getState()->SetValue((int)0x80);
				}
			}

			g_env->CallVoidMethod(myTouchList[touch], clearEventList);
		}
	/*	else // no event = no move
		{
			myDeviceItems[currentDevice++]->getState()->SetValue((float)0);
			myDeviceItems[currentDevice++]->getState()->SetValue((float)0);
			currentDevice++;
		}*/
	}
	
	// call father update
	ParentClassType::UpdateDevice();
}

void	MultiTouchAndroid::DoInputDeviceDescription()
{

	myDeviceItemsCount=3 * myMaxTouch; // (posx + posy + state) * TouchCount

	DeviceItem**	devicearray=new DeviceItem*[myDeviceItemsCount];
	
	int currentDevice=0;

	for (int i = 0; i < myMaxTouch; i++)
	{
		devicearray[currentDevice++] = new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0)));
		devicearray[currentDevice++] = new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0)));
		devicearray[currentDevice++] = new DeviceItem(DeviceItemState<int>(0));
	}

	InitItems(myDeviceItemsCount,devicearray);

	for(currentDevice=0;currentDevice<myDeviceItemsCount;currentDevice++)
	{
		delete devicearray[currentDevice];
	}

	delete[] devicearray;

}

DEFINE_METHOD(MultiTouchAndroid, ReinitCB)
{
	printf("reinit MultiTouchAndroid\n");
	
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	for (int i=0;i<myMaxTouch;i++) 
		g_env->DeleteGlobalRef(myTouchList[i]);
	
	jclass  pMaClasse = g_env->FindClass("com/kigs/kigsmain/KigsGLSurfaceView");	
	jmethodID getMaxTouch = g_env->GetStaticMethodID(pMaClasse, "getMaxTouch", "()I");
	myMaxTouch = g_env->CallStaticIntMethod(pMaClasse, getMaxTouch);
	myTouchList.resize(myMaxTouch);

	for (int i = 0; i < myMaxTouch; i++)
	{
		jmethodID getTouchClass = g_env->GetStaticMethodID(pMaClasse, "getTouchList", "(I)Lcom/kigs/input/KigsTouchEventList;");
		jobject pobjet = g_env->CallStaticObjectMethod(pMaClasse, getTouchClass, i);
		myTouchList[i] = g_env->NewGlobalRef(pobjet);
	}
	
	return false;
}

