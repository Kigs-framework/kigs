#include "PrecompiledHeaders.h"
#include "CoreAction.h"
#include "CoreValue.h"

#include "ModuleCoreAnimation.h"


CoreAction::~CoreAction()
{
}

// if paramstring contains -> then extract param name part and return real target (son on current target)
CMSP CoreAction::checkSubTarget(kstl::string& paramstring)
{
	int found = paramstring.find_last_of("->");
	if (found != kstl::string::npos)
	{
		found -= 1;
		kstl::string CoreModifiablePath = paramstring.substr(0, found);
		paramstring = paramstring.substr(found + 2, paramstring.length() - found - 2);
		auto ptr = mTarget.lock();
		if (ptr)
		{
			CMSP findTarget = ptr->GetInstanceByPath(CoreModifiablePath);
			if (findTarget)
			{
				return findTarget;
			}
		}
		mTargetPath = CoreModifiablePath;
	}
	return mTarget.lock();
}


// return true if finished
bool	CoreAction::update(kdouble time)
{
	bool	finished=false;
	if(mStartTime<0.0)
	{
		KIGS_ERROR("Action update called before start\n",1);
		return true;
	}

	if(!isDone())
	{
		// if out of duration, clamp time to the end
		if(mDuration>=0.0)
		{
			if((time - mStartTime)>=mDuration)
			{
				time=mStartTime+mDuration;
				finished=true;
			}
		}

		finished|=protectedUpdate(time);

		if(finished)
		{
			// if finished and duration was not set, set duration according to current time
			if(mDuration<0.0)
			{
				mDuration=time-mStartTime;
			}
			setIsDone();
		}
		return finished;
	}

	return true;

}


void CoreActionWait::init(CoreSequence* sequence,CoreVector* params)
{
	mTarget = sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vStart,kfloat vEnd,unsigned int paramID => 4 params
	if(!(params->size() == 1))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0]->getValue(readfloat);
	mDuration=readfloat;
}

