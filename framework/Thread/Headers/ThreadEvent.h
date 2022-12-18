#ifndef _THREADEVENT_H_
#define _THREADEVENT_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "Semaphore.h"


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

	maInt			mEventCounter;
	int				mCurrentCount;
	maBool			mAutoReset;

	std::mutex*					mCriticalSection=nullptr;
	bool						mOwnCriticalSection=true;
	std::condition_variable		mConditionVariable;
};

#endif //_THREADEVENT_H_
