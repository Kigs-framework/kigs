#include "CoreFSM.h"
#include "Core.h"
#include "CoreBaseApplication.h"
#include "CoreFSMState.h"


// an FSM instance is defined and added to an object like this :
// add FSM

/*

  SP<CoreFSM> fsm = KigsCore::GetInstanceOf("fsm", "CoreFSM");
  // add fsm to current instance
  addItem(fsm);
  // add state Appear
  fsm->addState("Appear", new CoreFSMStateClass(Ghost, Appear)());
  // create a wait transition
  SP<CoreFSMTransition> wait = KigsCore::GetInstanceOf("wait", "CoreFSMDelayTransition");
  // when wait transition is activated, ask to go to "FreeMove" state
  wait->setState("FreeMove");
  // init transition
  wait->Init();
  // add wait transition
  fsm->getState("Appear")->addTransition(wait);

  // add other states
  ...

  // set start state
  fsm->setStartState("Appear");
  // init FSM => start FSM
  fsm->Init();

*/


IMPLEMENT_CLASS_INFO(CoreFSM)

// constructor ask for autoupdate
CoreFSM::CoreFSM(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
{
#ifdef DEBUG_COREFSM
	mStateChangeBuffer.init(100);
#endif
}

void CoreFSM::Update(const Timer& timer, void* addParam) 
{
	if (!IsInit()) // if not init, try to init
	{
		Init();
	}
	// still not init, then return here
	if (!IsInit())
	{
		return;
	}

	u32 specialOrder = 0;
	KigsID stateID("");

	// update current state (check if a transition is needed)
	bool transit=mCurrentState.back()->update(mAttachedObject, specialOrder, stateID);

	// a transition is needed
	if (transit)
	{
		// state ask to pop
		if (specialOrder == (u32)FSMStateSpecialOrder::POP_TRANSITION)
		{
			popCurrentState();
			return;
		}

		// if transition ask a push
		if (specialOrder == (u32)FSMStateSpecialOrder::PUSH_TRANSITION)
		{
			// then push state given it's ID
			pushCurrentState(getState(stateID));
		}
		else
		{
			// classic transition
			changeCurrentState(getState(stateID));
		}
	}

}
//! get state given by its name
CoreFSMStateBase* CoreFSM::getState(const KigsID& id)
{
	// if state is in the map
	if (mPossibleStates.find(id) != mPossibleStates.end())
	{
		// return it
		return mPossibleStates[id];
	}
	return nullptr;
}

#ifdef DEBUG_COREFSM
void	CoreFSM::dumpLastStates()
{
	int bufsize = mStateChangeBuffer.size();
	if (bufsize)
	{
		printf("last state changes : \n");

		for (int todump = 0; todump < bufsize; todump++)
		{
			auto& current = mStateChangeBuffer[todump];

			printf("At %lf ", current.mTime);
			printf(" state %s ", current.mState->getID().toString().c_str());
			switch (current.mCause)
			{
			case FSMStateSpecialOrder::NORMAL_TRANSITION:
				printf(" was set \n");
				break;
			case FSMStateSpecialOrder::PUSH_TRANSITION:
				printf(" was pushed \n");
				break;
			case FSMStateSpecialOrder::POP_TRANSITION:
				printf(" was set after previous set was pop \n");
				break;
			}

		}
	}
}
#endif

// change the state on the stack by the given state
void	CoreFSM::changeCurrentState(CoreFSMStateBase* newone)
{
	if (mCurrentState.size())
	{
		CoreFSMStateBase* prevone = mCurrentState.back();
		// stop previous state
		prevone->stop(mAttachedObject,newone);
		// downgrade object from previous state
		mAttachedObject->Downgrade(prevone->getID());
		// change state
		mCurrentState.back() = newone;
		// upgrade object with new current state
		mAttachedObject->Upgrade(dynamic_cast<UpgradorBase*>(newone));
		// start new state
		newone->start(mAttachedObject,prevone);
#ifdef DEBUG_COREFSM
		mStateChangeBuffer.push_back({ KigsCore::GetCoreApplication()->GetApplicationTimer()->GetTime(),FSMStateSpecialOrder::NORMAL_TRANSITION ,newone });
#endif
	}
}

//! push the given state on the stack
void	CoreFSM::pushCurrentState(CoreFSMStateBase* newone)
{
	CoreFSMStateBase* prevone = nullptr;
	if (mCurrentState.size())
	{
		prevone = mCurrentState.back();
		// stop previous state
		prevone->stop(mAttachedObject,newone);
		// downgrade object from previous state
		mAttachedObject->Downgrade(mCurrentState.back()->getID());
	}

#ifdef DEBUG_COREFSM
	mStateChangeBuffer.push_back({ KigsCore::GetCoreApplication()->GetApplicationTimer()->GetTime(),FSMStateSpecialOrder::PUSH_TRANSITION ,newone });
#endif
	// push state
	mCurrentState.push_back(newone);
	// upgrade object with new current state
	mAttachedObject->Upgrade(dynamic_cast<UpgradorBase*>(newone));
	// start new state
	newone->start(mAttachedObject,prevone);
}
void	CoreFSM::popCurrentState()
{
	CoreFSMStateBase* prevone = nullptr;
	CoreFSMStateBase* newone = nullptr;

	if (mCurrentState.size() > 1)
	{
		// retrieve state to be started 
		newone = mCurrentState[mCurrentState.size() - 2];
	}
	if (mCurrentState.size())
	{
		prevone = mCurrentState.back();
		// stop previous state
		prevone->stop(mAttachedObject,newone);
		// downgrade object from previous state
		mAttachedObject->Downgrade(mCurrentState.back()->getID());
		// and pop the state
		mCurrentState.pop_back();
	}

#ifdef DEBUG_COREFSM
	mStateChangeBuffer.push_back({KigsCore::GetCoreApplication()->GetApplicationTimer()->GetTime(),FSMStateSpecialOrder::POP_TRANSITION ,newone });
#endif

	if (newone)
	{
		// if stack is not empty upgrade object with new stack back
		mAttachedObject->Upgrade(dynamic_cast<UpgradorBase*>(newone));
		// and start upgrador
		newone->start(mAttachedObject,prevone);
	}
}

//! set FSM start state
void	CoreFSM::setStartState(const KigsID& id)
{
	if (mPossibleStates.find(id) != mPossibleStates.end())
	{
		if (!IsInit()) // if not init, init
		{
			Init();
		}
		if (mCurrentState.size() == 0)
		{
			// push state
			pushCurrentState(mPossibleStates[id]);
		}
		else
		{
			KIGS_ERROR("FSM start state already set", 2);
		}
	}
	else
	{
		KIGS_ERROR("FSM State not available", 2);
	}
}

//! declare a new possible state to the FSM
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

// Init CoreFSM
void	CoreFSM::InitModifiable()
{
	if (IsInit()) // already init
	{
		return;
	}
	ParentClassType::InitModifiable();
	if (IsInit())
	{
		if (GetParents().size() && (mCurrentState.size()==0)) // can't init without parent and start state
		{
			mAttachedObject = GetParents()[0];
			// auto update only after init is OK
			KigsCore::GetCoreApplication()->AddAutoUpdate(this);
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

// declare all instanciable classes
void	initCoreFSM()
{
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSM, CoreFSM, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMOnSignalTransition, CoreFSMOnSignalTransition, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMOnEventTransition, CoreFSMOnEventTransition, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMDelayTransition, CoreFSMDelayTransition, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMOnValueTransition, CoreFSMOnValueTransition, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMOnMethodTransition, CoreFSMOnMethodTransition, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMInternalSetTransition, CoreFSMInternalSetTransition, CoreFSM);
}