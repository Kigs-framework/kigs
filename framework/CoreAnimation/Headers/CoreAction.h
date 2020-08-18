#ifndef _COREACTION_H_
#define _COREACTION_H_

#include "CoreSequence.h"
#include "CoreItem.h"
#include "kTypes.h"
#include "TecLibs/Tec3D.h"

class CoreVector;

#define	TimeEpsilon	(0.005)

// ****************************************
// * CoreAction class
// * --------------------------------------
/**
* \file	CoreAction.h
* \class	CoreAction
* \ingroup CoreAnimation
* \brief	Special CoreItem type used to animate attributes
*
*/
// ****************************************

class CoreAction : public CoreItem
{
public:

	friend class CoreSequence;
	friend class ModuleCoreAnimation;

	virtual ~CoreAction();
	
	// return true if finished
	bool	update(kdouble time);	

	virtual void init(CoreSequence* sequence,CoreVector* params)=0;

	void	setIsDone()
	{
		mType=(COREITEM_TYPE)(11);
	}

	bool	isDone()
	{
		return (((unsigned int)mType)==11);
	}

	kdouble getEndTime()
	{
		return mStartTime+mDuration;
	}

	// action is started by sequence
	virtual void	setStartTime(kdouble t)
	{
		mStartTime=t;
	}

	// undone
	virtual void reset()
	{
		mType=(COREITEM_TYPE)(10);
	}

	inline kdouble	getDuration()
	{
		return mDuration;
	}

	virtual void*	getContainerStruct()
	{
		return 0;
	}

protected:

	// default constructor called by sons only
	CoreAction() : CoreItem((COREITEM_TYPE)10), mTarget(0), mStartTime(-1.0f), mDuration(-1.0f), mActionFlags(0),mParamID(0)
	{
		mTargetPath = "";
	}

	// if paramstring contains -> then extract param name part and return real target (son on current target)
	CoreModifiable*	checkSubTarget(kstl::string& paramstring);

	inline void CheckDelayTarget()
	{
		// if delayed target
		if (mTargetPath != "")
		{
			CMSP findTarget = mTarget->GetInstanceByPath(mTargetPath);
			if (findTarget)
			{
				mTarget = findTarget.get();
				mTargetPath = "";
			}
		}
	}

	// return true if action asks itself to be closed
	virtual bool	protectedUpdate(kdouble time)
	{
		CheckDelayTarget();
		return false; // no used
	}

	CoreModifiable*		mTarget;
	kdouble				mStartTime;
	kdouble				mDuration;

	unsigned int		mActionFlags;
	kstl::string		mTargetPath;
	unsigned int		mParamID;
};


// ****************************************
// * CoreActionWait class
// * --------------------------------------
/**
* \file	CoreAction.h
* \class	CoreActionWait
* \ingroup CoreAnimation
* \brief	Do nothing CoreAction 
*
* Usefull to delay another animation start. 
*/
// ****************************************
class CoreActionWait : public CoreAction
{
public:

	CoreActionWait() : CoreAction()
	{}
	virtual void init(CoreSequence* sequence,CoreVector* params);
protected:

	virtual bool	protectedUpdate(kdouble time){ CoreAction::protectedUpdate(time);  return false; };
};



#endif //_COREACTION_H_
