#include "PrecompiledHeaders.h"
#include "ThreadLocalStorageManager.h"

IMPLEMENT_CLASS_INFO(ThreadLocalStorageManager)

ThreadLocalStorageManager::ThreadLocalStorageManager(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
{
}

void ThreadLocalStorageManager::InitModifiable() 
{
	myThreadIndexTLS = AllocateLocalStorage();
	SetLocalStorage(myThreadIndexTLS, KigsCore::GetModule(_S_2_ID("ModuleThread")));
}


void ThreadLocalStorageManager::ProtectedDestroy()
{
	FreeLocalStorage(myThreadIndexTLS);
	CoreModifiable::ProtectedDestroy();
}

ThreadLocalStorageManager::~ThreadLocalStorageManager()
{
	
}
