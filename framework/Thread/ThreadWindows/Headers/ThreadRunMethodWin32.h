#ifndef _THREADRUNMETHODWIN32_H_
#define _THREADRUNMETHODWIN32_H_

#include <windows.h>
#include "ThreadRunMethod.h"


// ****************************************
// * ThreadRunMethodWin32 class
// * --------------------------------------
/*!  \class ThreadRunMethodWin32
      Windows threadrun class
	 \ingroup ThreadWindows
*/
// ****************************************

class ThreadRunMethodWin32 : public ThreadRunMethod
{
public:
    DECLARE_CLASS_INFO(ThreadRunMethodWin32,ThreadRunMethod,Thread)

    ThreadRunMethodWin32(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	virtual void waitDeath(unsigned long P_Time_Out = 0xFFFFFFFF);
	virtual void sleepThread();
	virtual void wakeUpThread();

	virtual void	setAffinityMask(int mask);

protected:
    
	//! destructor
	virtual ~ThreadRunMethodWin32();

	//! internal thread start
	virtual void StartThread();

	//! internal thread end
	virtual void EndThread();

	HANDLE	myHandle;
	DWORD	myThreadId;
};    

#endif //_THREADRUNMETHODWIN32_H_
