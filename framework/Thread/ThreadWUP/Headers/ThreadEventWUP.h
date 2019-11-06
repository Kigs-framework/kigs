#ifndef _THREADEVENTWUP_H_
#define _THREADEVENTWUP_H_

#include <windows.h>
#include "ThreadEvent.h"


// ****************************************
// * ThreadEventWUP class
// * --------------------------------------
/*!  \class ThreadEventWUP
Windows semaphore class
\ingroup ThreadWindows
*/
// ****************************************

class ThreadEventWUP : public ThreadEvent
{
public:
	DECLARE_CLASS_INFO(ThreadEventWUP, ThreadEvent, Thread)

	ThreadEventWUP(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	
protected:

	virtual void platformWait();
	virtual void platformSignal();

	//! destructor
	virtual ~ThreadEventWUP();

	//HANDLE	myHandle;
	CRITICAL_SECTION myCriticalSection;
	CONDITION_VARIABLE myConditionVariable;

};

#endif //_THREADEVENTWUP_H_
