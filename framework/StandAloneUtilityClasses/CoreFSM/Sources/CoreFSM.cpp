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

  // set start state (setting the start state also init/start the FSM)
  fsm->setStartState("Appear");

*/

using namespace Kigs;
using namespace Kigs::Fsm;

SP<CoreFSMTransition> CoreFSM::mPopTransition=nullptr;

IMPLEMENT_CLASS_INFO(CoreFSM)

// constructor ask for autoupdate
CoreFSM::CoreFSM(const std::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
{
#ifdef DEBUG_COREFSM
	mStateChangeBuffer.init(100);
#endif

	// at least one block
	mFSMBlock.push_back({ (u32)-1,{}});
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
	auto transit= mFSMBlock[mCurrentBlockIndex].mCurrentState.back()->update(mAttachedObject, specialOrder, stateID);

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
		if ( (specialOrder == (u32)FSMStateSpecialOrder::PUSH_TRANSITION) || (specialOrder == (u32)FSMStateSpecialOrder::PUSHBLOCK_TRANSITION) )
		{
			u32 newblockindex(-1);

			if (specialOrder == (u32)FSMStateSpecialOrder::PUSHBLOCK_TRANSITION)
			{
				newblockindex=transit->getStateBlockIndex();
			}

			// then push state given it's ID
			pushCurrentState(getState(stateID, newblockindex), newblockindex);
		}
		else
		{
			// classic transition
			changeCurrentState(getState(stateID));
		}
	}

}
//! get state given by its name
/*  search in current block first, and if not found, return in other block if found */
CoreFSMStateBase* CoreFSM::getState(const KigsID& id,u32 blockindex) const
{
	if (blockindex == (u32)-1)
	{
		blockindex = mCurrentBlockIndex;
	}
	// if state is in the map
	auto f = mPossibleStates.find(id);

	if (f != mPossibleStates.end())
	{
		for (const auto& s : (*f).second)
		{
			if (s.second == blockindex)
			{
				// return the one in the good block
				return s.first;
			}
		}
		return (*f).second[0].first; // return first one
	}
	return nullptr;
}



//! get state on current state stack given by its name 
CoreFSMStateBase* CoreFSM::getStackedState(const KigsID& id) const
{

	// if state is in the current block
	auto f=getState(id);
	
	if (f)
	{
		size_t count = mFSMBlock[mCurrentBlockIndex].mCurrentState.size();
		if (count)
		{
			for (size_t i = count; i > 0; i--)
			{
				if (mFSMBlock[mCurrentBlockIndex].mCurrentState[i - 1] == f)
					return f;
			}
		}
	}
	return nullptr;
}


//! get state on current state stack given by pos : pos = 0 => currentState, pos = 1 => mCurrentState[mCurrentState.size()-2] ...
CoreFSMStateBase* CoreFSM::getStackedStateAt(size_t pos) const
{
	size_t count = mFSMBlock[mCurrentBlockIndex].mCurrentState.size();
	if (count > pos)
	{
		size_t i = count - 1 - pos; 
		return mFSMBlock[mCurrentBlockIndex].mCurrentState[i];
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
			case FSMStateSpecialOrder::PUSHBLOCK_TRANSITION:
				printf(" was pushed in other block\n");
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
	if (mFSMBlock[mCurrentBlockIndex].mCurrentState.size())
	{
		CoreFSMStateBase* prevone = mFSMBlock[mCurrentBlockIndex].mCurrentState.back();
		// stop previous state
		prevone->stop(mAttachedObject,newone);
		// downgrade object from previous state
		mAttachedObject->Downgrade(prevone->getID(),false,true);
		// change state
		mFSMBlock[mCurrentBlockIndex].mCurrentState.back() = newone;
		// upgrade object with new current state
		mAttachedObject->Upgrade(dynamic_cast<UpgradorBase*>(newone),false,true);
		// start new state
		newone->start(mAttachedObject,prevone);
#ifdef DEBUG_COREFSM
		mStateChangeBuffer.push_back({ KigsCore::GetCoreApplication()->GetApplicationTimer()->GetTime(),FSMStateSpecialOrder::NORMAL_TRANSITION ,newone });
#endif
	}
}

//! push the given state on the stack
void	CoreFSM::pushCurrentState(CoreFSMStateBase* newone, u32 newblockIndex)
{
	CoreFSMStateBase* prevone = nullptr;
	if (mFSMBlock[mCurrentBlockIndex].mCurrentState.size())
	{
		prevone = mFSMBlock[mCurrentBlockIndex].mCurrentState.back();
		// stop previous state
		prevone->stop(mAttachedObject,newone);
		// downgrade object from previous state but don't destroy it as we will get back there and don't detach methods
		mAttachedObject->Downgrade(mFSMBlock[mCurrentBlockIndex].mCurrentState.back()->getID(),false,false);
	}

#ifdef DEBUG_COREFSM
	mStateChangeBuffer.push_back({ KigsCore::GetCoreApplication()->GetApplicationTimer()->GetTime(),FSMStateSpecialOrder::PUSH_TRANSITION ,newone });
#endif

	if ((newblockIndex != (u32)-1) && (mCurrentBlockIndex != newblockIndex)) // change block if asked
	{
		mFSMBlock[newblockIndex].mFromBlock = mCurrentBlockIndex;
		mCurrentBlockIndex = newblockIndex;
	}

	// push state
	mFSMBlock[mCurrentBlockIndex].mCurrentState.push_back(newone);
	// upgrade object with new current state
	mAttachedObject->Upgrade(dynamic_cast<UpgradorBase*>(newone),false,true);
	// start new state
	newone->start(mAttachedObject,prevone);
}
void	CoreFSM::popCurrentState()
{
	CoreFSMStateBase* prevone = nullptr;
	CoreFSMStateBase* newone = nullptr;

	u32 changeBlock = mCurrentBlockIndex;

	if (mFSMBlock[mCurrentBlockIndex].mCurrentState.size() > 1)
	{
		// retrieve state to be started 
		newone = mFSMBlock[mCurrentBlockIndex].mCurrentState[mFSMBlock[mCurrentBlockIndex].mCurrentState.size() - 2];
	}
	else
	{
		if (mFSMBlock[mCurrentBlockIndex].mFromBlock != (u32)-1) // go to previous block
		{
			changeBlock = mFSMBlock[mCurrentBlockIndex].mFromBlock;

			if (mFSMBlock[changeBlock].mCurrentState.size())
			{
				newone = mFSMBlock[changeBlock].mCurrentState.back();
			}
			mFSMBlock[mCurrentBlockIndex].mFromBlock = -1;
		}
	}
	if (mFSMBlock[mCurrentBlockIndex].mCurrentState.size())
	{
		prevone = mFSMBlock[mCurrentBlockIndex].mCurrentState.back();
		// stop previous state
		prevone->stop(mAttachedObject,newone);
		// downgrade object from previous state
		mAttachedObject->Downgrade(mFSMBlock[mCurrentBlockIndex].mCurrentState.back()->getID(),false,true);
		// and pop the state
		mFSMBlock[mCurrentBlockIndex].mCurrentState.pop_back();
	}

#ifdef DEBUG_COREFSM
	mStateChangeBuffer.push_back({KigsCore::GetCoreApplication()->GetApplicationTimer()->GetTime(),FSMStateSpecialOrder::POP_TRANSITION ,newone });
#endif

	if (newone)
	{
		// if stack is not empty upgrade object with new stack back but don't init it again
		// reset methods in case previous pop with the same method name occurs
		mAttachedObject->Upgrade(dynamic_cast<UpgradorBase*>(newone),false,true);
		// and start upgrador
		newone->start(mAttachedObject,prevone);
	}

	mCurrentBlockIndex = changeBlock;
}

//! set FSM start state
void	CoreFSM::setStartState(const KigsID& id, u32 blockindex)
{
	auto itf = mPossibleStates.find(id);
	if (itf != mPossibleStates.end())
	{
		if (!IsInit()) // if not init, init (so start)
		{
			Init();
		}

		if (blockindex == (u32)-1)
		{
			blockindex = 0;
		}

		CoreFSMStateBase* foundstate = getState(id, blockindex);
		if (foundstate)
		{
			mCurrentBlockIndex = blockindex;
			if (mFSMBlock[mCurrentBlockIndex].mCurrentState.size() == 0)
			{
				// push state
				pushCurrentState(foundstate);
			}
			else
			{
				KIGS_ERROR("FSM start state already set", 2);
			}
		}
	}
	else
	{
		KIGS_ERROR("FSM State not available", 2);
	}
}

// return block index
u32	CoreFSM::addBlock()
{
	if (IsInit())
	{
		KIGS_ERROR("try to add states on an initialized FSM", 2);
		return -1;
	}
	mFSMBlock.push_back({ (u32)-1,{} });
	return mFSMBlock.size() - 1;
}

void CoreFSM::setCurrentBlock(u32 index)
{
	if (IsInit())
	{
		KIGS_ERROR("try to add states on an initialized FSM", 2);
		return;
	}
	if (index < mFSMBlock.size())
	{
		mCurrentBlockIndex = index;
	}
	else
	{
		KIGS_ERROR("try to set unknown block state", 2);
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

	auto f = mPossibleStates.find(id);
	if (f != mPossibleStates.end()) // check if already exists
	{
		for (auto s : (*f).second)
		{
			if (s.second == mCurrentBlockIndex)
			{
				KIGS_ERROR("try to add an already existing state", 2);
				return;
			}
		}
	}
	
	mPossibleStates[id].push_back({ base, mCurrentBlockIndex });
	
}

void CoreFSM::activateTransition(const KigsID& id)
{
	if (mFSMBlock[mCurrentBlockIndex].mCurrentState.size())
	{
		mFSMBlock[mCurrentBlockIndex].mCurrentState.back()->activateTransition(id);
	}
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
		if (GetParents().size() && (mFSMBlock[mCurrentBlockIndex].mCurrentState.size()==0)) // can't init without parent and start state
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

	while (mFSMBlock[mCurrentBlockIndex].mCurrentState.size())
	{
		popCurrentState();
	}

	for (auto statelist : mPossibleStates)
	{
		for (auto state : statelist.second)
		{
			delete state.first;
		}
	}
}

// declare all instanciable classes
void	Kigs::Fsm::initCoreFSM()
{
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSM, CoreFSM, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMOnSignalTransition, CoreFSMOnSignalTransition, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMOnEventTransition, CoreFSMOnEventTransition, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMDelayTransition, CoreFSMDelayTransition, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMOnValueTransition, CoreFSMOnValueTransition, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMOnMethodTransition, CoreFSMOnMethodTransition, CoreFSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreFSMInternalSetTransition, CoreFSMInternalSetTransition, CoreFSM);

	CoreFSM::initStaticCoreFSMInstances();
}

void	Kigs::Fsm::closeCoreFSM()
{
	CoreFSM::closeStaticCoreFSMInstances();
}

void CoreFSM::initStaticCoreFSMInstances()
{
	if (CoreFSM::mPopTransition == nullptr)
	{
		CoreFSM::mPopTransition = KigsCore::GetInstanceOf("gobalPopTransition", "CoreFSMInternalSetTransition");
		mPopTransition->setValue("TransitionBehavior", "Pop");
		mPopTransition->Init();
	}
}
void CoreFSM::closeStaticCoreFSMInstances()
{
	CoreFSM::mPopTransition = nullptr;
}