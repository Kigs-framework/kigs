#include "PrecompiledHeaders.h"

#include "AnonymousModule.h"
#include "Core.h"
#include "Timer.h"
#include "ModuleFileManager.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

//! macro to auto implement static members
IMPLEMENT_CLASS_INFO(AnonymousModule)

//! constructor just call ModuleBase constructor
AnonymousModule::AnonymousModule(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	myDynamicModule=0;
}

//! destructor do nothing
AnonymousModule::~AnonymousModule()
{
}

//! init : load associated dll and call the ModuleInit method
void AnonymousModule::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	myCore=core;
	KigsCore::ModuleInit(core,this);

	FilePathManager*	pathManager=(FilePathManager*)KigsCore::GetSingleton("FilePathManager");

	kstl::string fullfilename=getName();
	SmartPointer<FileHandle> fullfilenamehandle;
	if(pathManager)
	{

		fullfilenamehandle = pathManager->FindFullName(getName());
		if (fullfilenamehandle)
		{
			fullfilename = fullfilenamehandle->myFullFileName;
		}
		else
		{
			fullfilename = getName();
		}
	}
    
#ifdef WIN32
#ifndef WUP
	HINSTANCE handle = LoadLibraryEx(fullfilename.c_str(), NULL, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);

	if(handle)
	{
		myDllInitFunc ImpFuncDLL;

		ImpFuncDLL =(myDllInitFunc) GetProcAddress(handle, "ModuleInit");

		if (ImpFuncDLL)
		{
			DynamicModuleHandleAndPointer toAdd;
			toAdd.myHandle=(void*)handle;
			toAdd.myInstance=ImpFuncDLL(core,params); 

			myDynamicModuleList.push_back(toAdd);
			
			if(myDynamicModule == 0)
			{
				myDynamicModule=toAdd.myInstance;
			}
		}
		else
		{
			// not init properly
			FreeLibrary(handle);
		}
	}
#endif
#endif
}

//! close module
void AnonymousModule::Close()
{
	BaseClose();

#ifdef WIN32
	// free library before calling base class
	kstl::vector<DynamicModuleHandleAndPointer>::iterator it;
	for (it = myDynamicModuleList.begin(); it != myDynamicModuleList.end(); ++it)
	{
		FreeLibrary((HINSTANCE)(*it).myHandle);
	}
#endif

    
}

//! update module
void AnonymousModule::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);
}
