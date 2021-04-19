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
IMPLEMENT_CLASS_INFO(BaseDllModule)


//! constructor just call ModuleBase constructor
AnonymousModule::AnonymousModule(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	mDynamicModule=0;
}

//! destructor do nothing
AnonymousModule::~AnonymousModule()
{
}

//! init : load associated dll and call the ModuleInit method
void AnonymousModule::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	mCore=core;
	KigsCore::ModuleInit(core,this);

	auto pathManager = KigsCore::Singleton<FilePathManager>();

	kstl::string fullfilename=getName();
	SP<FileHandle> fullfilenamehandle;
	if(pathManager)
	{

		fullfilenamehandle = pathManager->FindFullName(getName());
		if (fullfilenamehandle->mStatus & FileHandle::Exist)
		{
			fullfilename = fullfilenamehandle->mFullFileName;
		}
		else
		{
			fullfilename = getName();
		}
	}
    
#ifdef WIN32
#ifndef WUP

	HINSTANCE handle = LoadLibrary(fullfilename.c_str()); // , NULL, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);

	if(handle)
	{
		mDllInitFunc ImpFuncDLL;

		ImpFuncDLL =(mDllInitFunc) GetProcAddress(handle, "ModuleInit");

		if (ImpFuncDLL)
		{
			DynamicModuleHandleAndPointer toAdd;
			toAdd.mHandle=(void*)handle;
			toAdd.mInstance=ImpFuncDLL(core,params); 

			mDynamicModuleList.push_back(toAdd);
			
			if(mDynamicModule == 0)
			{
				mDynamicModule=toAdd.mInstance;
			}
		}
		else
		{
			// not init properly
			FreeLibrary(handle);
		}
	}
#ifdef _DEBUG
	else
	{
		DWORD err = GetLastError();
		if (err)
		{
			LPSTR messageBuffer = nullptr;

			//Ask Win32 to give us the string version of that message ID.
			//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
			size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

			//Copy the error message into a std::string.
			std::string message(messageBuffer, size);

			//Free the Win32's string's buffer.
			LocalFree(messageBuffer);

			KIGS_WARNING("Error loading dll", 1);
			KIGS_WARNING(message.c_str(), 1);

		}
	}
#endif
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
	for (it = mDynamicModuleList.begin(); it != mDynamicModuleList.end(); ++it)
	{
		FreeLibrary((HINSTANCE)(*it).mHandle);
	}
#endif

    
}

//! update module
void AnonymousModule::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);
}
