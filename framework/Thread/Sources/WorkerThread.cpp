#include "PrecompiledHeaders.h"
#include "WorkerThread.h"
#include "ThreadPoolManager.h"
#include <Remotery.h>

IMPLEMENT_CLASS_INFO(WorkerThread)

WorkerThread::WorkerThread(const kstl::string& name, CLASS_NAME_TREE_ARG) : Thread(name, PASS_CLASS_NAME_TREE_ARG)
, myCurrentTask(0)
, myNeedExit(false)
, mySemaphore(0)
, myThreadEvent(0)
, myThreadEventEnd(0)
, myParentPoolManager(0)
, myIsAutoFeed(false)
{
	myEmptyParams.clear();
}

WorkerThread::~WorkerThread()
{
	cleanTask();
	if (myThreadEvent)
	{
		myThreadEvent->Destroy();
	}
}

//! overloaded InitModifiable method. Init thread
void WorkerThread::InitModifiable()
{
	if (!IsInit())
	{
		if (mySemaphore)
		{
			myThreadEvent = (ThreadEvent*)KigsCore::GetInstanceOf("WorkerThreadEvent", _S_2_ID("ThreadEvent"));
			myThreadEvent->setValue(LABEL_TO_ID(AutoReset), true);
			myThreadEvent->setSemaphore(mySemaphore);
			Thread::InitModifiable();
		}
	}
}


//! the thread run method, wait for a task and sleep when task is done
int WorkerThread::protectedRun()
{
	rmt_SetCurrentThreadName(getName().c_str());
	do
	{
		if (!myIsAutoFeed)
			pause();

		myIsAutoFeed = false;
		if (myCurrentTask)
		{
			myCurrentTask->myMethodInstance->CallMethod(myCurrentTask->myMethodID, myEmptyParams, myCurrentTask->myPrivateParams);
			processDone();
		}
	} while (myNeedExit == false);

	return 0;
}

void	WorkerThread::pause()
{
	myThreadEvent->wait();
}

void	WorkerThread::resume()
{
	myThreadEvent->signal();
}

void	WorkerThread::processDone()
{
	mySemaphore->addItem(this);
	cleanTask();
	myThreadEventEnd->signal();
	myThreadEventEnd = 0;
	myIsAutoFeed=myParentPoolManager->ManageQueue(this);
	mySemaphore->removeItem(this);
}

// to be called in a locked block
bool	WorkerThread::isAvailable()
{
	return (myCurrentTask == 0);
}

// to be called in a locked block
bool	WorkerThread::setTask(MethodCallingStruct* task,ThreadEvent* finishedevent)
{
	bool result = false;
	if (myCurrentTask == 0)
	{
		myThreadEventEnd = finishedevent;
		myCurrentTask = task;
		resume();
		result = true;
	}
	return result;
}