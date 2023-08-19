// ******************************************************************************
// *   DO NOT MODIFY THIS FILE													*
// *   THIS FILE IS A GENERIC FILE COMMON TO SEVERAL APPLICATION                *
// ******************************************************************************

//! this file must define KIGS_APPLICATION_CLASS with the name of your application class
#include "KigsApplication.h"
#include "TimerIncludes.h"

#include <GLES/gl.h>
#include <GLES/glext.h>

using namespace Kigs;
using namespace Kigs::Core;


SP<CoreBaseApplication>	mApp=nullptr;
bool					gResumeReloadTexture=false;

#include <string.h>
#include <jni.h>
#ifdef _DEBUG
#include <android/log.h>
#endif

#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>

// only those function are visibles from outside of the dll
#define KIGS_JNIEXPORT	__attribute__((visibility("default")))


#define ApplicationName(a) Stringify(a)
#define Stringify(a) #a

extern "C" {

#include <game-activity/native_app_glue/android_native_app_glue.c>

android_app* 			mAndroidApp=nullptr;

/*!
 * Handles commands sent to this Android application
 * @param pApp the app the commands are coming from
 * @param cmd the command to handle
 */
void handle_cmd(android_app *pApp, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            // A new window is created, associate a renderer with it. You may replace this with a
            // "game" class if that suits your needs. Remember to change all instances of userData
            // if you change the class here as a reinterpret_cast is dangerous this in the
            // android_main function and the APP_CMD_TERM_WINDOW handler case.
			
			if(!mApp)
			{
				
				mAndroidApp=pApp;
				
				//! First thing to do
				KigsCore::Init();
				// no need to register app to factory
				DECLARE_CLASS_INFO_WITHOUT_FACTORY(KIGS_APPLICATION_CLASS, ApplicationName(KIGS_APPLICATION_CLASS));
				mApp = KIGS_APPLICATION_CLASS::CreateInstance(ApplicationName(KIGS_APPLICATION_CLASS));
				
				#ifdef INIT_DEFAULT_MODULES
					// on android no BASE_DATA_PATH used
					//! then init
					mApp->InitApp(0,true);
				#else
					//! then init
					mApp->Init(0,false);
				#endif //INIT_DEFAULT_MODULES
				
				pApp->userData = mApp.get();
			}
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being destroyed. Use this to clean up your userData to avoid leaking
            // resources.
            //
            // We have to check if userData is assigned just in case this comes in really quickly
            if (pApp->userData) {
                //
               //! close
				mApp->CloseApp();
				mApp=nullptr;
                pApp->userData = nullptr;
				
				//! last thing to do
				KigsCore::Close();
            }
            break;
        default:
            break;
    }
}

/*!
 * Enable the motion events you want to handle; not handled events are
 * passed back to OS for further processing. For this example case,
 * only pointer and joystick devices are enabled.
 *
 * @param motionEvent the newly arrived GameActivityMotionEvent.
 * @return true if the event is from a pointer or joystick device,
 *         false for all other input devices.
 */
bool motion_event_filter_func(const GameActivityMotionEvent *motionEvent) {
    auto sourceClass = motionEvent->source & AINPUT_SOURCE_CLASS_MASK;
    return (sourceClass == AINPUT_SOURCE_CLASS_POINTER ||
            sourceClass == AINPUT_SOURCE_CLASS_JOYSTICK);
}

/*!
 * This the main entry point for a native activity
 */
void android_main(struct android_app *pApp) {

    // Register an event handler for Android events
    pApp->onAppCmd = handle_cmd;

    // Set input event filters (set it to NULL if the app wants to process all inputs).
    // Note that for key inputs, this example uses the default default_key_filter()
    // implemented in android_native_app_glue.c.
    android_app_set_motion_event_filter(pApp, motion_event_filter_func);

    // This sets up a typical game/event loop. It will run until the app is destroyed.
    int events;
    android_poll_source *pSource;
    do {
        // Process all pending events before running game logic.
        if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0) {
            if (pSource) {
                pSource->process(pApp, pSource);
            }
        }

        // Check if any user data is associated. This is assigned in handle_cmd
        if (pApp->userData) {

            mApp->UpdateApp();
        }
    } while (!pApp->destroyRequested);
}

	
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
