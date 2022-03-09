#include "CoreFSMState.h"
#include "CoreFSM.h"

// state update check all transitions and return true if need transition
// when true is returned, specialOrder and stateID parameters are correctly set
bool CoreFSMStateBase::update(CoreModifiable* currentParentClass, u32& specialOrder, KigsID& stateID)
{
	if (mActiveTransition) // the state has activated itself the next transition
	{
		stateID = mActiveTransition->getState();
		specialOrder = mActiveTransition->getValue<u32>("TransitionBehavior");
		mActiveTransition->executeTransition(this);
		mActiveTransition = nullptr;
		return true;
	}
	for (auto t : mTransitions)
	{
		if(t->checkTransition(currentParentClass))
		{
			stateID = t->getState();
			specialOrder = t->getValue<u32>("TransitionBehavior");
			t->executeTransition(this);
			return true;
		}
	}
	return false;
}

bool CoreFSMStateBase::activateTransition(const KigsID& transitionname)
{
	for (auto t : mTransitions)
	{
		if (t->getNameID() == transitionname)
		{
			mActiveTransition = t.get();
			return true;
		}
	}
	return false;
}

// when state start, start all transitions
void	CoreFSMStateBase::start(CoreModifiable* currentParentClass, CoreFSMStateBase* prevstate)
{
	// start all transitions
	for (auto t : mTransitions)
	{
		t->start();
	}
}

// when state stop, stop all transitions
void	CoreFSMStateBase::stop(CoreModifiable* currentParentClass, CoreFSMStateBase* nextstate)
{
	// stop all transitions
	for (auto t : mTransitions)
	{
		t->stop();
	}
}

// a state can also ask itself to pop 
void CoreFSMStateBase::popState()
{
	mActiveTransition = CoreFSM::mPopTransition.get();
}