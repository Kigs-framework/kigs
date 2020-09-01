
#include "GeolocationAndroid.h"
#include "Core.h"
#include "DeviceItem.h"


IMPLEMENT_CLASS_INFO(GeolocationAndroid)

GeolocationAndroid::GeolocationAndroid(const kstl::string& name,CLASS_NAME_TREE_ARG) : GeolocationDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
	mLatitude = 0.0;
	mLongitude = 0.0;
	mAltitude = 0.0;
	mAccuracy = 0.f;

	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jclass pMaClasse =g_env->FindClass("com/kigs/input/KigsGeolocation");
	mKigsGeolocation = (jclass)g_env->NewGlobalRef(pMaClasse);

	mGetLatitude=g_env->GetStaticMethodID(mKigsGeolocation, "getLatitude", "()D");
	mGetLongitude = g_env->GetStaticMethodID(mKigsGeolocation, "getLongitude", "()D");
	mGetAltitude = g_env->GetStaticMethodID(mKigsGeolocation, "getAltitude", "()D");
	mGetAccuracy = g_env->GetStaticMethodID(mKigsGeolocation, "getAccuracy", "()F");
	mGetActive = g_env->GetStaticMethodID(mKigsGeolocation, "isActive", "()Z");
	mSetActive=g_env->GetStaticMethodID(mKigsGeolocation, "Activate", "(ZIF)V");
}

GeolocationAndroid::~GeolocationAndroid()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	g_env->DeleteGlobalRef(mKigsGeolocation);
	mKigsGeolocation=0;
}

bool	GeolocationAndroid::Aquire()
{
	if (GeolocationDevice::Aquire())
	{
		Start();
		return true;
	}
	return false;
}

bool	GeolocationAndroid::Release()
{
	if (GeolocationDevice::Release())
	{
		Stop();
		return true;
	}
	return false;
}
 
void	GeolocationAndroid::UpdateDevice()
{
	if (mIsActive)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		mLatitude = (kdouble)g_env->CallStaticDoubleMethod(mKigsGeolocation, mGetLatitude);
		mLongitude = (kdouble)g_env->CallStaticDoubleMethod(mKigsGeolocation, mGetLongitude);
		mAltitude = (kdouble)g_env->CallStaticDoubleMethod(mKigsGeolocation, mGetAltitude);
		mAccuracy = (kfloat)g_env->CallStaticFloatMethod(mKigsGeolocation, mGetAccuracy);
	}
}

void	GeolocationAndroid::DoInputDeviceDescription()
{
	GeolocationDevice::InitModifiable();
}

void	GeolocationAndroid::Start()
{
	if (!mIsActive)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		g_env->CallStaticVoidMethod(mKigsGeolocation, mSetActive, true, (int)mRate, (kfloat)mMinDistance);
		mIsActive = true;
	}
}
void	GeolocationAndroid::Stop()
{
	if (mIsActive)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		g_env->CallStaticVoidMethod(mKigsGeolocation, mSetActive, false, (int)mRate, (kfloat)mMinDistance);
		mIsActive = false;
	}
}
