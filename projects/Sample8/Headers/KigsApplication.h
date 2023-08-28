#pragma once

#ifndef BASE_APP_IMPORT
//! include the real application class file
#include "Sample8.h"
#endif

//! yes I want Timer and FileManager to be auto initialized
#define INIT_DEFAULT_MODULES

#ifdef INIT_DEFAULT_MODULES
//! And then I want the base data path to be :
#define BASE_DATA_PATH "."
#endif //INIT_DEFAULT_MODULES

//! then define it as the application class 
#define KIGS_APPLICATION_CLASS Sample8

//! for Intel Appup version

#define ADP_APPLICATIONID ADP_DEBUG_APPLICATIONID

