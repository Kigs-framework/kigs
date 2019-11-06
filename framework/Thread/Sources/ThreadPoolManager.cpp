#include "PrecompiledHeaders.h"
#include "ThreadPoolManager.h"

IMPLEMENT_CLASS_INFO(ThreadPoolManager)

ThreadPoolManager::ThreadPoolManager(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, myThreadCount(*this, true, LABEL_AND_ID(ThreadCount),4)
, mySemaphore(0)
{
	myThreadList.clear();
	myQueuedtasks.clear();
	
}

void	ThreadPoolManager::InitModifiable()
{
	if (!IsInit())
	{
		// create a pool with n thread manage working / paused threads
		if (myThreadCount > 0)
		{

			mySemaphore = (Semaphore*)KigsCore::GetInstanceOf("ThreadPoolManagerSemaphore", _S_2_ID("Semaphore"));

			int i;
			for (i = 0; i < (int)myThreadCount; i++)
			{
				char str[128];
				snprintf(str, 128, "workerThread_%d", i+1);
				WorkerThread*	newworker = (WorkerThread*)(KigsCore::GetInstanceOf(str, _S_2_ID("WorkerThread")));
				newworker->setSemaphore(mySemaphore);
				newworker->setThreadPoolManager(this);
				newworker->Init();
				//newworker->setAffinityMask(1 << i);
				myThreadList.push_back(newworker);
			}
			CoreModifiable::InitModifiable();
		}
	}
}

ThreadPoolManager::~ThreadPoolManager()
{
	unsigned int i;
	for (i = 0; i < myThreadList.size(); i++)
	{
		myThreadList[i]->KillThread();
		myThreadList[i]->Destroy();
	}
	myThreadList.clear();

	if (mySemaphore)
	{
		mySemaphore->Destroy();
	}
}

// return event 
ThreadEvent*	ThreadPoolManager::setTask(MethodCallingStruct* task)
{

	// lock 
	mySemaphore->addItem(this);
	
	// getAvailableEvent always returns a valid available event 
	ThreadEvent* result = getTaskEndEvent();
	result->setValue(LABEL_TO_ID(EventCounter), 1);
	WorkerThread*	wavailable = getAvailableThread();
	if (wavailable)
		wavailable->setTask(task, result);
	else
		addTaskToQueue(task,result);
	result->Destroy();
	// unlock
	mySemaphore->removeItem(this);
	return result;
}

void ThreadPoolManager::addTaskToQueue(MethodCallingStruct* task,ThreadEvent* endevent)
{
	MethodEventPair	toAdd;
	toAdd.myMethodCallingStruct = task;
	toAdd.myThreadEvent = endevent;

	myQueuedtasks.push_back(toAdd);

}

// to be called in locked block
WorkerThread*	ThreadPoolManager::getAvailableThread()
{
	unsigned int i;
	for (i = 0; i < myThreadList.size(); i++)
	{
		if (myThreadList[i]->isAvailable())
		{
			return myThreadList[i];
		}
	}
	
	return 0;
}

// to be called in locked block
ThreadEvent*	ThreadPoolManager::getTaskEndEvent()
{
	
	// if no event available, create a new one
	ThreadEvent* newevent = (ThreadEvent*)KigsCore::GetInstanceOf("WorkerThreadEvent", _S_2_ID("ThreadEvent"));
	newevent->setSemaphore(mySemaphore);
	newevent->Init();

	return newevent;
}


ThreadPoolManager::TaskGroupHandle*	ThreadPoolManager::createTaskGroup()
{
	TaskGroup* newgroup = new TaskGroup();
	return newgroup;
}

ThreadPoolManager::TaskGroupHandle*	ThreadPoolManager::createTaskGroup(MethodCallingStruct* tasks[], int size)
{
	TaskGroup* newgroup = new TaskGroup();
	for (int i = 0; i < size; ++i)
	{
		newgroup->addTask(tasks[i]);
	}
	return newgroup;
}

SmartPointer<ThreadEvent> ThreadPoolManager::LaunchTaskGroup(ThreadPoolManager::TaskGroupHandle* tgh)
{
	TaskGroup* taskgroup = (TaskGroup*)tgh;
	// lock 
	mySemaphore->addItem(this);
	int taskgroupsize = taskgroup->taskList.size();
	if (taskgroupsize == 0)
	{
		// unlock
		mySemaphore->removeItem(this);
		delete taskgroup;
		return 0;
	}

	SmartPointer<ThreadEvent> result = getTaskEndEvent();
	result->setValue(LABEL_TO_ID(EventCounter), taskgroupsize);

	int i;
	for (i = 0; i < taskgroupsize; i++)
	{
		WorkerThread*	wavailable = getAvailableThread();
		if (wavailable)
			wavailable->setTask(taskgroup->taskList[i], result.get());
		else
			addTaskToQueue(taskgroup->taskList[i], result.get());
	}
	
	// unlock
	mySemaphore->removeItem(this);
	
	delete taskgroup;
	return result;
}

// when a WorkerThread has finished is task, launch a queued task if available 
// called between lock
// return true if a new task is launched
bool	ThreadPoolManager::ManageQueue(WorkerThread* endedThread)
{
	bool result = false;
	if (myQueuedtasks.size())
	{
		endedThread->setTask(myQueuedtasks[0].myMethodCallingStruct, myQueuedtasks[0].myThreadEvent.get());
		myQueuedtasks.erase(myQueuedtasks.begin());
		result = true;
	}

	return result;
}