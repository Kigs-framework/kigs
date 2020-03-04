#include "SemaphoreAndroid.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(SemaphoreAndroid)


SemaphoreAndroid::SemaphoreAndroid(const kstl::string& name,CLASS_NAME_TREE_ARG) : Semaphore(name,PASS_CLASS_NAME_TREE_ARG)
{	
    // init lock here
    pthread_mutexattr_init(&mutexAttr);
    pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutexlock, &mutexAttr);
}

SemaphoreAndroid::~SemaphoreAndroid()
{  
	// deinit lock here
	pthread_mutex_destroy(&mutexlock);
	pthread_mutexattr_destroy(&mutexAttr);
}    

bool	SemaphoreAndroid::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{	
    // lock
    pthread_mutex_lock(&mutexlock);
	
	// don't call father addItem, else an infinite loop is made
	//Semaphore::addItem(item,pos PASS_LINK_NAME(linkName));
	

	return true;
}

bool	SemaphoreAndroid::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
    // unlock
    pthread_mutex_unlock(&mutexlock);

	return true; 
	//Semaphore::removeItem(item);
}
