#include "KeyboardAndroid.h"
#include "Core.h"
#include "DeviceItem.h"
#include "NotificationCenter.h"

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
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	myJKeyboard = KigsJavaIDManager::RegisterClass(g_env, "com/kigs/input/KigsKeyboard");

	JGetActions = g_env->GetStaticMethodID(myJKeyboard, "GetKeyActions", "()[B");
	JClear = g_env->GetStaticMethodID(myJKeyboard, "Clear", "()V");

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
	m_KeyUpList.clear();
	m_KeyDownList.clear();
	kstl::vector<KeyEvent>	touchVector;

	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	// get up event 
	jbyteArray  arr = (jbyteArray)g_env->CallStaticObjectMethod(myJKeyboard, JGetActions);
	//jsize len = g_env->GetArrayLength(arr);
	jbyte *body = g_env->GetByteArrayElements(arr, 0);
	/*for (int i = 0; i<len; i++)
	{
		int keycode = body[i];
		m_KeyUpList.push_back(keycode);

		maInt* value = new maInt(*this, false, LABEL_AND_ID(KeyCode), -keycode);
		touchVector.push_back(value);
	}*/

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
				m_KeyUpList.push_back(EventList[i]);
				break;
			}
			case KeyEvent::ACTION_DOWN:
			{
				m_KeyDownList.push_back(EventList[i]);
				break;
			}
			}
			myDeviceItems[EventList[i].KeyCode]->getState()->SetValue(EventList[i].Action);
			touchVector.push_back(EventList[i]);
		}

	}

	g_env->ReleaseByteArrayElements(arr, body, 0);

	if (!touchVector.empty())
	{
		EmitSignal(Signals::KeyboardEvent, touchVector);
	}

	g_env->CallStaticVoidMethod(myJKeyboard, JClear);
}

void	KeyboardAndroid::DoInputDeviceDescription()
{
	myDeviceItemsCount = 256;

	DeviceItem**	devicearray = new DeviceItem*[myDeviceItemsCount];

	unsigned int currentDevice = 0;

	unsigned int index;
	for (index = 0; index < myDeviceItemsCount; index++)
	{
		devicearray[currentDevice++] = new DeviceItem(DeviceItemState<int>(0));
	}

	InitItems(myDeviceItemsCount, devicearray);
	for (index = 0; index < myDeviceItemsCount; index++)
	{
		delete devicearray[index];
	}

	delete[] devicearray;
}


void KeyboardAndroid::Show()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	jclass lMaClasse = g_env->FindClass("com/kigs/input/KigsKeyboard");

	jmethodID ShowMethod = g_env->GetStaticMethodID(lMaClasse, "ShowKeyboard", "(Z)V");
	g_env->CallStaticVoidMethod(lMaClasse, ShowMethod, true);

	g_env->DeleteLocalRef(lMaClasse);
};

void KeyboardAndroid::Hide()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	jclass lMaClasse = g_env->FindClass("com/kigs/input/KigsKeyboard");

	jmethodID ShowMethod = g_env->GetStaticMethodID(lMaClasse, "ShowKeyboard", "(Z)V");
	g_env->CallStaticVoidMethod(lMaClasse, ShowMethod, false);

	g_env->DeleteLocalRef(lMaClasse);
};

DEFINE_METHOD(KeyboardAndroid, ReinitCB)
{
	printf("reinit KeyboardAndroid\n");

	return false;
}