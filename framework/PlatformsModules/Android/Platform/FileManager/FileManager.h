#ifndef _PLATFORM_FILEMANAGER_H
#define _PLATFORM_FILEMANAGER_H

//#include "../../FileManagerAndroid/Headers/ModuleFileManagerAndroid.h"
void 	PlatformFileManagerModuleInit(KigsCore* /*core*/, const std::vector<CoreModifiableAttribute*>* /*params*/){;}
void 	PlatformFileManagerModuleClose(KigsCore* /*core*/){;}
void 	PlatformFileManagerModuleUpdate(const Timer* /*timer*/){;}
void	PlatformFileManagerGetModuleInstance(ModuleBase** /*instance*/){;}

#endif //_PLATFORM_FILEMANAGER_H 
