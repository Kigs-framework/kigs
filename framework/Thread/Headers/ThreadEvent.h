#pragma once

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "Semaphore.h"

namespace Kigs
{
	namespace Thread
	{
		// ****************************************
		// * ThreadEvent class
		// * --------------------------------------
		/**
		 * \file	ThreadEvent.h
		 * \class	ThreadEvent
		 * \ingroup Thread
		 * \brief	Thread synchronisation object Event.
		 */
		 // ****************************************
		class ThreadEvent : public CoreModifiable
		{
		public:

			DECLARE_CLASS_INFO(ThreadEvent, CoreModifiable, Thread)
				ThreadEvent(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~ThreadEvent();

			void	wait();
			void	signal();

			void	setSemaphore(Semaphore* semaphore)
			{
				if (semaphore)
				{
					if (mOwnCriticalSection)
					{
						delete mCriticalSection;
					}
					mCriticalSection = &(semaphore->GetPrivateMutex());
					mOwnCriticalSection = false;
				}
			}

		protected:

			friend class WorkerThread;
			friend class ThreadPoolManager;

			bool						mOwnCriticalSection = true;
			bool						mAutoReset = false;
			s32							mCurrentCount = 0;
			s32							mEventCounter = 1;

			WRAP_ATTRIBUTES(mAutoReset, mEventCounter);

			std::mutex*					mCriticalSection = nullptr;
			std::condition_variable		mConditionVariable;
		};

	}
}