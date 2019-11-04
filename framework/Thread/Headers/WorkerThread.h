#ifndef _WORKERTHREAD_H_
#define _WORKERTHREAD_H_

#include "Thread.h"
#include "Semaphore.h"
#include "ThreadEvent.h"
#include "SmartPointer.h"

// ****************************************
// * WorkerThread class
// * --------------------------------------
/*!  \class WorkerThread
		class waiting for tasks to do
\ingroup Thread
*/
// ****************************************

class ThreadPoolManager;

class WorkerThread : public Thread
{
public:

	DECLARE_CLASS_INFO(WorkerThread, Thread, Thread)

	//! constructor
	WorkerThread(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void	setSemaphore(Semaphore* semaphore)
	{
		mySemaphore = semaphore;
	}

	void	setThreadPoolManager(ThreadPoolManager* poolmanager)
	{
		myParentPoolManager = poolmanager;
	}

	void	askExit()
	{
		myNeedExit = true;
	}

	bool	isAvailable();

	// return true if available else return false
	bool	setTask(MethodCallingStruct* task,ThreadEvent* finishedevent);

protected:

	void cleanTask()
	{
		if (myCurrentTask)
			delete myCurrentTask;
		myCurrentTask = 0;
		
	}

	void	pause();
	void	resume();
	void	processDone();

	//! overloaded InitModifiable method. Init thread
	void InitModifiable() override;


	//! the thread run method
	int	protectedRun() override;

	//! destructor
	virtual ~WorkerThread();

	MethodCallingStruct*	myCurrentTask;

	kstl::vector<CoreModifiableAttribute*>	myEmptyParams;

	bool	myNeedExit;

	Semaphore*							mySemaphore;
	ThreadEvent*						myThreadEvent;
	SmartPointer<ThreadEvent>			myThreadEventEnd;

	ThreadPoolManager*			myParentPoolManager;

	// true if the task has set another task (with ThreadPoolManager::ManageQueue) when done
	bool						myIsAutoFeed;
};

#endif //_WORKERTHREAD_H_
