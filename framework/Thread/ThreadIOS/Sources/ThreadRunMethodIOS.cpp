#include "ThreadRunMethodIOS.h"
#include "Core.h"
#include "Thread.h"

IMPLEMENT_CLASS_INFO(ThreadRunMethodIOS)

ThreadRunMethodIOS::ThreadRunMethodIOS(const kstl::string& name, CLASS_NAME_TREE_ARG) : ThreadRunMethod(name, PASS_CLASS_NAME_TREE_ARG)
{
    myPThread = 0;
    pthread_cond_init(&myCond,NULL);
}

ThreadRunMethodIOS::~ThreadRunMethodIOS()
{  
    if(myPThread && myThread)
        pthread_exit(reinterpret_cast<void*>(myPThread));
}

void ThreadRunMethodIOS::EndThread()
{
    if(myPThread&& myThread)
        pthread_exit(reinterpret_cast<void*>(myPThread));
}

void ThreadRunMethodIOS::StartThread()
{
    pthread_create(&myPThread, NULL, GetThreadRunMethod(),reinterpret_cast<void*>(myThread));
}

void ThreadRunMethodIOS::sleepThread()
{
    pthread_mutex_lock(&myMutex);
    pthread_cond_wait(&myCond, &myMutex);
}

void ThreadRunMethodIOS::wakeUpThread()
{
    pthread_cond_signal(&myCond);
    pthread_mutex_unlock(&myMutex);
}

void ThreadRunMethodIOS::waitDeath(unsigned long P_Time_Out)
{
    pthread_join(myPThread,NULL);
}

