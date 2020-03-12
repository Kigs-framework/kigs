#ifndef _THREADEVENT_H_
#define _THREADEVENT_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "Semaphore.h"

// ****************************************
// * ThreadEvent class
// * --------------------------------------
/*!  \class ThreadEvent
	synchronisation object Event (or pthread_cond_t for pthread like)
\ingroup Thread
*/
// ****************************************

class ThreadEvent : public CoreModifiable
{
public:

	DECLARE_CLASS_INFO(ThreadEvent, CoreModifiable, Thread)

	//! constructor
	ThreadEvent(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void	wait();
	void	signal();

	void	setSemaphore(Semaphore* semaphore)
	{
		if (semaphore)
		{
			if (myOwnCriticalSection)
			{
				delete myCriticalSection;
			}
			myCriticalSection = &(semaphore->GetPrivateMutex());
			myOwnCriticalSection = false;
		}
	}

protected:

	friend class WorkerThread;
	friend class ThreadPoolManager;

	//! destructor
	virtual ~ThreadEvent();

	maInt			myEventCounter;
	int				myCurrentCount;
	maBool			myAutoReset;

	std::mutex*					myCriticalSection=nullptr;
	bool						myOwnCriticalSection=true;
	std::condition_variable		myConditionVariable;
};

#endif //_THREADEVENT_H_
