#ifndef _KIGSAPPLICATION_H_
#define _KIGSAPPLICATION_H_

//! include the real application class file
#include "KigsPackager.h"

//! yes I want Timer and FileManager to be auto initialized
#define INIT_DEFAULT_MODULES

#ifdef INIT_DEFAULT_MODULES
//! And then I want the base data path to be :
#define BASE_DATA_PATH "../../data"
#endif //INIT_DEFAULT_MODULES

//! then define it as the application class 
#define KIGS_APPLICATION_CLASS KigsPackager

//! for Intel Appup version

#define ADP_APPLICATIONID ADP_DEBUG_APPLICATIONID




#endif //_KIGSAPPLICATION_H_