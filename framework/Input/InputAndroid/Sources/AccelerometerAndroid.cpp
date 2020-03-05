/*
 *  AccelerometerAndroid.cpp
 *  AndroidInputAndroid
 *
 *  Created by steph on 4/4/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "AccelerometerAndroid.h"
#include "Core.h"
#include "DeviceItem.h"


IMPLEMENT_CLASS_INFO(AccelerometerAndroid)

AccelerometerAndroid::AccelerometerAndroid(const kstl::string& name, CLASS_NAME_TREE_ARG)
	: AccelerometerDevice(name, PASS_CLASS_NAME_TREE_ARG)
	, isRunning(false)
{

	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jclass pMaClasse = g_env->FindClass("com/kigs/input/KigsAccelerometer");
	myKigsAccelerometer = (jclass)g_env->NewGlobalRef(pMaClasse);

	// check if supported
	jmethodID method = g_env->GetStaticMethodID(myKigsAccelerometer, "isSupported", "()Z");
	isAvailable = g_env->CallStaticBooleanMethod(myKigsAccelerometer, method);

	jmethodID methodwc = g_env->GetStaticMethodID(myKigsAccelerometer, "isWorldCoordinatesSupported", "()Z");
	isWCAvailable = g_env->CallStaticBooleanMethod(myKigsAccelerometer, methodwc);

	if (isAvailable)
	{
		StopMethod = g_env->GetStaticMethodID(myKigsAccelerometer, "stopListening", "(Z)V");
		StartMethod = g_env->GetStaticMethodID(myKigsAccelerometer, "startListening", "(I)V");

		getValue = g_env->GetStaticMethodID(myKigsAccelerometer, "getValue", "()[B");
	}
}

AccelerometerAndroid::~AccelerometerAndroid()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	g_env->DeleteGlobalRef(myKigsAccelerometer);
	myKigsAccelerometer = 0;
}

bool	AccelerometerAndroid::Aquire()
{
	if (AccelerometerDevice::Aquire())
	{
		Start();
		return true;
	}
	return false;
}

bool	AccelerometerAndroid::Release()
{
	if (AccelerometerDevice::Release())
	{
		Stop();
		return true;
	}
	return false;
}


void	AccelerometerAndroid::Start()
{
	if (isAvailable)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		g_env->CallStaticVoidMethod(myKigsAccelerometer, StartMethod, (int)myRate);
		isRunning = true;
	}
}

void	AccelerometerAndroid::Stop()
{
	if (isAvailable)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		g_env->CallStaticVoidMethod(myKigsAccelerometer, StopMethod, false);
		isRunning = false;
	}
}


void	AccelerometerAndroid::UpdateDevice()
{
	if (!isAvailable)
		return;
	if (!isRunning)
		return;


	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jbyteArray  arr = (jbyteArray)g_env->CallStaticObjectMethod(myKigsAccelerometer, getValue);
	jbyte *body = g_env->GetByteArrayElements(arr, 0);
	float* val = (float*)body;
	myRawAccX = val[0];
	myRawAccY = val[1];
	myRawAccZ = val[2];

	myAccX = val[3];
	myAccY = val[4];
	myAccZ = val[5];

	myVelX = val[6];
	myVelY = val[7];
	myVelZ = val[8];

	myPosX = val[9];
	myPosY = val[10];
	myPosZ = val[11];

	g_env->ReleaseByteArrayElements(arr, body, 0);

}

void	AccelerometerAndroid::DoInputDeviceDescription()
{
	AccelerometerDevice::InitModifiable();
	if (!isAvailable)
	{
		UninitModifiable();
	}
}