#ifndef _THREADRUNMETHODIOS_H_
#define _THREADRUNMETHODIOS_H_

#include "ThreadRunMethod.h"
#include <pthread.h>

// ****************************************
// * ThreadRunMethodIOS class
// * --------------------------------------
/*!  \class ThreadRunMethodIOS
      IPhone threadrun class
	 \ingroup ThreadIOS
*/
// ****************************************

class ThreadRunMethodIOS : public ThreadRunMethod
{
public:
	DECLARE_CLASS_INFO(ThreadRunMethodIOS, ThreadRunMethod, Thread)

	ThreadRunMethodIOS(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
  
	virtual void waitDeath(unsigned long P_Time_Out = 0xFFFFFFFF);
	virtual void sleepThread();
	virtual void wakeUpThread();

protected:
    
	//! destructor
	virtual ~ThreadRunMethodIOS();

	//! internal thread start
	virtual void StartThread();

	//! internal thread end
	virtual void EndThread();

    pthread_t           myPThread;
    pthread_mutex_t     myMutex;
    pthread_cond_t      myCond;
};    

#endif //_THREADRUNMETHODIOS_H_
