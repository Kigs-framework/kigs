#include "KeyboardAndroid.h"
#include "Core.h"
#include "DeviceItem.h"
#include "NotificationCenter.h"

using namespace Kigs::Input;

KeyboardAndroid*	CurrentKeyboard = 0;

// only those function are visibles from outside of the dll
#define KIGS_JNIEXPORT	__attribute__((visibility("default")))


extern "C" {

	KIGS_JNIEXPORT void JNICALL Java_com_kigs_input_KeyboardAndroid_PushKeyEvent(JNIEnv * env, jobject obj, jobject buff)
	{

		jbyte* bbuf_in = (jbyte*)env->GetDirectBufferAddress(buff);

		if (CurrentKeyboard)
			CurrentKeyboard->PushEvent(bbuf_in, 0);
	}

};


IMPLEMENT_CLASS_INFO(KeyboardAndroid)

IMPLEMENT_CONSTRUCTOR(KeyboardAndroid)
{
	/*JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	mJKeyboard = KigsJavaIDManager::RegisterClass(g_env, "com/kigs/input/KigsKeyboard");

	mJGetActions = g_env->GetStaticMethodID(mJKeyboard, "GetKeyActions", "()[B");
	mJClear = g_env->GetStaticMethodID(mJKeyboard, "Clear", "()V");
*/
	CurrentKeyboard = this;
}



void	KeyboardAndroid::PushEvent(void* data, int count)
{
	KeyEvent * EventList = (KeyEvent*)data;
	for (int i = 0; i < count; i++)
	{
		mReceivedEvent.push_back(EventList[i]);
	}
}

void	KeyboardAndroid::UpdateDevice()
{
	mKeyUpList.clear();
	mKeyDownList.clear();
	std::vector<KeyEvent>	touchVector;

	/*JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	// get up event 
	jbyteArray  arr = (jbyteArray)g_env->CallStaticObjectMethod(mJKeyboard, mJGetActions);
	//jsize len = g_env->GetArrayLength(arr);
	jbyte *body = g_env->GetByteArrayElements(arr, 0);

	unsigned char eventCount = ((unsigned char*)body)[0];

	if (eventCount > 0)
	{
		KeyEvent * EventList = (KeyEvent*)(body + 4);
		for (int i = 0; i < eventCount; i++)
		{
			switch (EventList[i].Action)
			{
			case KeyEvent::ACTION_UP:
			{
				mKeyUpList.push_back(EventList[i]);
				break;
			}
			case KeyEvent::ACTION_DOWN:
			{
				mKeyDownList.push_back(EventList[i]);
				break;
			}
			}
			mDeviceItems[EventList[i].KeyCode]->getState()->SetValue(EventList[i].Action);
			touchVector.push_back(EventList[i]);
		}

	}

	g_env->ReleaseByteArrayElements(arr, body, 0);

	if (!touchVector.empty())
	{
		EmitSignal(Signals::KeyboardEvent, touchVector);
	}

	g_env->CallStaticVoidMethod(mJKeyboard, mJClear);*/
}

void	KeyboardAndroid::DoInputDeviceDescription()
{
	mDeviceItemsCount = 256;

	DeviceItem**	devicearray = new DeviceItem*[mDeviceItemsCount];

	unsigned int currentDevice = 0;

	unsigned int index;
	for (index = 0; index < mDeviceItemsCount; index++)
	{
		devicearray[currentDevice++] = new DeviceItem(DeviceItemState<int>(0));
	}

	InitItems(mDeviceItemsCount, devicearray);
	for (index = 0; index < mDeviceItemsCount; index++)
	{
		delete devicearray[index];
	}

	delete[] devicearray;
}


void KeyboardAndroid::Show()
{
	/*JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	jclass lMaClasse = g_env->FindClass("com/kigs/input/KigsKeyboard");

	jmethodID ShowMethod = g_env->GetStaticMethodID(lMaClasse, "ShowKeyboard", "(Z)V");
	g_env->CallStaticVoidMethod(lMaClasse, ShowMethod, true);

	g_env->DeleteLocalRef(lMaClasse);*/
};

void KeyboardAndroid::Hide()
{
	/*JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	jclass lMaClasse = g_env->FindClass("com/kigs/input/KigsKeyboard");

	jmethodID ShowMethod = g_env->GetStaticMethodID(lMaClasse, "ShowKeyboard", "(Z)V");
	g_env->CallStaticVoidMethod(lMaClasse, ShowMethod, false);

	g_env->DeleteLocalRef(lMaClasse);*/
};

DEFINE_METHOD(KeyboardAndroid, ReinitCB)
{
	printf("reinit KeyboardAndroid\n");

	return false;
}