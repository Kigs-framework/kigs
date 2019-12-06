#include "PrecompiledHeaders.h"
#include "Thread.h"
#include "ThreadRunMethod.h"
#include "Core.h"
#include <ThreadLocalStorageManager.h>
#include <ThreadProfiler.h>

IMPLEMENT_CLASS_INFO(Thread)

Thread::Thread(const kstl::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
, myThreadRunMethod(NULL)
, bOpen(*this,false,LABEL_AND_ID(bOpen), true)
{
	myThreadRunMethod =NULL;
  myCurrentState=UNINITIALISED;
}     

Thread::~Thread()
{
	ThreadProfiler* threadProfiler = (ThreadProfiler*)KigsCore::GetSingleton("ThreadProfiler");
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
		myThreadRunMethod->removeItem(this);
		myThreadRunMethod->Destroy();
		myThreadRunMethod=0;
		myProgress=-1;
	}
}

ThreadReturnType	Thread::Run(void* param)
{
	Thread* localThis=(Thread*)param;
	ThreadLocalStorageManager* tls_manager = (ThreadLocalStorageManager*)KigsCore::GetSingleton("ThreadLocalStorageManager");
	if (tls_manager)
		tls_manager->RegisterThread(localThis);

	ThreadProfiler* threadProfiler = (ThreadProfiler*)KigsCore::GetSingleton("ThreadProfiler");
	if (threadProfiler)
		threadProfiler->RegisterThread(localThis);

	localThis->myProgress=0;
	// run thread
	int result = localThis->protectedRun();
	localThis->myProgress=1;

    localThis->SetOpenFlag(false);
    localThis->myCurrentState=FINISHED;
    
	// thread is finished
	if(localThis->myThreadRunMethod)
	{
		localThis->myThreadRunMethod->removeItem(localThis);
		localThis->myThreadRunMethod->Destroy();
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
	myThreadRunMethod=(ThreadRunMethod*)KigsCore::GetInstanceOf(this->getName() + "ThreadRunMethod","ThreadRunMethod");		
	
	if(!myThreadRunMethod->isSubType(ThreadRunMethod::myClassID))
	{
		myThreadRunMethod->Destroy();
		myThreadRunMethod=0;
		UninitModifiable();
	}

	if(myThreadRunMethod)
	{
		myThreadRunMethod->addItem(this);
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