#ifndef _THREADEVENTANDROID_H_
#define _THREADEVENTANDROID_H_

#include <pthread.h>
#include "ThreadEvent.h"


// ****************************************
// * ThreadEventAndroid class
// * --------------------------------------
/*!  \class ThreadEventAndroid
Windows semaphore class
\ingroup ThreadAndroid
*/
// ****************************************

class ThreadEventAndroid : public ThreadEvent
{
public:
	DECLARE_CLASS_INFO(ThreadEventAndroid, ThreadEvent, Thread)

	ThreadEventAndroid(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	
protected:

	void platformWait() override;
	void platformSignal() override;

	//! destructor
	virtual ~ThreadEventAndroid();

	pthread_mutex_t myMutex;
	pthread_cond_t myEventCondition;

};

#endif //_THREADEVENTANDROID_H_
