#ifndef _CORESEQUENCE_H_
#define _CORESEQUENCE_H_

#include "CoreVector.h"
#include "Timer.h"

class CoreSequence : public CoreVector
{
public:
	CoreSequence(CoreModifiable* target, KigsID name, Timer* reftimer = 0);

	virtual ~CoreSequence();

	KigsID	getID() const
	{
		return myID;
	}

	void	startAtFirstUpdate()
	{
		myStartTime = -4.0;
	}

	inline void	start(const Timer& timer)
	{
		myRefTimer = &timer;
		protectedStart(timer.GetTime());
	}
	inline void	start()// use ref timer
	{
		protectedStart(myRefTimer->GetTime());
	}

	// return true if finished
	bool	update(const Timer& timer);

	inline void	pause(const Timer& timer)
	{
		protectedPause(timer.GetTime());
	}
	inline void	pause() // use ref timer
	{
		protectedPause(myRefTimer->GetTime());
	}

	// reset sequence
	void	stop();

	CoreModifiable*	getTarget() const
	{
		return myTarget;
	}

	// remove the target if already destroyed
	void removeTarget()
	{
		myTarget = NULL;
	}

	// return true if target is modifiable 
	bool	useModifiable(CoreModifiable* modifiable) const
	{
		if (myTarget == modifiable)
		{
			return true;
		}
		return false;
	}

protected:

	void	protectedStart(kdouble time);
	void	protectedPause(kdouble time);
	void	protectedUpdate(kdouble time);

	CoreModifiable*		myTarget;
	KigsID				myID;
	kdouble				myStartTime;
	kdouble				myPauseTime;
	const Timer*		myRefTimer;
	unsigned int		myCurrentActionIndex;
};


#endif //_CORESEQUENCE_H_
