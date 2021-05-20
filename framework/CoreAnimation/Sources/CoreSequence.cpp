#include "PrecompiledHeaders.h"
#include "CoreSequence.h"
#include "CoreAction.h"
#include "Core.h"
#include "ModuleCoreAnimation.h"


CoreSequence::CoreSequence(CMSP target, KigsID nameID, Timer* reftimer) :
	mTarget(target)
	, mID(nameID)
	, mStartTime(-1.0)
	, mPauseTime(-1.0)
	, mRefTimer(reftimer)
	, mCurrentActionIndex(0)
{
	ModuleCoreAnimation* coreanim = (ModuleCoreAnimation*)KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex);
	KigsCore::Connect(target.get(), "Destroy", coreanim, "OnDestroyCallBack");
}

// return true if finished
bool	CoreSequence::update(const Timer& timer)
{
	if (mStartTime < -2.0) // start at first update mode
	{
		if (mRefTimer)
		{
			protectedStart(mRefTimer->GetTime());
		}
		else
		{
			protectedStart(timer.GetTime());
		}
	}

	if (mStartTime < 0.0) // not started
	{
		return false;
	}

	if (mRefTimer)
	{
		protectedUpdate(mRefTimer->GetTime());
	}
	else
	{
		protectedUpdate(timer.GetTime());
	}

	if (mCurrentActionIndex == 0xFFFFFFFF)
	{
		mTarget->EmitSignal("SequenceFinished", mTarget, this);
		return true;
	}
	return false;
}

void	CoreSequence::stop()
{
	// reset all actions
	kstl::vector<CoreItemSP>::iterator itAction = mVector.begin();
	kstl::vector<CoreItemSP>::iterator itActionEnd = mVector.end();

	while (itAction != itActionEnd)
	{
		CoreAction* current = (CoreAction*)(*itAction).get();
		current->reset();
		++itAction;
	}

	// go back to first action
	mCurrentActionIndex = 0;

	//reset mStartTime && mPauseTime
	mPauseTime = mStartTime = -1.0;
}


CoreSequence::~CoreSequence()
{
	ModuleCoreAnimation* coreanim = (ModuleCoreAnimation*)KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex);

	if (mTarget)
	{
		KigsCore::Disconnect(mTarget.get(), "Destroy", coreanim, "OnDestroyCallBack");
	}
}

void	CoreSequence::protectedStart(kdouble time)
{
	if (mStartTime < 0.0) // check if not already started
	{
		if (mVector.size())
		{
			CoreAction* current = (CoreAction*)mVector[mCurrentActionIndex].get();
			current->setStartTime(time);
			mStartTime = time;
		}
	}
}

void	CoreSequence::protectedPause(kdouble time)
{
	// TODO
}

void	CoreSequence::protectedUpdate(kdouble time)
{
	bool done = false;
	if (mCurrentActionIndex == 0xFFFFFFFF) return;

	CoreAction* current = (CoreAction*)mVector[mCurrentActionIndex].get();
	while (!done)
	{
		if (current->update(time)) // action is finished, start next one
		{
			++mCurrentActionIndex;
			if (mCurrentActionIndex < mVector.size())
			{
				kdouble previousend = current->getEndTime();
				current = (CoreAction*)mVector[mCurrentActionIndex].get();
				current->setStartTime(previousend);
			}
			else
			{
				mCurrentActionIndex = 0xFFFFFFFF;
				done = true;
			}
		}
		else
		{
			done = true;
		}
	}
}