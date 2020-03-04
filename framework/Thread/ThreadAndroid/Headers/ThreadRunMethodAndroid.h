#ifndef _THREADRUNMETHODANDROID_H_
#define _THREADRUNMETHODANDROID_H_

#include "ThreadRunMethod.h"
#include <pthread.h>
#include <jni.h>


// ****************************************
// * ThreadRunMethodAndroid class
// * --------------------------------------
/*!  \class ThreadRunMethodAndroid
      Android threadrun class
	 \ingroup Thread
*/
// ****************************************

class ThreadRunMethodAndroid : public ThreadRunMethod
{
public:
    DECLARE_CLASS_INFO(ThreadRunMethodAndroid,ThreadRunMethod,Thread)

    ThreadRunMethodAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	void waitDeath(unsigned long P_Time_Out = 0xFFFFFFFF) override;
	void sleepThread() override;
	void wakeUpThread() override;
  
	void setAffinityMask(int mask) override;
protected:
    
	//! destructor
	virtual ~ThreadRunMethodAndroid();

	//! internal thread start
	void StartThread() override;

	//! internal thread end
	void EndThread() override;

	pthread_t myPThread;
	pthread_mutex_t myMutex;
	pthread_cond_t myCond;
};    

#endif //_THREADRUNMETHODANDROID_H_
