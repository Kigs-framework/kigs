#include "PrecompiledHeaders.h"
#include "ThreadLocalStorageManagerAndroid.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(ThreadLocalStorageManagerAndroid)

ThreadLocalStorageManagerAndroid::ThreadLocalStorageManagerAndroid(const kstl::string& name, CLASS_NAME_TREE_ARG) : ThreadLocalStorageManager(name, PASS_CLASS_NAME_TREE_ARG)
{
	
}

int ThreadLocalStorageManagerAndroid::AllocateLocalStorage() 
{
	unsigned int result = _key_index_assoc.size();
	//if(result >= PTHREAD_KEYS_MAX)
	//	KIGS_ERROR("Out of pthread TLS keys", 2);

	_key_index_assoc.resize(_key_index_assoc.size()+1);

	pthread_key_create(&_key_index_assoc[result], 0);
	return result;
}

void* ThreadLocalStorageManagerAndroid::GetLocalStorage(unsigned int index)
{
	return pthread_getspecific(_key_index_assoc[index]);
}

bool ThreadLocalStorageManagerAndroid::SetLocalStorage(unsigned int index, void* data) 
{
	pthread_setspecific(_key_index_assoc[index], data);
	return true;
}

void ThreadLocalStorageManagerAndroid::FreeLocalStorage(unsigned int index)
{
	pthread_key_delete(_key_index_assoc[index]);
}

ThreadLocalStorageManagerAndroid::~ThreadLocalStorageManagerAndroid()
{
	
}
