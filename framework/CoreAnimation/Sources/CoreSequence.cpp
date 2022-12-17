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
		auto ptr = mTarget.lock();
		if(ptr)
			ptr->EmitSignal("SequenceFinished", ptr.get(), this);
		return true;
	}
	return false;
}

void	CoreSequence::stop()
{
	// reset all actions
	std::vector<CoreItemSP>::iterator itAction = mVector.begin();
	std::vector<CoreItemSP>::iterator itActionEnd = mVector.end();

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
	auto coreanim = KigsCore::GetModule<ModuleCoreAnimation>();
	if (!coreanim) return;

	if (auto ptr = mTarget.lock())
	{
		KigsCore::Disconnect(ptr.get(), "Destroy", coreanim.get(), "OnDestroyCallBack");
	}
}

void	CoreSequence::protectedStart(double time)
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

void	CoreSequence::protectedPause(double time)
{
	// TODO
}

void	CoreSequence::protectedUpdate(double time)
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
				double previousend = current->getEndTime();
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