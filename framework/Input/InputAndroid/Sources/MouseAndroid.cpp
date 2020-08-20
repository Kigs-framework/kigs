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
	mTouchList=g_env->NewGlobalRef(pobjet);
	
	pMaClasse =g_env->FindClass("com/kigs/input/KigsTouchEventList");

	//printf("retreive  KigsTouchEventList methods\n");
	mGetEventCount=g_env->GetMethodID(pMaClasse, "getEventCount", "()I");
	mGetEvent=g_env->GetMethodID(pMaClasse, "getEvent", "(I)Lcom/kigs/input/KigsTouchEvent;");
	mClearEventList=g_env->GetMethodID(pMaClasse, "clearEventList", "()V");

	pMaClasse =g_env->FindClass("com/kigs/input/KigsTouchEvent");
	mEventGetX = g_env->GetMethodID(pMaClasse, "getX", "()F");
	mEventGetY = g_env->GetMethodID(pMaClasse, "getY", "()F");
	mEventGetAction = g_env->GetMethodID(pMaClasse, "getAction", "()I");
	
	//printf("ok, that's it\n");
	mPosX = 0;
	mPosY = 0;	
   	mFrameCountSinceLastValidTouched = 1000; //Any big value
	
	KigsCore::GetNotificationCenter()->addObserver(this, "ReinitCB", "ResetContext");
}

MouseAndroid::~MouseAndroid()
{  
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	g_env->DeleteGlobalRef(mTouchList);
}    

bool	MouseAndroid::Aquire()
{
	if (MouseDevice::Aquire())
	{
		mPosX = 0;
		mPosY = 0;
		mFrameCountSinceLastValidTouched = 1000; //Any big value
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
	if (mMultiTouchPointer)
	{
		auto state = mMultiTouchPointer->getTouchState(0);
		float x, y;
		mMultiTouchPointer->getTouchPos(0, x, y);
		mDeviceItems[0]->getState()->SetValue((float)((int)x - (int)(kfloat)mPosX));
		mDeviceItems[1]->getState()->SetValue((float)((int)y - (int)(kfloat)mPosY));
		mDeviceItems[2]->getState()->SetValue(state);
	}
	else
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		int event_Count=g_env->CallIntMethod(mTouchList,mGetEventCount);
		int currentDevice=0;
	
		if(event_Count)
		{
			int i;
		
			// manage event
			for(i=0;i<event_Count;i++)
			{
				jobject event=g_env->CallObjectMethod(mTouchList,mGetEvent,i);
			
				int action=(int)g_env->CallIntMethod(event,mEventGetAction);
				float x=(float)g_env->CallFloatMethod(event,mEventGetX);
				float y=(float)g_env->CallFloatMethod(event,mEventGetY);
				currentDevice=0;
			
				if(action == 1) // released
				{
					mDeviceItems[currentDevice++]->getState()->SetValue((float)((int)x-(int)(kfloat)mPosX));
					mDeviceItems[currentDevice++]->getState()->SetValue((float)((int)y - (int)(kfloat)mPosY));
					mDeviceItems[currentDevice++]->getState()->SetValue((int)0x00);
				}
				else
				{	
					mDeviceItems[currentDevice++]->getState()->SetValue((float)(int)x - (int)(kfloat)mPosX);
					mDeviceItems[currentDevice++]->getState()->SetValue((float)(int)y - (int)(kfloat)mPosY);
					mDeviceItems[currentDevice++]->getState()->SetValue((int)0x80);
				}
			}
			
			g_env->CallVoidMethod(mTouchList,mClearEventList);
		}
		else // no event = no move
		{
			mDeviceItems[currentDevice++]->getState()->SetValue((float)0);
			mDeviceItems[currentDevice++]->getState()->SetValue((float)0);
		}
	}
	// call father update
	MouseDevice::UpdateDevice();
}

void	MouseAndroid::DoInputDeviceDescription()
{
	mMultiTouchPointer = (MultiTouchDevice*)mMultiTouch;

	mButtonsCount=1;
	mDeviceItemsCount=3; // 2 for posx and posy

	DeviceItem**	devicearray=new DeviceItem*[mDeviceItemsCount];
	
	int currentDevice=0;

	devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(0.0f));
	devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(0.0f));

	int currentButton;
	for(currentButton=0;currentButton<mButtonsCount;currentButton++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<int>(0));
	}

	InitItems(mDeviceItemsCount,devicearray);

	for(currentDevice=0;currentDevice<mDeviceItemsCount;currentDevice++)
	{
		delete devicearray[currentDevice];
	}

	delete[] devicearray;

}

DEFINE_METHOD(MouseAndroid, ReinitCB)
{
	printf("reinit MouseAndroid\n");
	if (mMultiTouchPointer) return false;
	
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	g_env->DeleteGlobalRef(mTouchList);
	
	//printf("retreive KigsGLSurfaceView \n");
	jclass  pMaClasse =g_env->FindClass("com/kigs/kigsmain/KigsGLSurfaceView");
	//printf("retreive getTouchList method \n");
	jmethodID getTouchClass=g_env->GetStaticMethodID(pMaClasse, "getTouchList", "(I)Lcom/kigs/input/KigsTouchEventList;");
	//printf("retreive KigsTouchEventList object \n");
	jobject pobjet = g_env->CallStaticObjectMethod(pMaClasse,getTouchClass,0);	
	//printf("create global ref \n");
	mTouchList=g_env->NewGlobalRef(pobjet);
	return false;
}
