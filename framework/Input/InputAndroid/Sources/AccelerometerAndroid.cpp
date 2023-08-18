#include "AccelerometerAndroid.h"
#include "Core.h"
#include "DeviceItem.h"

using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(AccelerometerAndroid)

AccelerometerAndroid::AccelerometerAndroid(const std::string& name, CLASS_NAME_TREE_ARG)
	: AccelerometerDevice(name, PASS_CLASS_NAME_TREE_ARG)
	, mIsRunning(false)
{
// TODO
/*
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jclass pMaClasse = g_env->FindClass("com/kigs/input/KigsAccelerometer");
	mKigsAccelerometer = (jclass)g_env->NewGlobalRef(pMaClasse);

	// check if supported
	jmethodID method = g_env->GetStaticMethodID(mKigsAccelerometer, "isSupported", "()Z");
	mIsAvailable = g_env->CallStaticBooleanMethod(mKigsAccelerometer, method);

	jmethodID methodwc = g_env->GetStaticMethodID(mKigsAccelerometer, "isWorldCoordinatesSupported", "()Z");
	mIsWCAvailable = g_env->CallStaticBooleanMethod(mKigsAccelerometer, methodwc);

	if (mIsAvailable)
	{
		mStopMethod = g_env->GetStaticMethodID(mKigsAccelerometer, "stopListening", "(Z)V");
		mStartMethod = g_env->GetStaticMethodID(mKigsAccelerometer, "startListening", "(I)V");

		mGetValue = g_env->GetStaticMethodID(mKigsAccelerometer, "getValue", "()[B");
	}*/
}

AccelerometerAndroid::~AccelerometerAndroid()
{
	/*JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	g_env->DeleteGlobalRef(mKigsAccelerometer);*/
	mKigsAccelerometer = 0;
}

bool	AccelerometerAndroid::Aquire()
{
	/*if (AccelerometerDevice::Aquire())
	{
		Start();
		return true;
	}*/
	return false;
}

bool	AccelerometerAndroid::Release()
{
	/*if (AccelerometerDevice::Release())
	{
		Stop();
		return true;
	}*/
	return false;
}


void	AccelerometerAndroid::Start()
{
	/*if (mIsAvailable)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		g_env->CallStaticVoidMethod(mKigsAccelerometer, mStartMethod, (int)mRate);
		mIsRunning = true;
	}*/
}

void	AccelerometerAndroid::Stop()
{
	/*if (mIsAvailable)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		g_env->CallStaticVoidMethod(mKigsAccelerometer, mStopMethod, false);
		mIsRunning = false;
	}*/
}


void	AccelerometerAndroid::UpdateDevice()
{
	if (!mIsAvailable)
		return;
	if (!mIsRunning)
		return;

/*
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jbyteArray  arr = (jbyteArray)g_env->CallStaticObjectMethod(mKigsAccelerometer, mGetValue);
	jbyte *body = g_env->GetByteArrayElements(arr, 0);
	float* val = (float*)body;
	mRawAcceleration[0] = val[0];
	mRawAcceleration[1] = val[1];
	mRawAcceleration[2] = val[2];

	mAcceleration[0] = val[3];
	mAcceleration[1] = val[4];
	mAcceleration[2] = val[5];

	mVelocity[0] = val[6];
	mVelocity[1] = val[7];
	mVelocity[2] = val[8];

	mPosition[0] = val[9];
	mPosition[1] = val[10];
	mPosition[2] = val[11];

	g_env->ReleaseByteArrayElements(arr, body, 0);
*/
}

void	AccelerometerAndroid::DoInputDeviceDescription()
{
	AccelerometerDevice::InitModifiable();
	if (!mIsAvailable)
	{
		UninitModifiable();
	}
}