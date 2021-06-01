// ******************************************************************************
// *   DO NOT MODIFY THIS FILE													*
// *   THIS FILE IS A GENERIC FILE COMMON TO SEVERAL APPLICATION                *
// ******************************************************************************

//! this file must define KIGS_APPLICATION_CLASS with the name of your application class
#include "Platform\Core\KigsJavaIDManager.h"
#include "KigsApplication.h"
#include "TimerIncludes.h"

#include <GLES/gl.h>
#include <GLES/glext.h>

SP<CoreBaseApplication>	myApp=nullptr;
bool					gResumeReloadTexture=false;

#include <string.h>
#include <jni.h>
#ifdef _DEBUG
#include <android/log.h>
#endif

// only those function are visibles from outside of the dll
#define KIGS_JNIEXPORT	__attribute__((visibility("default")))

extern "C" {

	KIGS_JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved);
	// life cycle

	KIGS_JNIEXPORT void JNICALL Java_com_kigs_kigsmain_KigsMainManager_Init(JNIEnv * env, jobject obj);
	KIGS_JNIEXPORT void JNICALL Java_com_kigs_kigsmain_KigsMainManager_Update(JNIEnv * env, jobject obj);
	KIGS_JNIEXPORT void JNICALL Java_com_kigs_kigsmain_KigsMainManager_Close(JNIEnv * env, jobject obj);
	
	KIGS_JNIEXPORT void JNICALL Java_com_kigs_kigsmain_KigsMainManager_Pause(JNIEnv * env, jobject obj);
	KIGS_JNIEXPORT void JNICALL Java_com_kigs_kigsmain_KigsMainManager_Resume(JNIEnv * env, jobject obj,jboolean reloadTexture);
	
	KIGS_JNIEXPORT void JNICALL Java_com_kigs_kigsmain_KigsMainManager_GLResize(JNIEnv * env, jobject obj,jint w,jint h);
	
	KIGS_JNIEXPORT jboolean JNICALL Java_com_kigs_kigsmain_KigsMainManager_NeedExit(JNIEnv * env, jobject obj);
	
};

extern "C" void __cxa_pure_virtual() { while (1); }

MEMORYMANAGEMENT_START 
#ifndef _NO_MEMORY_MANAGER_

#include "Platform/Core/MemoryManager.h"

void* gMemoryBlock=0;
void MMManagerinitCallBack(void)
{
	// allocate 8 Mo
	gMemoryBlock=malloc(MEMSIZE_FOR_MEMORY_MANAGER);
	
	// alloc more prealloc
	the_mem_manager.MMInitAll( gMemoryBlock, MEMSIZE_FOR_MEMORY_MANAGER,1024*1024,16,2048 );

	the_mem_manager.MMSetPrintFunction( printf );

}
#endif 
MEMORYMANAGEMENT_END

#define ApplicationName(a) Stringify(a)
#define Stringify(a) #a


KIGS_JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
MEMORYMANAGEMENT_START 
#ifndef _NO_MEMORY_MANAGER_
	MMManagerinitCallBack();
#endif 
MEMORYMANAGEMENT_END

	// init JavaIDManager
	if(KigsJavaIDManager::init(vm) == false)
	{
		KIGS_ERROR("KigsJavaIDManager was not init correctly",2); 
		return 0;
	}
	
	#ifdef _DEBUG
	#if (CURRENT_BUILD_ARCHITECTURE == 7)
	printf("DLL Loaded for processor ARMV7\n");
	#elif (CURRENT_BUILD_ARCHITECTURE == 5)
	printf("DLL Loaded for processor ARMV5\n");
	#else
	//printf("DLL Loaded unknown architecture\n");
	#endif
	#endif
	
	//! First thing to do
	KigsCore::Init();

	return JNI_VERSION_1_4;
}

KIGS_JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved)
{

	//! last thing to do
	KigsCore::Close();
	
	KigsJavaIDManager::close();

	MEMORYMANAGEMENT_START 
#ifndef _NO_MEMORY_MANAGER_
	free(gMemoryBlock);
#endif 
	MEMORYMANAGEMENT_END
}


KIGS_JNIEXPORT void JNICALL Java_com_kigs_kigsmain_KigsMainManager_Init(JNIEnv * env, jobject obj)
{
	if(myApp)
	{
		//printf("myApp already Init\n");
		return;
	}


	// no need to register app to factory
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(KIGS_APPLICATION_CLASS, ApplicationName(KIGS_APPLICATION_CLASS));
	myApp = KIGS_APPLICATION_CLASS::CreateInstance(ApplicationName(KIGS_APPLICATION_CLASS));
	
	#ifdef INIT_DEFAULT_MODULES
	#ifdef BASE_DATA_PATH
			//! then init
			myApp->InitApp(BASE_DATA_PATH,true);
	#else
			//! then init
			myApp->InitApp(0,true);
	#endif //BASE_DATA_PATH

	#else
		//! then init
		myApp->Init(0,false);

	#endif //INIT_DEFAULT_MODULES
	
}
 
 
KIGS_JNIEXPORT void JNICALL Java_com_kigs_kigsmain_KigsMainManager_Update(JNIEnv * env, jobject obj)
{
	if(!myApp)
	{
		//printf("myApp not init in update call\n");
		return;
	}
	
	myApp->UpdateApp();

}

KIGS_JNIEXPORT void JNICALL Java_com_kigs_kigsmain_KigsMainManager_Close(JNIEnv * env, jobject obj)
{
	
	if(myApp)
	{
		//! close
		myApp->CloseApp();
		myApp=nullptr;
	}

}

KIGS_JNIEXPORT jboolean JNICALL Java_com_kigs_kigsmain_KigsMainManager_NeedExit(JNIEnv * env, jobject obj)
{
	if(myApp)
	{
		return (jboolean)myApp->NeedExit();
	}
	return false;
}

KIGS_JNIEXPORT void JNICALL Java_com_kigs_kigsmain_KigsMainManager_Pause(JNIEnv * env, jobject obj)
{
	
	if(myApp)
	{
		
		myApp->Sleep();
	}
	
}

KIGS_JNIEXPORT void JNICALL Java_com_kigs_kigsmain_KigsMainManager_Resume(JNIEnv * env, jobject obj,jboolean reloadTexture)
{
	if(myApp)
	{
		gResumeReloadTexture=reloadTexture;
		myApp->Resume();
		gResumeReloadTexture=false;
	}
}

	
KIGS_JNIEXPORT void JNICALL Java_com_kigs_kigsmain_KigsMainManager_GLResize(JNIEnv * env, jobject obj,jint w,jint h)
{
	// init graphics
	/*glShadeModel(GL_SMOOTH);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	
	glEnable(GL_TEXTURE_2D);
	
	glViewport(0, 0, w, h);*/
}


