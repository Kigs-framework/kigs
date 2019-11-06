#include "PrecompiledHeaders.h"
#include "ThreadLocalStorageManagerWUP.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(ThreadLocalStorageManagerWUP)

ThreadLocalStorageManagerWUP::ThreadLocalStorageManagerWUP(const kstl::string& name, CLASS_NAME_TREE_ARG) : ThreadLocalStorageManager(name, PASS_CLASS_NAME_TREE_ARG)
{
	
}

int ThreadLocalStorageManagerWUP::AllocateLocalStorage() 
{
	unsigned int result = TlsAlloc();
	if (result == TLS_OUT_OF_INDEXES)
		KIGS_ERROR("Out of WUP TLS Indexes", 2);
	return result;
}

void* ThreadLocalStorageManagerWUP::GetLocalStorage(unsigned index) 
{
	return TlsGetValue(index);
}

bool ThreadLocalStorageManagerWUP::SetLocalStorage(unsigned index, void* data) 
{
	return TlsSetValue(index, data) != 0;
}

void ThreadLocalStorageManagerWUP::FreeLocalStorage(unsigned index) 
{
	TlsFree(index);
}

ThreadLocalStorageManagerWUP::~ThreadLocalStorageManagerWUP()
{
	
}
