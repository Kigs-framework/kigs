#ifndef _THREADEVENTWIN32_H_
#define _THREADEVENTWIN32_H_

#include <windows.h>
#include "ThreadEvent.h"


// ****************************************
// * ThreadEventWin32 class
// * --------------------------------------
/*!  \class ThreadEventWin32
Windows semaphore class
\ingroup ThreadWindows
*/
// ****************************************

class ThreadEventWin32 : public ThreadEvent
{
public:
	DECLARE_CLASS_INFO(ThreadEventWin32, ThreadEvent, Thread)

	ThreadEventWin32(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	
protected:

	virtual void platformWait();
	virtual void platformSignal();

	//! destructor
	virtual ~ThreadEventWin32();

	//HANDLE	myHandle;
	CRITICAL_SECTION myCriticalSection;
	CONDITION_VARIABLE myConditionVariable;

};

#endif //_THREADEVENTWIN32_H_
