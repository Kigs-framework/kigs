#include "MouseAndroid.h"
#include "Core.h"
#include "DeviceItem.h"
#include "NotificationCenter.h"
#include "MultiTouchDevice.h"

IMPLEMENT_CLASS_INFO(MouseAndroid)

IMPLEMENT_CONSTRUCTOR(MouseAndroid)
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	//printf("retreive KigsGLSurfaceView \n");
	jclass  pMaClasse =g_env->FindClass("com/kigs/kigsmain/KigsGLSurfaceView");
	
	//printf("retreive getTouchList method \n");
	jmethodID getTouchClass=g_env->GetStaticMethodID(pMaClasse, "getTouchList", "(I)Lcom/kigs/input/KigsTouchEventList;");
	
	//printf("retreive KigsTouchEventList object \n");
	jobject pobjet = g_env->CallStaticObjectMethod(pMaClasse,getTouchClass,0);
	
	//printf("create global ref \n");
	myTouchList=g_env->NewGlobalRef(pobjet);
	
	pMaClasse =g_env->FindClass("com/kigs/input/KigsTouchEventList");

	//printf("retreive  KigsTouchEventList methods\n");
	getEventCount=g_env->GetMethodID(pMaClasse, "getEventCount", "()I");
	getEvent=g_env->GetMethodID(pMaClasse, "getEvent", "(I)Lcom/kigs/input/KigsTouchEvent;");
	clearEventList=g_env->GetMethodID(pMaClasse, "clearEventList", "()V");

	pMaClasse =g_env->FindClass("com/kigs/input/KigsTouchEvent");
	Event_getX = g_env->GetMethodID(pMaClasse, "getX", "()F");
	Event_getY = g_env->GetMethodID(pMaClasse, "getY", "()F");
	Event_getAction = g_env->GetMethodID(pMaClasse, "getAction", "()I");
	
	//printf("ok, that's it\n");
	myPosX = 0;
	myPosY = 0;	
   	m_FrameCountSinceLastValidTouched = 1000; //Any big value
	
	KigsCore::GetNotificationCenter()->addObserver(this, "ReinitCB", "ResetContext");
}

MouseAndroid::~MouseAndroid()
{  
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	g_env->DeleteGlobalRef(myTouchList);
}    

bool	MouseAndroid::Aquire()
{
	if (MouseDevice::Aquire())
	{
		myPosX = 0;
		myPosY = 0;
		m_FrameCountSinceLastValidTouched = 1000; //Any big value
		return true;
	}
	return false;
}

bool	MouseAndroid::Release()
{
	return MouseDevice::Release();
}

void	MouseAndroid::UpdateDevice()
{
	if (mMultiTouch)
	{
		auto state = mMultiTouch->getTouchState(0);
		float x, y;
		mMultiTouch->getTouchPos(0, x, y);
		myDeviceItems[0]->getState()->SetValue((float)((int)x - (int)(kfloat)myPosX));
		myDeviceItems[1]->getState()->SetValue((float)((int)y - (int)(kfloat)myPosY));
		myDeviceItems[2]->getState()->SetValue(state);
	}
	else
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		int event_Count=g_env->CallIntMethod(myTouchList,getEventCount);
		int currentDevice=0;
	
		if(event_Count)
		{
			int i;
		
			// manage event
			for(i=0;i<event_Count;i++)
			{
				jobject event=g_env->CallObjectMethod(myTouchList,getEvent,i);
			
				int action=(int)g_env->CallIntMethod(event,Event_getAction);
				float x=(float)g_env->CallFloatMethod(event,Event_getX);
				float y=(float)g_env->CallFloatMethod(event,Event_getY);
				currentDevice=0;
			
				if(action == 1) // released
				{
					myDeviceItems[currentDevice++]->getState()->SetValue((float)((int)x-(int)(kfloat)myPosX));
					myDeviceItems[currentDevice++]->getState()->SetValue((float)((int)y - (int)(kfloat)myPosY));
					myDeviceItems[currentDevice++]->getState()->SetValue((int)0x00);
				}
				else
				{	
					myDeviceItems[currentDevice++]->getState()->SetValue((float)(int)x - (int)(kfloat)myPosX);
					myDeviceItems[currentDevice++]->getState()->SetValue((float)(int)y - (int)(kfloat)myPosY);
					myDeviceItems[currentDevice++]->getState()->SetValue((int)0x80);
				}
			}
			
			g_env->CallVoidMethod(myTouchList,clearEventList);
		}
		else // no event = no move
		{
			myDeviceItems[currentDevice++]->getState()->SetValue((float)0);
			myDeviceItems[currentDevice++]->getState()->SetValue((float)0);
		}
	}
	// call father update
	MouseDevice::UpdateDevice();
}

void	MouseAndroid::DoInputDeviceDescription()
{
	mMultiTouch = (MultiTouchDevice*)mMultiTouchRef;

	myButtonsCount=1;
	myDeviceItemsCount=3; // 2 for posx and posy

	DeviceItem**	devicearray=new DeviceItem*[myDeviceItemsCount];
	
	int currentDevice=0;

	devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(0.0f));
	devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(0.0f));

	int currentButton;
	for(currentButton=0;currentButton<myButtonsCount;currentButton++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<int>(0));
	}

	InitItems(myDeviceItemsCount,devicearray);

	for(currentDevice=0;currentDevice<myDeviceItemsCount;currentDevice++)
	{
		delete devicearray[currentDevice];
	}

	delete[] devicearray;

}

DEFINE_METHOD(MouseAndroid, ReinitCB)
{
	printf("reinit MouseAndroid\n");
	if (mMultiTouch) return false;
	
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	g_env->DeleteGlobalRef(myTouchList);
	
	//printf("retreive KigsGLSurfaceView \n");
	jclass  pMaClasse =g_env->FindClass("com/kigs/kigsmain/KigsGLSurfaceView");
	//printf("retreive getTouchList method \n");
	jmethodID getTouchClass=g_env->GetStaticMethodID(pMaClasse, "getTouchList", "(I)Lcom/kigs/input/KigsTouchEventList;");
	//printf("retreive KigsTouchEventList object \n");
	jobject pobjet = g_env->CallStaticObjectMethod(pMaClasse,getTouchClass,0);	
	//printf("create global ref \n");
	myTouchList=g_env->NewGlobalRef(pobjet);
	return false;
}
