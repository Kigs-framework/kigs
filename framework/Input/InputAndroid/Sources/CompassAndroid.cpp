
#include "CompassAndroid.h"
#include "Core.h"
#include "DeviceItem.h"


IMPLEMENT_CLASS_INFO(CompassAndroid)

CompassAndroid::CompassAndroid(const std::string& name, CLASS_NAME_TREE_ARG)
	: CompassDevice(name, PASS_CLASS_NAME_TREE_ARG)
	, mIsRunning(false)
{
	mCompX = 0.0f;
	mCompY = 0.0f;
	mCompZ = 0.0f;

	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jclass pMaClasse = g_env->FindClass("com/kigs/input/KigsCompass");
	mKigsCompass = (jclass)g_env->NewGlobalRef(pMaClasse);

	// check if supported
	jmethodID method = g_env->GetStaticMethodID(mKigsCompass, "isSupported", "()Z");
	mIsAvailable = g_env->CallStaticBooleanMethod(mKigsCompass, method);

	if (mIsAvailable)
	{
		mStopMethod = g_env->GetStaticMethodID(mKigsCompass, "stopListening", "(Z)V");
		mStartMethod = g_env->GetStaticMethodID(mKigsCompass, "startListening", "(I)V");

		mGetValue = g_env->GetStaticMethodID(mKigsCompass, "getValue", "()[B");
	}
}

CompassAndroid::~CompassAndroid()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	g_env->DeleteGlobalRef(mKigsCompass);
	mKigsCompass = 0;
}

bool	CompassAndroid::Aquire()
{
	if (CompassDevice::Aquire())
	{
		Start();
		return true;
	}
	return false;
}

bool	CompassAndroid::Release()
{
	if (CompassDevice::Release())
	{
		Stop();
		return true;
	}
	return false;
}


void	CompassAndroid::Start()
{
	if (mIsAvailable)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		g_env->CallStaticVoidMethod(mKigsCompass, mStartMethod, (int)mRate);
		mIsRunning = true;
	}
}

void	CompassAndroid::Stop()
{
	if (mIsAvailable)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		g_env->CallStaticVoidMethod(mKigsCompass, mStopMethod, false);
		mIsRunning = false;
	}
}


void	CompassAndroid::UpdateDevice()
{
	if (!mIsAvailable)
		return;
	if (!mIsRunning)
		return;


	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());


	jbyteArray  arr = (jbyteArray)g_env->CallStaticObjectMethod(mKigsCompass, mGetValue);
	jbyte *body = g_env->GetByteArrayElements(arr, 0);
	float* val = (float*)body;
	float x = val[0];
	float y = val[1];
	float z = val[2];
	mCompX = x;
	mCompY = y;
	mCompZ = z;

	g_env->ReleaseByteArrayElements(arr, body, 0);

}

void	CompassAndroid::DoInputDeviceDescription()
{
	CompassDevice::InitModifiable(); 
	if (!mIsAvailable)
	{
		UninitModifiable();
	}
}