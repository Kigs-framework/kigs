#include "PrecompiledHeaders.h"
#include "ThreadEventAndroid.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(ThreadEventAndroid)


ThreadEventAndroid::ThreadEventAndroid(const kstl::string& name, CLASS_NAME_TREE_ARG) : ThreadEvent(name, PASS_CLASS_NAME_TREE_ARG)
{
	pthread_mutex_init(&myMutex, NULL);
	pthread_cond_init(&myEventCondition, NULL);
}

ThreadEventAndroid::~ThreadEventAndroid()
{
	pthread_mutex_destroy(&myMutex);
	pthread_cond_destroy(&myEventCondition);
}

void	ThreadEventAndroid::platformWait()
{
	pthread_mutex_lock(&myMutex);

	while(myCurrentCount < myEventCounter)
		pthread_cond_wait(&myEventCondition, &myMutex);

	if(myAutoReset)
		myCurrentCount = 0;
	
	pthread_mutex_unlock(&myMutex);
}
void	ThreadEventAndroid::platformSignal()
{
	pthread_mutex_lock(&myMutex);
	myCurrentCount++;
	pthread_cond_signal(&myEventCondition);
	pthread_mutex_unlock(&myMutex);
}
