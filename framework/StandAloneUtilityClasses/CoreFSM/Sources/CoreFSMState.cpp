#include "CoreFSMState.h"


bool CoreFSMStateBase::Update(CoreModifiable* currentParentClass, u32& specialOrder, KigsID& stateID)
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

void	CoreFSMStateBase::start(CoreFSMStateBase* prevstate)
{
	// start all transitions
	for (auto t : mTransitions)
	{
		t->start();
	}
}
void	CoreFSMStateBase::stop(CoreFSMStateBase* nextstate)
{
	// stop all transitions
	for (auto t : mTransitions)
	{
		t->stop();
	}
}
