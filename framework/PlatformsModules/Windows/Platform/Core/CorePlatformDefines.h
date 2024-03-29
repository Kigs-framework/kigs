#pragma once

//#define DOPROFILE

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
#define KIGS_ERROR_LEVEL		2
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

#ifndef _NO_MEMORY_MANAGER_

#define MM_USE_CUSTOM_CALLBACK
#define	MM_IS_INIT_BY_CORE

#endif


// **********************************************************************
//
// THREAD MANAGEMENT
//
// **********************************************************************

#define SYSTEM_THREAD_RETURN_A_VALUE

#ifdef SYSTEM_THREAD_RETURN_A_VALUE
typedef	int ThreadReturnType;
#else
typedef	void ThreadReturnType;
#endif

// **********************************************************************
//
// RTTI (dynamic cast) MANAGEMENT
//
// **********************************************************************

//#define RTTI_IS_AVAILABLE

// **********************************************************************
//
// FILE MANAGEMENT
//
// **********************************************************************

#define PLATFORM_FILE					FILE
#define Platform_Hiddenfopen			Win32fopen
#define Platform_Hiddenfread			Win32fread
#define Platform_Hiddenfwrite			Win32fwrite
#define Platform_Hiddenftell			Win32ftell
#define Platform_Hiddenfseek			Win32fseek
#define Platform_Hiddenfflush			Win32fflush
#define Platform_Hiddenfclose			Win32fclose
#define Platform_CheckState				Win32CheckState
#define Platform_FindFullName			Win32FindFullName
#define	Platform_CreateFolderTree		Win32CreateFolderTree

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
#define USE_LIB_GIF

#define SUPPORT_S3TC_TEXTURE
#define SUPPORT_ETC_TEXTURE

#define kigsprintf(...) printf(__VA_ARGS__)

//#ifdef KIGS_TOOLS
//#define NO_DELAYED_INIT
//#endif
