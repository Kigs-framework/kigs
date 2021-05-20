#ifndef _CORESEQUENCE_H_
#define _CORESEQUENCE_H_

#include "CoreVector.h"
#include "Timer.h"

// ****************************************
// * CoreSequence class
// * --------------------------------------
/**
* \class	CoreSequence
* \file		CoreSequence.h
* \ingroup CoreAnimation
* \brief	Manage a sequence of animation ( list of animations ) 
*
*/
// ****************************************

class CoreSequence : public CoreVector
{
public:
	CoreSequence(CoreModifiable* target, KigsID name, Timer* reftimer = 0);

	virtual ~CoreSequence();

	KigsID	getID() const
	{
		return mID;
	}

	void	startAtFirstUpdate()
	{
		mStartTime = -4.0;
	}

	inline void	start(const Timer& timer)
	{
		mRefTimer = &timer;
		protectedStart(timer.GetTime());
	}
	inline void	start()// use ref timer
	{
		protectedStart(mRefTimer->GetTime());
	}

	// return true if finished
	bool	update(const Timer& timer);

	inline void	pause(const Timer& timer)
	{
		protectedPause(timer.GetTime());
	}
	inline void	pause() // use ref timer
	{
		protectedPause(mRefTimer->GetTime());
	}

	// reset sequence
	void	stop();

	CMSP	getTarget() const
	{
		return mTarget;
	}

	// remove the target if already destroyed
	void removeTarget()
	{
		mTarget = NULL;
	}

	// return true if target is modifiable 
	bool	useModifiable(CoreModifiable* modifiable) const
	{
		if (mTarget.get() == modifiable)
		{
			return true;
		}
		return false;
	}

protected:

	void	protectedStart(kdouble time);
	void	protectedPause(kdouble time);
	void	protectedUpdate(kdouble time);

	SP<CoreModifiable>	mTarget;
	KigsID				mID;
	kdouble				mStartTime;
	kdouble				mPauseTime;
	const Timer*		mRefTimer;
	unsigned int		mCurrentActionIndex;
};


#endif //_CORESEQUENCE_H_
