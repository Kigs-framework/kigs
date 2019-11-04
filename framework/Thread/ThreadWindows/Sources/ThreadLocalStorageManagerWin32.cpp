#include "PrecompiledHeaders.h"
#include "ThreadLocalStorageManagerWin32.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(ThreadLocalStorageManagerWin32)

ThreadLocalStorageManagerWin32::ThreadLocalStorageManagerWin32(const kstl::string& name, CLASS_NAME_TREE_ARG) : ThreadLocalStorageManager(name, PASS_CLASS_NAME_TREE_ARG)
{
	
}

int ThreadLocalStorageManagerWin32::AllocateLocalStorage() 
{
	unsigned int result = TlsAlloc();
	if (result == TLS_OUT_OF_INDEXES)
		KIGS_ERROR("Out of Win32 TLS Indexes", 2);
	return result;
}

void* ThreadLocalStorageManagerWin32::GetLocalStorage(unsigned index) 
{
	return TlsGetValue(index);
}

bool ThreadLocalStorageManagerWin32::SetLocalStorage(unsigned index, void* data) 
{
	return TlsSetValue(index, data) != 0;
}

void ThreadLocalStorageManagerWin32::FreeLocalStorage(unsigned index) 
{
	TlsFree(index);
}

ThreadLocalStorageManagerWin32::~ThreadLocalStorageManagerWin32()
{
	
}
