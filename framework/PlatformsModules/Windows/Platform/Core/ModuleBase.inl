#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void PlatformCloseDLL(void* handle) 
{
#ifndef _KIGS_ONLY_STATIC_LIB_
	FreeLibrary((HINSTANCE)(*it).myHandle);
#endif
}


DynamicModuleHandleAndPointer PlatformLoadDLL(const std::string& dllname)
{
#ifndef _KIGS_ONLY_STATIC_LIB_

	FilePathManager*	pathManager = (FilePathManager*)KigsCore::GetSingleton(_S_2_ID("FilePathManager"));
	if (!pathManager)
		return;

	HINSTANCE handle = NULL;
	SmartPointer<FileHandle> fullfilenamehandle = pathManager->FindFullName(dllname);

	if (fullfilenamehandle)
		handle = LoadLibrary((fullfilenamehandle->myFullFileName).c_str());

	if (handle)
	{
		myDllInitFunc ImpFuncDLL;

		ImpFuncDLL = (myDllInitFunc)GetProcAddress(handle, "ModuleInit");

		if (ImpFuncDLL)
		{
			DynamicModuleHandleAndPointer toAdd;
			toAdd.myHandle = (void*)handle;
			toAdd.myInstance = ImpFuncDLL(core, params);
			return toAdd;
		}
		else
		{
			//! not init properly
			FreeLibrary(handle);
			printf("unable to load module %s\n", dllname.c_str());
		}
	}
	else
	{
		printf("unable to load module %s\n", dllname.c_str());
	}
#endif
	return {};
}

