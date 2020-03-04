/*
 *  GeolocationAndroid.cpp
 *  AndroidInputAndroid
 *
 *  Created by Antoine on 22/01/15.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

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

	jclass pMaClasse =g_env->FindClass("com/assoria/input/KigsGeolocation");
	myKigsGeolocation = (jclass)g_env->NewGlobalRef(pMaClasse);

	getLatitude=g_env->GetStaticMethodID(myKigsGeolocation, "getLatitude", "()D");
	getLongitude = g_env->GetStaticMethodID(myKigsGeolocation, "getLongitude", "()D");
	getAltitude = g_env->GetStaticMethodID(myKigsGeolocation, "getAltitude", "()D");
	getAccuracy = g_env->GetStaticMethodID(myKigsGeolocation, "getAccuracy", "()F");
	isActive = g_env->GetStaticMethodID(myKigsGeolocation, "isActive", "()Z");
	setActive=g_env->GetStaticMethodID(myKigsGeolocation, "Activate", "(ZIF)V");
}

GeolocationAndroid::~GeolocationAndroid()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	g_env->DeleteGlobalRef(myKigsGeolocation);
	myKigsGeolocation=0;
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
		mLatitude = (kdouble)g_env->CallStaticDoubleMethod(myKigsGeolocation, getLatitude);
		mLongitude = (kdouble)g_env->CallStaticDoubleMethod(myKigsGeolocation, getLongitude);
		mAltitude = (kdouble)g_env->CallStaticDoubleMethod(myKigsGeolocation, getAltitude);
		mAccuracy = (kfloat)g_env->CallStaticFloatMethod(myKigsGeolocation, getAccuracy);
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
		g_env->CallStaticVoidMethod(myKigsGeolocation, setActive, true, (int)myRate, (kfloat)myMinDistance);
		mIsActive = true;
	}
}
void	GeolocationAndroid::Stop()
{
	if (mIsActive)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		g_env->CallStaticVoidMethod(myKigsGeolocation, setActive, false, (int)myRate, (kfloat)myMinDistance);
		mIsActive = false;
	}
}
