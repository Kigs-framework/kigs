#ifndef _PLATFORM_FILEMANAGER_H
#define _PLATFORM_FILEMANAGER_H

// no specific Windows Filemanager

void 	PlatformFileManagerModuleInit(KigsCore* /*core*/, const kstl::vector<CoreModifiableAttribute*>* /*params*/){;}
void 	PlatformFileManagerModuleClose(KigsCore* /*core*/){;}
void 	PlatformFileManagerModuleUpdate(const Timer* /*timer*/){;}
void	PlatformFileManagerGetModuleInstance(ModuleBase** /*instance*/){;}

#endif //_PLATFORM_FILEMANAGER_H 
