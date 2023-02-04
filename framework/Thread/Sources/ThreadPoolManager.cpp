#include "PrecompiledHeaders.h"
#include "ThreadPoolManager.h"

using namespace Kigs::Thread;

IMPLEMENT_CLASS_INFO(ThreadPoolManager)

ThreadPoolManager::ThreadPoolManager(const std::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, mThreadCount(*this, "ThreadCount", 4)
, mSemaphore(nullptr)
{
	mThreadList.clear();
	mQueuedtasks.clear();
	
}

void	ThreadPoolManager::InitModifiable()
{
	if (!IsInit())
	{
		// create a pool with n thread manage working / paused threads
		if (mThreadCount > 0)
		{
			mSemaphore = KigsCore::GetInstanceOf("ThreadPoolManagerSemaphore", "Semaphore");

			int i;
			for (i = 0; i < (int)mThreadCount; i++)
			{
				char str[128];
				snprintf(str, 128, "workerThread_%d", i+1);
				SP<WorkerThread>	newworker = KigsCore::GetInstanceOf(str, "WorkerThread");
				newworker->setSemaphore(mSemaphore.get());
				newworker->setThreadPoolManager(this);
				newworker->Init();
				//newworker->setAffinityMask(1 << i);
				mThreadList.push_back(newworker);
			}
			CoreModifiable::InitModifiable();
		}
	}
}

ThreadPoolManager::~ThreadPoolManager()
{
	unsigned int i;
	for (i = 0; i < mThreadList.size(); i++)
	{
		mThreadList[i]->askExit();
		// wait for task to end
		while (mThreadList[i]->GetState() == Thread::State::RUNNING)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}

		mThreadList[i]=nullptr;
	}
	mThreadList.clear();

	mSemaphore = nullptr;
}

// return event 
SmartPointer<ThreadEvent>	ThreadPoolManager::setTask(MethodCallingStruct* task)
{

	// lock 
	mSemaphore->GetPrivateMutex().lock();
	
	// getAvailableEvent always returns a valid available event 
	SmartPointer<ThreadEvent> result = getTaskEndEvent();
	result->setValue("EventCounter", 1);
	SP<WorkerThread>	wavailable = getAvailableThread();
	if (wavailable)
		wavailable->setTask(task, result);
	else
		addTaskToQueue(task,result);

	// unlock
	mSemaphore->GetPrivateMutex().unlock();
	return result;
}

void ThreadPoolManager::addTaskToQueue(MethodCallingStruct* task,SmartPointer<ThreadEvent>& endevent)
{
	MethodEventPair	toAdd;
	toAdd.mMethodCallingStruct = task;
	toAdd.mThreadEvent = endevent;

	mQueuedtasks.push_back(toAdd);

}

size_t	ThreadPoolManager::getRunningTaskCount()
{
	size_t i;
	auto count= mThreadList.size();
	for (i = 0; i < mThreadList.size(); i++)
	{
		if (mThreadList[i]->isAvailable())
		{
			count--;
		}
	}
	return count;
}

// to be called in locked block
SP<WorkerThread>	ThreadPoolManager::getAvailableThread()
{
	unsigned int i;
	for (i = 0; i < mThreadList.size(); i++)
	{
		if (mThreadList[i]->isAvailable())
		{
			return mThreadList[i];
		}
	}
	
	return nullptr;
}

// to be called in locked block
SmartPointer<ThreadEvent>	ThreadPoolManager::getTaskEndEvent()
{
	
	// if no event available, create a new one
	SmartPointer<ThreadEvent> newevent = KigsCore::GetInstanceOf("WorkerThreadEvent", "ThreadEvent");
	newevent->setSemaphore(mSemaphore.get());
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
	std::lock_guard<std::mutex> lk(mSemaphore->GetPrivateMutex());
	size_t taskgroupsize = taskgroup->mTaskList.size();
	if (taskgroupsize == 0)
	{
		// unlock
		delete taskgroup;
		return 0;
	}

	SmartPointer<ThreadEvent> result = getTaskEndEvent();
	result->setValue("EventCounter", taskgroupsize);

	size_t i;
	for (i = 0; i < taskgroupsize; i++)
	{
		SP<WorkerThread>	wavailable = getAvailableThread();
		if (wavailable)
			wavailable->setTask(taskgroup->mTaskList[i], result);
		else
			addTaskToQueue(taskgroup->mTaskList[i], result);
	}
	
	delete taskgroup;
	return result;
}

// when a WorkerThread has finished is task, launch a queued task if available 
// called between lock
// return true if a new task is launched
bool	ThreadPoolManager::ManageQueue(WorkerThread* endedThread)
{
	bool result = false;
	if (mQueuedtasks.size())
	{
		endedThread->setTask(mQueuedtasks[0].mMethodCallingStruct, mQueuedtasks[0].mThreadEvent);
		mQueuedtasks.erase(mQueuedtasks.begin());
		result = true;
	}

	return result;
}