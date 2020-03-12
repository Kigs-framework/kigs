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
	myThreadEvent = nullptr;
}

//! overloaded InitModifiable method. Init thread
void WorkerThread::InitModifiable()
{
	if (!IsInit())
	{
		if (mySemaphore)
		{
			myThreadEvent = KigsCore::GetInstanceOf("WorkerThreadEvent","ThreadEvent");
			myThreadEvent->setValue(LABEL_TO_ID(AutoReset), true);
			myThreadEvent->setSemaphore(mySemaphore);

			// set Callee and Method, else Thread::InitModifiable will not call Start
			myCallee = this;
			myMethod = "WorkerThread";
			Thread::InitModifiable();
		}
	}
}


//! the thread run method, wait for a task and sleep when task is done
void WorkerThread::Start()
{
	myCurrentThread = std::thread([this]()
		{
			myCurrentState = State::RUNNING;
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
			Done();
		});

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
	mySemaphore->GetMutex().lock();
	cleanTask();
	myThreadEventEnd->signal();
	myThreadEventEnd = 0;
	myIsAutoFeed=myParentPoolManager->ManageQueue(this);
	mySemaphore->GetMutex().unlock();
}

// to be called in a locked block
bool	WorkerThread::isAvailable()
{
	return (myCurrentTask == 0);
}

// to be called in a locked block
bool	WorkerThread::setTask(MethodCallingStruct* task,SmartPointer<ThreadEvent>& finishedevent)
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