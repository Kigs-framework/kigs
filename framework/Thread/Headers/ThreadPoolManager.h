#pragma once

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "WorkerThread.h"
#include "Semaphore.h"
#include "SmartPointer.h"

namespace Kigs
{
	namespace Thread
	{
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
				ThreadPoolManager(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~ThreadPoolManager();

			void	InitModifiable() override;

			// returns the end thread event associated to the task
			SmartPointer<ThreadEvent>	setTask(MethodCallingStruct* task);

			class TaskGroupHandle
			{
			public:
				virtual void	addTask(MethodCallingStruct* task) = 0;
				virtual ~TaskGroupHandle() {};
			};

			TaskGroupHandle* createTaskGroup();
			TaskGroupHandle* createTaskGroup(MethodCallingStruct* [], int size);
			SmartPointer<ThreadEvent>		LaunchTaskGroup(TaskGroupHandle* tgh);

			size_t	getRunningTaskCount();

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
				std::vector<MethodCallingStruct*> mTaskList;
				virtual ~TaskGroup() {};
			};

			SP<WorkerThread>	getAvailableThread();
			SmartPointer<ThreadEvent>	getTaskEndEvent();

			// queue
			void	addTaskToQueue(MethodCallingStruct* task, SmartPointer<ThreadEvent>& endevent);

			struct MethodEventPair
			{
				MethodCallingStruct* mMethodCallingStruct;
				SmartPointer<ThreadEvent>		mThreadEvent;
			};

			s32										mThreadCount = 4;
			SP<Semaphore>							mSemaphore;
			std::vector<SP<WorkerThread>>			mThreadList;
			std::vector<MethodEventPair>			mQueuedtasks;

			WRAP_ATTRIBUTES(mThreadCount);

		};

	}
}
