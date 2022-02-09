#pragma once
#include "CoreModifiable.h"

// to be called first
// declare FSM classes to instance factory
void	initCoreFSM();

// should be called to clear static instances 
void	closeCoreFSM();


#ifdef _DEBUG
// keep track of state changes
#define DEBUG_COREFSM	 
#endif

#ifdef DEBUG_COREFSM
#include "CoreRawBuffer.h"
#endif

class CoreFSMStateBase;
class CoreFSMTransition;


// ****************************************
// * CoreFSM class
// * --------------------------------------
/**
 * \class	CoreFSM
 * \file	CoreFSM.h
 * \ingroup CoreFSM
 * \brief	manage a finite state machine attached to an object.
 *
 */
 // ****************************************
class CoreFSM : public CoreModifiable
{
public:

	//! transition can ask to push/pop or just change the state
	enum class FSMStateSpecialOrder
	{
		NORMAL_TRANSITION	= 0,
		POP_TRANSITION		= 1,
		PUSH_TRANSITION		= 2
	};

	DECLARE_CLASS_INFO(CoreFSM, CoreModifiable, CoreFSM);
	DECLARE_CONSTRUCTOR(CoreFSM);

	//! declare a new possible state to the FSM
	void	addState(const KigsID& id, CoreFSMStateBase* base);

	//! set FSM start state
	void	setStartState(const KigsID& id);

	//! get state given by its name
	CoreFSMStateBase* getState(const KigsID& id);

	//! init method
	void	InitModifiable()override;

	//! update method
	void Update(const Timer& timer, void* addParam) override;

	//! destructor
	virtual ~CoreFSM();

	static	void initStaticCoreFSMInstances();
	static	void closeStaticCoreFSMInstances();


protected:

	//! state transition management
	//! push the given state on the stack
	void	pushCurrentState(CoreFSMStateBase*);
	//! change the current state on the stack
	void	changeCurrentState(CoreFSMStateBase*);
	//! pop the state on the stack => current state is the new stack back
	void	popCurrentState();

	//! the object the FSM is attached to
	CoreModifiable*		mAttachedObject = nullptr;

	//! state stack
	std::vector<CoreFSMStateBase*> mCurrentState;

	//! map of possible states for this FSM 
	std::unordered_map<KigsID, CoreFSMStateBase*>	mPossibleStates;

	friend class CoreFSMStateBase;
	static SP<CoreFSMTransition> mPopTransition;

#ifdef DEBUG_COREFSM

	struct trackStateChange
	{
		double					mTime;
		FSMStateSpecialOrder	mCause;
		CoreFSMStateBase*		mState;
	};

	CircularBuffer<trackStateChange>	mStateChangeBuffer;

public:
	void	dumpLastStates();

#endif
};