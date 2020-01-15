#include "PrecompiledHeaders.h"
#include "Thread.h"
#include "ThreadRunMethod.h"
#include "Core.h"
#include <ThreadLocalStorageManager.h>
#include <ThreadProfiler.h>

IMPLEMENT_CLASS_INFO(Thread)

Thread::Thread(const kstl::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
, myThreadRunMethod(nullptr)
, bOpen(*this,false,LABEL_AND_ID(bOpen), true)
{
	
  myCurrentState=UNINITIALISED;
}     

Thread::~Thread()
{
	SP<ThreadProfiler> threadProfiler = KigsCore::GetSingleton("ThreadProfiler");
	if (threadProfiler)
		threadProfiler->RemoveThread(this);

	KillThread();
}    

void Thread::KillThread ( )
{
	if(myThreadRunMethod)
	{
		if ( myCurrentState == RUNNING )
		{
			myThreadRunMethod->EndThread ( ); 
			myCurrentState = FINISHED;
		}
		CMSP toDel(this, GetRefTag{});
		myThreadRunMethod->removeItem(toDel);
		myThreadRunMethod=nullptr;
		myProgress=-1;
	}
}

ThreadReturnType	Thread::Run(void* param)
{
	SP<Thread> localThis((Thread*)param, StealRefTag{});
	SP<ThreadLocalStorageManager> tls_manager = KigsCore::GetSingleton("ThreadLocalStorageManager");
	if (tls_manager)
		tls_manager->RegisterThread(localThis.get());

	SP<ThreadProfiler> threadProfiler = KigsCore::GetSingleton("ThreadProfiler");
	if (threadProfiler)
		threadProfiler->RegisterThread(localThis.get());

	localThis->myProgress=0;
	// run thread
	int result = localThis->protectedRun();
	localThis->myProgress=1;

    localThis->SetOpenFlag(false);
    localThis->myCurrentState=FINISHED;
    
	// thread is finished
	if(localThis->myThreadRunMethod)
	{
		localThis->myThreadRunMethod->removeItem((CMSP&)localThis);
		localThis->myThreadRunMethod=0;
	}
    
    localThis->protectedClose();
#ifdef SYSTEM_THREAD_RETURN_A_VALUE
	return (ThreadReturnType)result;
#endif
}

void Thread::InitModifiable()
{
	CoreModifiable::InitModifiable();
	
	// create a os dependant thread 
	myThreadRunMethod=KigsCore::GetInstanceOf(this->getName() + "ThreadRunMethod","ThreadRunMethod");		
	
	if(!myThreadRunMethod->isSubType(ThreadRunMethod::myClassID))
	{
		myThreadRunMethod=nullptr;
		UninitModifiable();
	}

	if(myThreadRunMethod)
	{
		CMSP toAdd(this, GetRefTag{});
		myThreadRunMethod->addItem(toAdd);
 		myThreadRunMethod->Init();
		myCurrentState=RUNNING;
	}
}

void	Thread::setAffinityMask(int mask)
{
	if (myThreadRunMethod)
	{
		myThreadRunMethod->setAffinityMask(mask);
	}
}

void Thread::sleepThread()
{
	myThreadRunMethod->sleepThread();
}

void Thread::wakeUpThread()
{
	myThreadRunMethod->wakeUpThread();
}

void Thread::waitDeath(unsigned long P_Time_Out)
{
	myThreadRunMethod->waitDeath(P_Time_Out);
}