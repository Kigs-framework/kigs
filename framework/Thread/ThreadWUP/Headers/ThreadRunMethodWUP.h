#ifndef _THREADRUNMETHODWUP_H_
#define _THREADRUNMETHODWUP_H_

#include <windows.h>
#include "ThreadRunMethod.h"


// ****************************************
// * ThreadRunMethodWUP class
// * --------------------------------------
/*!  \class ThreadRunMethodWUP
      Windows threadrun class
	 \ingroup ThreadWindows
*/
// ****************************************

class ThreadRunMethodWUP : public ThreadRunMethod
{
public:
    DECLARE_CLASS_INFO(ThreadRunMethodWUP,ThreadRunMethod,Thread)

    ThreadRunMethodWUP(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	virtual void waitDeath(unsigned long P_Time_Out = 0xFFFFFFFF);
	virtual void sleepThread();
	virtual void wakeUpThread();

	virtual void	setAffinityMask(int mask);

protected:
    
	//! destructor
	virtual ~ThreadRunMethodWUP();

	//! internal thread start
	virtual void StartThread();

	//! internal thread end
	virtual void EndThread();

	HANDLE	myHandle;
	DWORD	myThreadId;
};    

#endif //_THREADRUNMETHODWUP_H_
