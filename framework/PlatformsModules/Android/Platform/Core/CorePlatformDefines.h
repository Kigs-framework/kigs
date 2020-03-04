#ifndef _CORE_PLATFORM_DEFINES_H
#define _CORE_PLATFORM_DEFINES_H

// **********************************************************************
//
// ERROR MANAGEMENT
//
// **********************************************************************


// level error 0 is for all error / warning / messages
// level error 1 is for error / warning 
// level error 2 is for error only
// level error 3 is for nothing
#ifdef _DEBUG
#define KIGS_ERROR_LEVEL		0
#else //_DEBUG
#define KIGS_ERROR_LEVEL		0
#endif //_DEBUG

#define	STOP_ON_ERROR

#define	KIGS_ERROR_BUFFER_SIZE	4096 

// **********************************************************************
//
// STL MANAGEMENT
//
// **********************************************************************

#define _NO_KSTL_OVERLOADING_
#ifndef _NO_KSTL_OVERLOADING_
#define _DO_KSTL_OVERLOADING_
#endif
// **********************************************************************
//
// MEMORY MANAGEMENT
//
// **********************************************************************

#define _NO_MEMORY_MANAGER_

#define MEMORYMANAGEMENT_START 
#define MEMORYMANAGEMENT_END 

#define NO_MEMORYMANAGEMENT_START 
#define NO_MEMORYMANAGEMENT_END	 


// **********************************************************************
//
// THREAD MANAGEMENT
//
// **********************************************************************

#define SYSTEM_THREAD_RETURN_A_VALUE
#ifdef SYSTEM_THREAD_RETURN_A_VALUE
typedef	void* ThreadReturnType;
#else
typedef	void ThreadReturnType;
#endif

// **********************************************************************
//
// RTTI (dynamic cast) MANAGEMENT
//
// **********************************************************************

// NO RTTI on Android
//#define RTTI_IS_AVAILABLE



// **********************************************************************
//
// FILE MANAGEMENT
//
// **********************************************************************

#define PLATFORM_FILE 							ANDROIDFILE
#define Platform_Hiddenfopen 					Android_fopen
#define Platform_Hiddenfread 					Android_fread
#define Platform_Hiddenfwrite 					Android_fwrite
#define Platform_Hiddenftell 					Android_ftell
#define Platform_Hiddenfseek 					Android_fseek
#define Platform_Hiddenfflush 					Android_fflush
#define Platform_Hiddenfclose 					Android_fclose
#define Platform_CheckState						Android_CheckState
#define Platform_FindFullName					Android_FindFullName
#define	Platform_CreateFolderTree				Android_CreateFolderTree



// **********************************************************************
//
// RANDOM MANAGEMENT
//
// **********************************************************************

#define Platform_rand		rand
#define Platform_srand		srand

// **********************************************************************
//
// MISC
//
// **********************************************************************

#define USE_LIB_PNG
#define USE_LIB_JPG
#define SUPPORT_ETC_TEXTURE

#define kigsprintf(...) printf(__VA_ARGS__)

#endif //_CORE_PLATFORM_DEFINES_H