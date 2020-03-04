#include "ThreadRunMethodAndroid.h"
#include "Core.h"
#include "Thread.h"
#include "CoreBaseApplication.h"

IMPLEMENT_CLASS_INFO(ThreadRunMethodAndroid)

ThreadRunMethodAndroid::ThreadRunMethodAndroid(const kstl::string& name,CLASS_NAME_TREE_ARG) : ThreadRunMethod(name,PASS_CLASS_NAME_TREE_ARG)
{
	myThread = 0;
	pthread_cond_init(&myCond, NULL);
}

ThreadRunMethodAndroid::~ThreadRunMethodAndroid()
{
	if(myPThread)
	{
		KigsJavaIDManager::detachCurrentThread(myPThread);
	}
}

void ThreadRunMethodAndroid::EndThread()
{
	pthread_exit((void*)myPThread);
}

void ThreadRunMethodAndroid::StartThread()
{
    int success = pthread_create(&myPThread, NULL, GetThreadRunMethod(), (void*)myThread);
	(void)success;
	KIGS_ASSERT(success == 0);
}

void ThreadRunMethodAndroid::sleepThread()
{
	pthread_mutex_lock(&myMutex);
    pthread_cond_wait(&myCond, &myMutex);
}

void ThreadRunMethodAndroid::wakeUpThread()
{
	pthread_cond_signal(&myCond);
    pthread_mutex_unlock(&myMutex);
}

void ThreadRunMethodAndroid::waitDeath(unsigned long P_Time_Out)
{
	int success = pthread_join( myPThread, NULL );
	(void)success;
	KIGS_ASSERT(success);
}

void	ThreadRunMethodAndroid::setAffinityMask(int mask)
{
	CoreBaseApplication::setThreadAffinityMask(&myPThread, mask);
}