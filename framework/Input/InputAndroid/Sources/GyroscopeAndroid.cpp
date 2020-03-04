/*
 *  GyroscopeAndroid.cpp
 *  AndroidInputAndroid
 *
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "GyroscopeAndroid.h"
#include "Core.h"
#include "DeviceItem.h"


IMPLEMENT_CLASS_INFO(GyroscopeAndroid)

GyroscopeAndroid::GyroscopeAndroid(const kstl::string& name, CLASS_NAME_TREE_ARG) : GyroscopeDevice(name, PASS_CLASS_NAME_TREE_ARG)
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jclass pMaClasse = g_env->FindClass("com/assoria/input/KigsGyroscope");
	myKigsGyroscope = (jclass)g_env->NewGlobalRef(pMaClasse);

	// check if supported
	jmethodID method = g_env->GetStaticMethodID(myKigsGyroscope, "isSupported", "()Z");
	isAvailable = g_env->CallStaticBooleanMethod(myKigsGyroscope, method);

	if (isAvailable)
	{
		StopMethod = g_env->GetStaticMethodID(myKigsGyroscope, "stopListening", "(Z)V");
		StartMethod = g_env->GetStaticMethodID(myKigsGyroscope, "startListening", "(I)V");

		getVelocity = g_env->GetStaticMethodID(myKigsGyroscope, "getVelocity", "()[B");
		getQuat = g_env->GetStaticMethodID(myKigsGyroscope, "getQuaternion", "()[B");
	}
}

GyroscopeAndroid::~GyroscopeAndroid()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	g_env->DeleteGlobalRef(myKigsGyroscope);
	myKigsGyroscope = 0;
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
	if (isAvailable)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		g_env->CallStaticVoidMethod(myKigsGyroscope, StartMethod, (int)myRate);
		isRunning = true;
	}
}

void	GyroscopeAndroid::Stop()
{
	if (isAvailable)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		g_env->CallStaticVoidMethod(myKigsGyroscope, StopMethod, false);
		isRunning = false;
	}
}
void	GyroscopeAndroid::UpdateDevice()
{
	if (!isAvailable)
		return;
	if (!isRunning)
		return;

	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jbyteArray  arr = (jbyteArray)g_env->CallStaticObjectMethod(myKigsGyroscope, getVelocity);
	jbyte *body = g_env->GetByteArrayElements(arr, 0);

	myRotationVelocity = *reinterpret_cast<Point3D*>(body);
	g_env->ReleaseByteArrayElements(arr, body, 0);

	arr = (jbyteArray)g_env->CallStaticObjectMethod(myKigsGyroscope, getQuat);
	body = g_env->GetByteArrayElements(arr, 0);
	
	myQuaternion = *reinterpret_cast<Vector4D*>(body);

	g_env->ReleaseByteArrayElements(arr, body, 0);
}

void	GyroscopeAndroid::DoInputDeviceDescription()
{
	GyroscopeDevice::InitModifiable();
	if (!isAvailable)
	{
		UninitModifiable();
	}
}