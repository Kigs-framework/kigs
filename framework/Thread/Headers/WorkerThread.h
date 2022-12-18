#ifndef _WORKERTHREAD_H_
#define _WORKERTHREAD_H_

#include "Thread.h"
#include "Semaphore.h"
#include "ThreadEvent.h"
#include "SmartPointer.h"


class ThreadPoolManager;

// ****************************************
// * WorkerThread class
// * --------------------------------------
/**
 * \file	WorkerThread.h
 * \class	WorkerThread
 * \ingroup Thread
 * \brief	Thread managed by a pool manager, waiting for tasks
 */
 // ****************************************
class WorkerThread : public Thread
{
public:

	DECLARE_CLASS_INFO(WorkerThread, Thread, Thread)
	WorkerThread(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~WorkerThread();

	void	setSemaphore(Semaphore* semaphore)
	{
		mSemaphore = semaphore;
	}

	void	setThreadPoolManager(ThreadPoolManager* poolmanager)
	{
		mParentPoolManager = poolmanager;
	}

	void	askExit()
	{
		mNeedExit = true;
		resume();
	}

	bool	isAvailable();

	// return true if available else return false
	bool	setTask(MethodCallingStruct* task,SmartPointer<ThreadEvent>& finishedevent);

protected:

	void cleanTask()
	{
		if (mCurrentTask)
			delete mCurrentTask;
		mCurrentTask = 0;
		
	}

	void	pause();
	void	resume();
	void	processDone();

	//! overloaded InitModifiable method. Init thread
	void InitModifiable() override;


	//! the thread run method
	void	Start() override;

	MethodCallingStruct*	mCurrentTask;

	std::vector<CoreModifiableAttribute*>	mEmptyParams;

	bool								mNeedExit;

	Semaphore*							mSemaphore;
	SP<ThreadEvent>						mThreadEvent;
	SmartPointer<ThreadEvent>			mThreadEventEnd;

	ThreadPoolManager*			mParentPoolManager;

	// true if the task has set another task (with ThreadPoolManager::ManageQueue) when done
	bool						mIsAutoFeed;
};

#endif //_WORKERTHREAD_H_
