#ifndef _KIGSAPPLICATION_H_
#define _KIGSAPPLICATION_H_

#ifndef BASE_APP_IMPORT
//! include the real application class file
#include "Sample5.h"
#endif

//! yes I want Timer and FileManager to be auto initialized
#define INIT_DEFAULT_MODULES

#ifdef INIT_DEFAULT_MODULES
//! And then I want the base data path to be :
#define BASE_DATA_PATH "../Data"
#endif //INIT_DEFAULT_MODULES

//! then define it as the application class 
#define KIGS_APPLICATION_CLASS Sample5

//! for Intel Appup version

#define ADP_APPLICATIONID ADP_DEBUG_APPLICATIONID




#endif //_KIGSAPPLICATION_H_
