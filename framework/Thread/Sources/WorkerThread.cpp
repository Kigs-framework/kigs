#include "PrecompiledHeaders.h"
#include "WorkerThread.h"
#include "ThreadPoolManager.h"
#include <Remotery.h>

IMPLEMENT_CLASS_INFO(WorkerThread)

WorkerThread::WorkerThread(const kstl::string& name, CLASS_NAME_TREE_ARG) : Thread(name, PASS_CLASS_NAME_TREE_ARG)
, mCurrentTask(0)
, mNeedExit(false)
, mSemaphore(0)
, mThreadEvent(0)
, mThreadEventEnd(0)
, mParentPoolManager(0)
, mIsAutoFeed(false)
{
	mEmptyParams.clear();
}

WorkerThread::~WorkerThread()
{
	cleanTask();
	mThreadEvent = nullptr;
}

//! overloaded InitModifiable method. Init thread
void WorkerThread::InitModifiable()
{
	if (!IsInit())
	{
		if (mSemaphore)
		{
			mThreadEvent = KigsCore::GetInstanceOf("WorkerThreadEvent","ThreadEvent");
			mThreadEvent->setValue(LABEL_TO_ID(AutoReset), true);
			mThreadEvent->setSemaphore(mSemaphore);

			// set Callee and Method, else Thread::InitModifiable will not call Start
			mCallee = this;
			mMethod = "WorkerThread";
			Thread::InitModifiable();
		}
	}
}


//! the thread run method, wait for a task and sleep when task is done
void WorkerThread::Start()
{
	mCurrentThread = std::thread([this]()
		{
			mCurrentState = State::RUNNING;
			do
			{
				if (!mIsAutoFeed)
					pause();

				mIsAutoFeed = false;
				if (mCurrentTask)
				{
					mCurrentTask->mMethodInstance->CallMethod(mCurrentTask->mMethodID, mEmptyParams, mCurrentTask->mPrivateParams);
					processDone();
				}
			} while (mNeedExit == false);
			Done();
		});

}

void	WorkerThread::pause()
{
	mThreadEvent->wait();
}

void	WorkerThread::resume()
{
	mThreadEvent->signal();
}

void	WorkerThread::processDone()
{
	mSemaphore->GetMutex().lock();
	cleanTask();
	mThreadEventEnd->signal();
	mThreadEventEnd = 0;
	mIsAutoFeed=mParentPoolManager->ManageQueue(this);
	mSemaphore->GetMutex().unlock();
}

// to be called in a locked block
bool	WorkerThread::isAvailable()
{
	return (mCurrentTask == 0);
}

// to be called in a locked block
bool	WorkerThread::setTask(MethodCallingStruct* task,SmartPointer<ThreadEvent>& finishedevent)
{
	bool result = false;
	if (mCurrentTask == 0)
	{
		mThreadEventEnd = finishedevent;
		mCurrentTask = task;
		resume();
		result = true;
	}
	return result;
}