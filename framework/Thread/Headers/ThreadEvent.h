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

	DECLARE_ABSTRACT_CLASS_INFO(ThreadEvent, CoreModifiable, Thread)

	//! constructor
	ThreadEvent(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	virtual void	wait();
	virtual void	signal();

	void	setSemaphore(Semaphore* semaphore)
	{
		mySemaphore = semaphore;
	}

protected:

	friend class WorkerThread;
	friend class ThreadPoolManager;

	virtual void platformWait() = 0;
	virtual void platformSignal() = 0;


	//! destructor
	virtual ~ThreadEvent();

	void	setAvailable(bool available);

	Semaphore*	mySemaphore;

	maInt			myEventCounter;
	int				myCurrentCount;
	maBool			myAutoReset;
};

#endif //_THREADEVENT_H_
