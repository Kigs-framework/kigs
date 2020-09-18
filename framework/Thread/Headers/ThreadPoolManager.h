#ifndef _THREADPOOLMANAGER_H_
#define _THREADPOOLMANAGER_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "WorkerThread.h"
#include "Semaphore.h"
#include "SmartPointer.h"


// ****************************************
// * ThreadPoolManager class
// * --------------------------------------
/**
 * \file	ThreadPoolManager.h
 * \class	ThreadPoolManager
 * \ingroup Thread
 * \brief	Manage a pool of WorkerThreads.
 */
 // ****************************************
class ThreadPoolManager : public CoreModifiable
{
public:

	DECLARE_CLASS_INFO(ThreadPoolManager, CoreModifiable, Thread)

	//! constructor
	ThreadPoolManager(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void	InitModifiable() override;

	// returns the end thread event associated to the task
	SmartPointer<ThreadEvent>	setTask(MethodCallingStruct* task);

	class TaskGroupHandle
	{
	public:
		virtual void	addTask(MethodCallingStruct* task) = 0;
		virtual ~TaskGroupHandle() {};
	};

	TaskGroupHandle*	createTaskGroup();
	TaskGroupHandle*	createTaskGroup(MethodCallingStruct*[], int size);
	SmartPointer<ThreadEvent>		LaunchTaskGroup(TaskGroupHandle* tgh);

	unsigned int	getRunningTaskCount();

protected:

	friend class WorkerThread;
	// when a WorkerThread has finished is task, launch a queued task if available 
	bool	ManageQueue(WorkerThread* endedThread);

	class TaskGroup : public TaskGroupHandle
	{
	public:
		TaskGroup()
		{
			mTaskList.clear();
		}

		virtual void	addTask(MethodCallingStruct* task)
		{
			mTaskList.push_back(task);
		}
		kstl::vector<MethodCallingStruct*> mTaskList;
		virtual ~TaskGroup() {};
	};

	//! destructor
	virtual ~ThreadPoolManager();

	maInt	mThreadCount;
	kstl::vector<SP<WorkerThread>>				mThreadList;


	SP<WorkerThread>	getAvailableThread();
	SmartPointer<ThreadEvent>	getTaskEndEvent();

	SP<Semaphore>	mSemaphore;

	// queue
	void	addTaskToQueue(MethodCallingStruct* task, SmartPointer<ThreadEvent>& endevent);

	struct MethodEventPair
	{
		MethodCallingStruct*			mMethodCallingStruct;
		SmartPointer<ThreadEvent>		mThreadEvent;
	};
	kstl::vector<MethodEventPair>		mQueuedtasks;

};

#endif //_THREADPOOLMANAGER_H_
