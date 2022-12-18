#include "GyroscopeAndroid.h"
#include "Core.h"
#include "DeviceItem.h"


IMPLEMENT_CLASS_INFO(GyroscopeAndroid)

GyroscopeAndroid::GyroscopeAndroid(const std::string& name, CLASS_NAME_TREE_ARG) : GyroscopeDevice(name, PASS_CLASS_NAME_TREE_ARG)
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jclass pMaClasse = g_env->FindClass("com/kigs/input/KigsGyroscope");
	mKigsGyroscope = (jclass)g_env->NewGlobalRef(pMaClasse);

	// check if supported
	jmethodID method = g_env->GetStaticMethodID(mKigsGyroscope, "isSupported", "()Z");
	mIsAvailable = g_env->CallStaticBooleanMethod(mKigsGyroscope, method);

	if (mIsAvailable)
	{
		mStopMethod = g_env->GetStaticMethodID(mKigsGyroscope, "stopListening", "(Z)V");
		mStartMethod = g_env->GetStaticMethodID(mKigsGyroscope, "startListening", "(I)V");

		mGetVelocity = g_env->GetStaticMethodID(mKigsGyroscope, "getVelocity", "()[B");
		mGetQuat = g_env->GetStaticMethodID(mKigsGyroscope, "getQuaternion", "()[B");
	}
}

GyroscopeAndroid::~GyroscopeAndroid()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	g_env->DeleteGlobalRef(mKigsGyroscope);
	mKigsGyroscope = 0;
}

bool	GyroscopeAndroid::Aquire()
{
	if (GyroscopeDevice::Aquire())
	{
		Start();
		return true;
	}
	return false;
}

bool	GyroscopeAndroid::Release()
{
	if (GyroscopeDevice::Release())
	{
		Stop();
		return true;
	}
	return false;
}

void	GyroscopeAndroid::Start()
{
	if (mIsAvailable)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		g_env->CallStaticVoidMethod(mKigsGyroscope, mStartMethod, (int)mRate);
		mIsRunning = true;
	}
}

void	GyroscopeAndroid::Stop()
{
	if (mIsAvailable)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		g_env->CallStaticVoidMethod(mKigsGyroscope, mStopMethod, false);
		mIsRunning = false;
	}
}
void	GyroscopeAndroid::UpdateDevice()
{
	if (!mIsAvailable)
		return;
	if (!mIsRunning)
		return;

	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jbyteArray  arr = (jbyteArray)g_env->CallStaticObjectMethod(mKigsGyroscope, mGetVelocity);
	jbyte *body = g_env->GetByteArrayElements(arr, 0);

	mRotationVelocity = *reinterpret_cast<Point3D*>(body);
	g_env->ReleaseByteArrayElements(arr, body, 0);

	arr = (jbyteArray)g_env->CallStaticObjectMethod(mKigsGyroscope, mGetQuat);
	body = g_env->GetByteArrayElements(arr, 0);
	
	mRotationQuaternion = *reinterpret_cast<Vector4D*>(body);

	g_env->ReleaseByteArrayElements(arr, body, 0);
}

void	GyroscopeAndroid::DoInputDeviceDescription()
{
	GyroscopeDevice::InitModifiable();
	if (!mIsAvailable)
	{
		UninitModifiable();
	}
}