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
/*!  \class Thread
      base class for threads
	 \ingroup Thread
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
	State	GetState(){return myCurrentState;}
	
	template<typename... T>
	void	Start(T&&... params);

	virtual void	Start();

	kfloat	GetProgress(){return 	myProgress;}
    inline State GetCurrentState() const {return myCurrentState;}
    
	void	setMethod(CoreModifiable* localthis, const std::string& method)
	{
		if (myFunctionWasInserted)
		{
			myCallee->RemoveMethod(myMethod.const_ref());
			myFunctionWasInserted = false;
		}

		myCallee = localthis;
		myMethod = method;
	}
	template<typename F>
	void	setMethod(CoreModifiable* localthis, const std::string& method, F&& func)
	{
		if (myFunctionWasInserted)
		{
			myCallee->RemoveMethod(myMethod.const_ref());
			myFunctionWasInserted = false;
		}

		myCallee = localthis;
		myMethod = method;

		myCallee->InsertFunction(method, func);
		myFunctionWasInserted = true;
	}

protected:

	// Init start thread if Method && Callee parameters are set
	virtual void InitModifiable() override;

	// reset all states
	void	Done();

	//! destructor
    virtual ~Thread();
 
	State					myCurrentState;
	kfloat					myProgress;
	std::thread				myCurrentThread;

	maString				myMethod= INIT_ATTRIBUTE(Method, "");
	maReference				myCallee= INIT_ATTRIBUTE(Callee,"");
	bool					myFunctionWasInserted = false;
};

#endif //_THREAD_H_
