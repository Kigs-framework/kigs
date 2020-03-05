#include "SemaphoreIOS.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(SemaphoreIOS)


SemaphoreIOS::SemaphoreIOS(const kstl::string& name, CLASS_NAME_TREE_ARG) : Semaphore(name, PASS_CLASS_NAME_TREE_ARG)
{
    pthread_mutex_init(&myMutexLock,0);
}

SemaphoreIOS::~SemaphoreIOS()
{  
    pthread_mutex_destroy(&myMutexLock);
}    

bool	SemaphoreIOS::addItem(CoreModifiable *item, ItemPosition pos)
{
    pthread_mutex_lock(&myMutexLock);
	return true;
}

bool	SemaphoreIOS::removeItem(CoreModifiable *item DECLARE_DEFAULT_LINK_NAME)
{
    pthread_mutex_unlock(&myMutexLock);

	return true;
}
