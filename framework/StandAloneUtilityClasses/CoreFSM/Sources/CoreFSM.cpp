#include "CoreFSM.h"
#include "Core.h"
#include "CoreBaseApplication.h"
#include "CoreFSMState.h"

IMPLEMENT_CLASS_INFO(CoreFSM)

CoreFSM::CoreFSM(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
{
	KigsCore::GetCoreApplication()->AddAutoUpdate(this);
}

void CoreFSM::Update(const Timer& timer, void* addParam) 
{
	if (!IsInit()) // if not init, try to init
	{
		Init();
	}
	if (!IsInit())
	{
		return;
	}
	u32 specialOrder = 0;
	KigsID stateID("");

	bool transit=mCurrentState.back()->Update(mAttachedObject, specialOrder, stateID);

	if (specialOrder == (u32)FSMStateSpecialOrder::POP_TRANSITION)
	{
		// TODO mCurrentState.back()->endState();
		popCurrentState();
		return;
	}
	if (transit)
	{
		// TODO mCurrentState.back()->endState();
		if (specialOrder == (u32)FSMStateSpecialOrder::PUSH_TRANSITION)
		{
			pushCurrentState(getState(stateID));
		}
		else
		{
			changeCurrentState(getState(stateID));
		}
	}

}

void	CoreFSM::changeCurrentState(CoreFSMStateBase* newone)
{
	if (!IsInit()) // if not init, error
	{
		KIGS_ERROR("FSM change current state on not init FSM", 2);
		return;
	}
	if (mCurrentState.size())
	{
		CoreFSMStateBase* prevone = mCurrentState.back();
		prevone->stop(newone);
		mAttachedObject->Downgrade(mCurrentState.back()->getID());
		mCurrentState.back() = newone;
		mAttachedObject->Upgrade(dynamic_cast<UpgradorBase*>(newone));
		newone->start(prevone);
	}
}

CoreFSMStateBase* CoreFSM::getState(const KigsID& id)
{
	if (mPossibleStates.find(id) != mPossibleStates.end())
	{
		return mPossibleStates[id];
	}
	return nullptr;
}


void	CoreFSM::pushCurrentState(CoreFSMStateBase* newone)
{
	if (!IsInit()) // if not init, error
	{
		KIGS_ERROR("FSM push current state on not init FSM", 2);
		return;
	}
	CoreFSMStateBase* prevone = nullptr;
	if (mCurrentState.size())
	{
		prevone = mCurrentState.back();
		prevone->stop(newone);
		mAttachedObject->Downgrade(mCurrentState.back()->getID());
	}
	mCurrentState.push_back(newone);
	mAttachedObject->Upgrade(dynamic_cast<UpgradorBase*>(newone));
	newone->start(prevone);
}
void	CoreFSM::popCurrentState()
{
	if (!IsInit()) // if not init, error
	{
		KIGS_ERROR("FSM pop current state on not init FSM", 2);
		return;
	}

	CoreFSMStateBase* prevone = nullptr;
	CoreFSMStateBase* newone = nullptr;

	if (mCurrentState.size() > 1)
	{
		newone = mCurrentState[mCurrentState.size() - 2];
	}
	if (mCurrentState.size())
	{
		prevone = mCurrentState.back();
		prevone->stop(newone);

		mAttachedObject->Downgrade(mCurrentState.back()->getID());
		mCurrentState.pop_back();
	}
	if (mCurrentState.size())
	{
		mAttachedObject->Upgrade(dynamic_cast<UpgradorBase*>(mCurrentState.back()));
		newone->start(prevone);
	}
}

void	CoreFSM::setStartState(const KigsID& id)
{
	if (mPossibleStates.find(id) != mPossibleStates.end())
	{
		if (!IsInit()) // if not init, init
		{
			Init();
		}
		pushCurrentState(mPossibleStates[id]);
	}
	else
	{
		KIGS_ERROR("FSM State not available", 2);
	}
}

void	CoreFSM::addState(const KigsID& id, CoreFSMStateBase* base)
{
	if (IsInit())
	{
		KIGS_ERROR("try to add states on an initialized FSM", 2);
		return;
	}

	if (mPossibleStates.find(id) != mPossibleStates.end()) // already there ?
	{
		if (mPossibleStates[id] != base)
		{
			KIGS_ERROR("try to add an already existing state", 2);
		}
		return;
	}
	mPossibleStates[id] = base;
}

void	CoreFSM::InitModifiable()
{
	if (IsInit()) // already init
	{
		return;
	}
	ParentClassType::InitModifiable();
	if (IsInit())
	{
		if (GetParents().size() || (mCurrentState.size()==0)) // can't init without parent or start state
		{
			mAttachedObject = GetParents()[0];
		}
		else
		{
			UnInit();
		}
	}
}

CoreFSM::~CoreFSM()
{
	KigsCore::GetCoreApplication()->RemoveAutoUpdate(this);

	while (mCurrentState.size())
	{
		popCurrentState();
	}

	for (auto state : mPossibleStates)
	{
		delete state.second;
	}

}


void	initCoreFSM()
{
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSM, CoreFSM, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMOnSignalTransition, CoreFSMOnSignalTransition, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMOnEventTransition, CoreFSMOnEventTransition, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMDelayTransition, CoreFSMDelayTransition, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMOnValueTransition, CoreFSMOnValueTransition, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMOnMethodTransition, CoreFSMOnMethodTransition, CoreFSM);
}