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
/*!  \class ThreadPoolManager
			create and manage a pool of WorkerThreads
\ingroup Thread
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

	// when a WorkerThread has finished is task, launch a queued task if available 
	bool	ManageQueue(WorkerThread* endedThread);

protected:

	class TaskGroup : public TaskGroupHandle
	{
	public:
		TaskGroup()
		{
			taskList.clear();
		}

		virtual void	addTask(MethodCallingStruct* task)
		{
			taskList.push_back(task);
		}
		kstl::vector<MethodCallingStruct*> taskList;
		virtual ~TaskGroup() {};
	};

	//! destructor
	virtual ~ThreadPoolManager();

	maInt	myThreadCount;
	kstl::vector<SP<WorkerThread>>				myThreadList;


	SP<WorkerThread>	getAvailableThread();
	SmartPointer<ThreadEvent>	getTaskEndEvent();

	SP<Semaphore>	mySemaphore;

	// queue
	void	addTaskToQueue(MethodCallingStruct* task, SmartPointer<ThreadEvent>& endevent);

	struct MethodEventPair
	{
		MethodCallingStruct*			myMethodCallingStruct;
		SmartPointer<ThreadEvent>		myThreadEvent;
	};
	kstl::vector<MethodEventPair>		myQueuedtasks;

};

#endif //_THREADPOOLMANAGER_H_
