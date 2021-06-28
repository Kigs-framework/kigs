#include "CoreFSMState.h"

// state update check all transitions and return true if need transition
// when true is returned, specialOrder and stateID parameters are correctly set
bool CoreFSMStateBase::update(CoreModifiable* currentParentClass, u32& specialOrder, KigsID& stateID)
{
	CoreFSMStateBase* found=nullptr;
	for (auto t : mTransitions)
	{
		if(t->checkTransition(currentParentClass))
		{
			stateID = t->getState();
			specialOrder = t->getValue<u32>("TransitionBehavior");

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
