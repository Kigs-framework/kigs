#ifndef _THREAD_H_
#define _THREAD_H_

#include "CoreModifiableAttribute.h"
#include "CoreModifiable.h"
#include "maReference.h"
#include "maString.h"
#include "AttributePacking.h"
#include <thread>

// ****************************************
// * Thread class
// * --------------------------------------
/**
 * \file	Thread.h
 * \class	Thread
 * \ingroup Thread
 * \brief	Manage a thread.
 */
 // ****************************************
class Thread : public CoreModifiable
{
public:

    DECLARE_CLASS_INFO(Thread,CoreModifiable,Thread)

	//! constructor
    Thread(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	enum class State
	{
		UNINITIALISED=0,
		RUNNING,
		FINISHED
	};

	//! return current state (uninitialised, normal or paused)
	State	GetState(){return mCurrentState;}
	
	template<typename... T>
	void	Start(T&&... params);

	virtual void	Start();

	kfloat	GetProgress(){return 	mProgress;}
    inline State GetCurrentState() const {return mCurrentState;}
    
	void	setMethod(CoreModifiable* localthis, const std::string& method)
	{
		if (mFunctionWasInserted)
		{
			mCallee->RemoveMethod(mMethod.const_ref());
			mFunctionWasInserted = false;
		}

		mCallee = localthis;
		mMethod = method;
	}
	template<typename F>
	void	setMethod(CoreModifiable* localthis, const std::string& method, F&& func)
	{
		if (mFunctionWasInserted)
		{
			mCallee->RemoveMethod(mMethod.const_ref());
			mFunctionWasInserted = false;
		}

		mCallee = localthis;
		mMethod = method;

		mCallee->InsertFunction(method, func);
		mFunctionWasInserted = true;
	}

protected:

	// Init start thread if Method && Callee parameters are set
	virtual void InitModifiable() override;

	// reset all states
	void	Done();

	//! destructor
    virtual ~Thread();
 
	State					mCurrentState;
	kfloat					mProgress;
	std::thread				mCurrentThread;

	maString				mMethod= INIT_ATTRIBUTE(Method, "");
	maReference				mCallee= INIT_ATTRIBUTE(Callee,"");
	bool					mFunctionWasInserted = false;
};

#endif //_THREAD_H_
