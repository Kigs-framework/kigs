#ifndef _COREACTION_H_
#define _COREACTION_H_

#include "CoreSequence.h"
#include "CoreItem.h"
#include "kTypes.h"
#include "TecLibs/Tec3D.h"

class CoreVector;

#define	TimeEpsilon	(0.005)

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
		m_eType=(COREITEM_TYPE)(11);
	}

	bool	isDone()
	{
		return (((unsigned int)m_eType)==11);
	}

	kdouble getEndTime()
	{
		return myStartTime+myDuration;
	}

	// action is started by sequence
	virtual void	setStartTime(kdouble t)
	{
		myStartTime=t;
	}

	// undone
	virtual void reset()
	{
		m_eType=(COREITEM_TYPE)(10);
	}

	inline kdouble	getDuration()
	{
		return myDuration;
	}

	virtual void*	getContainerStruct()
	{
		return 0;
	}

protected:

	// default constructor called by sons only
	CoreAction() : CoreItem((COREITEM_TYPE)10), myTarget(0), myStartTime(-1.0f), myDuration(-1.0f), myActionFlags(0),myParamID(0)
	{
		myTargetPath = "";
	}

	// if paramstring contains -> then extract param name part and return real target (son on current target)
	CoreModifiable*	checkSubTarget(kstl::string& paramstring);

	inline void CheckDelayTarget()
	{
		// if delayed target
		if (myTargetPath != "")
		{
			CMSP findTarget = myTarget->GetInstanceByPath(myTargetPath);
			if (findTarget)
			{
				myTarget = findTarget.get();
				myTargetPath = "";
			}
		}
	}

	// return true if action asks itself to be closed
	virtual bool	protectedUpdate(kdouble time)
	{
		CheckDelayTarget();
		return false; // no used
	}

	CoreModifiable*		myTarget;
	kdouble				myStartTime;
	kdouble				myDuration;

	unsigned int		myActionFlags;
	kstl::string		myTargetPath;
	unsigned int		myParamID;
};


// do nothing
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
