#ifndef _THREAD_H_
#define _THREAD_H_

#include "CoreModifiableAttribute.h"
#include "CoreModifiable.h"

// ****************************************
// * Thread class
// * --------------------------------------
/*!  \class Thread
      base class for threads
	 \ingroup Thread
*/
// ****************************************

class ThreadRunMethod;

class Thread : public CoreModifiable
{
public:

    DECLARE_ABSTRACT_CLASS_INFO(Thread,CoreModifiable,Thread)

	//! constructor
    Thread(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	enum State
	{
		UNINITIALISED=0,
		RUNNING,
		FINISHED
	};

	//! return current state (uninitialised, normal or paused)
	State	GetState(){return myCurrentState;}
	
	//!function for kill thread
	void KillThread ();
	void waitDeath(unsigned long P_Time_Out);
	void sleepThread();
	void wakeUpThread();
	kfloat	GetProgress(){return 	myProgress;}
    inline void SetOpenFlag(bool a_value){bOpen = a_value;}
    inline State GetCurrentState() const {return myCurrentState;}
	
	void	setAffinityMask(int mask);
    
protected:

	//! overloaded InitModifiable method. Init thread
	void InitModifiable() override;

	//! static run method, call the protectedRun method
	static ThreadReturnType	Run(void* param);

	//! the thread run method
	virtual int	protectedRun() =0 ;
    virtual void protectedClose(){};

	//! destructor
    virtual ~Thread();

	//! real thread 
	friend class			ThreadRunMethod;
	State					myCurrentState;
	SP<ThreadRunMethod>		myThreadRunMethod;
	kfloat					myProgress;
	maBool					bOpen;

};

#endif //_THREAD_H_
