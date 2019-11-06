#include "PrecompiledHeaders.h"
#include "CoreAction.h"
#include "CoreValue.h"

#include "ModuleCoreAnimation.h"


CoreAction::~CoreAction()
{

}

// if paramstring contains -> then extract param name part and return real target (son on current target)
CoreModifiable*	CoreAction::checkSubTarget(kstl::string& paramstring)
{
	int found = paramstring.find_last_of("->");
	if (found != kstl::string::npos)
	{
		found -= 1;
		kstl::string CoreModifiablePath = paramstring.substr(0, found);
		paramstring = paramstring.substr(found + 2, paramstring.length() - found - 2);
		CoreModifiable* findTarget = myTarget->GetInstanceByPath(CoreModifiablePath);
		if (findTarget != 0)
		{
			return findTarget;
		}

		myTargetPath = CoreModifiablePath;
	}
	return myTarget;
}


// return true if finished
bool	CoreAction::update(kdouble time)
{
	bool	finished=false;
	if(myStartTime<0.0)
	{
		KIGS_ERROR("Action update called before start\n",1);
		return true;
	}

	if(!isDone())
	{
		// if out of duration, clamp time to the end
		if(myDuration>=0.0)
		{
			if((time - myStartTime)>=myDuration)
			{
				time=myStartTime+myDuration;
				finished=true;
			}
		}

		finished|=protectedUpdate(time);

		if(finished)
		{
			// if finished and duration was not set, set duration according to current time
			if(myDuration<0.0)
			{
				myDuration=time-myStartTime;
			}
			setIsDone();
		}
		return finished;
	}

	return true;

}


void CoreActionWait::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vStart,kfloat vEnd,unsigned int paramID => 4 params
	if(!(params->size() == 1))
	{
		return;
	}
#endif
	float readfloat;
	((CoreValue<float>&)(*params)[0]).getValue(readfloat);
	myDuration=readfloat;
}

