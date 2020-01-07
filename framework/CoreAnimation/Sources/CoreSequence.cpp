#include "PrecompiledHeaders.h"
#include "CoreSequence.h"
#include "CoreAction.h"
#include "Core.h"
#include "ModuleCoreAnimation.h"


CoreSequence::CoreSequence(CoreModifiable* target, KigsID nameID, Timer* reftimer) :
	myTarget(target)
	, myID(nameID)
	, myStartTime(-1.0)
	, myPauseTime(-1.0)
	, myRefTimer(reftimer)
	, myCurrentActionIndex(0)
{
	ModuleCoreAnimation* coreanim = (ModuleCoreAnimation*)KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex);
	KigsCore::Connect(target, "Destroy", coreanim, "OnDestroyCallBack");
}

// return true if finished
bool	CoreSequence::update(const Timer& timer)
{
	if (myStartTime < -2.0) // start at first update mode
	{
		if (myRefTimer)
		{
			protectedStart(myRefTimer->GetTime());
		}
		else
		{
			protectedStart(timer.GetTime());
		}
	}

	if (myStartTime < 0.0) // not started
	{
		return false;
	}

	if (myRefTimer)
	{
		protectedUpdate(myRefTimer->GetTime());
	}
	else
	{
		protectedUpdate(timer.GetTime());
	}

	if (myCurrentActionIndex == 0xFFFFFFFF)
	{
		myTarget->Emit("SequenceFinished", myTarget, this);
		return true;
	}
	return false;
}

void	CoreSequence::stop()
{
	// reset all actions
	kstl::vector<CoreItemSP>::iterator itAction = myVector.begin();
	kstl::vector<CoreItemSP>::iterator itActionEnd = myVector.end();

	while (itAction != itActionEnd)
	{
		CoreAction* current = (CoreAction*)(*itAction).get();
		current->reset();
		++itAction;
	}

	// go back to first action
	myCurrentActionIndex = 0;

	//reset myStartTime && myPauseTime
	myPauseTime = myStartTime = -1.0;
}


CoreSequence::~CoreSequence()
{
	ModuleCoreAnimation* coreanim = (ModuleCoreAnimation*)KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex);

	if (myTarget)
	{
		KigsCore::Disconnect(myTarget, "Destroy", coreanim, "OnDestroyCallBack");
	}
}

void	CoreSequence::protectedStart(kdouble time)
{
	if (myStartTime < 0.0) // check if not already started
	{
		if (myVector.size())
		{
			CoreAction* current = (CoreAction*)myVector[myCurrentActionIndex].get();
			current->setStartTime(time);
			myStartTime = time;
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
	if (myCurrentActionIndex == 0xFFFFFFFF) return;

	CoreAction* current = (CoreAction*)myVector[myCurrentActionIndex].get();
	while (!done)
	{
		if (current->update(time)) // action is finished, start next one
		{
			++myCurrentActionIndex;
			if (myCurrentActionIndex < myVector.size())
			{
				kdouble previousend = current->getEndTime();
				current = (CoreAction*)myVector[myCurrentActionIndex].get();
				current->setStartTime(previousend);
			}
			else
			{
				myCurrentActionIndex = 0xFFFFFFFF;
				done = true;
			}
		}
		else
		{
			done = true;
		}
	}
}