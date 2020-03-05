/*
 *  CompassAndroid.cpp
 *  AndroidInputAndroid
 *
 *  Created by steph on 4/4/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "CompassAndroid.h"
#include "Core.h"
#include "DeviceItem.h"


IMPLEMENT_CLASS_INFO(CompassAndroid)

CompassAndroid::CompassAndroid(const kstl::string& name, CLASS_NAME_TREE_ARG)
	: CompassDevice(name, PASS_CLASS_NAME_TREE_ARG)
	, isRunning(false)
{
	myCompX = 0.0f;
	myCompY = 0.0f;
	myCompZ = 0.0f;

	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jclass pMaClasse = g_env->FindClass("com/kigs/input/KigsCompass");
	myKigsCompass = (jclass)g_env->NewGlobalRef(pMaClasse);

	// check if supported
	jmethodID method = g_env->GetStaticMethodID(myKigsCompass, "isSupported", "()Z");
	isAvailable = g_env->CallStaticBooleanMethod(myKigsCompass, method);

	if (isAvailable)
	{
		StopMethod = g_env->GetStaticMethodID(myKigsCompass, "stopListening", "(Z)V");
		StartMethod = g_env->GetStaticMethodID(myKigsCompass, "startListening", "(I)V");

		getValue = g_env->GetStaticMethodID(myKigsCompass, "getValue", "()[B");
	}
}

CompassAndroid::~CompassAndroid()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	g_env->DeleteGlobalRef(myKigsCompass);
	myKigsCompass = 0;
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
	if (isAvailable)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		g_env->CallStaticVoidMethod(myKigsCompass, StartMethod, (int)myRate);
		isRunning = true;
	}
}

void	CompassAndroid::Stop()
{
	if (isAvailable)
	{
		JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
		g_env->CallStaticVoidMethod(myKigsCompass, StopMethod, false);
		isRunning = false;
	}
}


void	CompassAndroid::UpdateDevice()
{
	if (!isAvailable)
		return;
	if (!isRunning)
		return;


	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());


	jbyteArray  arr = (jbyteArray)g_env->CallStaticObjectMethod(myKigsCompass, getValue);
	jbyte *body = g_env->GetByteArrayElements(arr, 0);
	float* val = (float*)body;
	float x = val[0];
	float y = val[1];
	float z = val[2];
	myCompX = x;
	myCompY = y;
	myCompZ = z;

	g_env->ReleaseByteArrayElements(arr, body, 0);

}

void	CompassAndroid::DoInputDeviceDescription()
{
	CompassDevice::InitModifiable(); 
	if (!isAvailable)
	{
		UninitModifiable();
	}
}