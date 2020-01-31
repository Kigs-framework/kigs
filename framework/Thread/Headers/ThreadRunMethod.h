#ifndef _THREADRUNMETHOD_H_
#define _THREADRUNMETHOD_H_

#include "CoreModifiable.h"
#include "Thread.h"

// ****************************************
// * ThreadRunMethod class
// * --------------------------------------
/*!  \class ThreadRunMethod
	base threadrun class
	 \ingroup ThreadWindows
*/
// ****************************************

class ThreadRunMethod : public CoreModifiable
{
public:

	friend class Thread;
    DECLARE_ABSTRACT_CLASS_INFO(ThreadRunMethod,CoreModifiable,Thread)

    ThreadRunMethod(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/*! \brief addItem
		overload from CoreModifiable
	*/
	bool addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;

	/*! \brief removeItem
		overload from CoreModifiable
	*/
	bool removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;

	virtual void waitDeath(unsigned long P_Time_Out = 0xFFFFFFFF) = 0;
	virtual void sleepThread() = 0;
	virtual void wakeUpThread() = 0;

	virtual void	setAffinityMask(int mask)=0;
  
protected:

	//! overloaded InitModifiable method. Start thread
	void InitModifiable() override;

	//! destructor
	virtual ~ThreadRunMethod();

	typedef ThreadReturnType (*ThreadRun)(void* param);

	ThreadRun	GetThreadRunMethod()
	{
		return Thread::Run;
	};

	//! internal thread start
	virtual void StartThread()=0;

	//! internal thread end
	virtual void EndThread()=0;

	Thread*	myThread;

};    

#endif //_THREADRUNMETHOD_H_