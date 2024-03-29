#pragma once
#include "CoreModifiable.h"

#ifdef _DEBUG
// keep track of state changes
#define DEBUG_COREFSM	 
#endif
#ifdef DEBUG_COREFSM
#include "CoreRawBuffer.h"
#endif

namespace Kigs
{
	namespace Fsm
	{
		using namespace Kigs::Core;
		// to be called first
		// declare FSM classes to instance factory
		void	initCoreFSM();

		// should be called to clear static instances 
		void	closeCoreFSM();


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
				NORMAL_TRANSITION = 0,
				POP_TRANSITION = 1,
				PUSH_TRANSITION = 2,
				PUSHBLOCK_TRANSITION = 3,
			};

			DECLARE_CLASS_INFO(CoreFSM, CoreModifiable, CoreFSM);
			DECLARE_CONSTRUCTOR(CoreFSM);

			//! declare a new possible state to the FSM
			void	addState(const KigsID& id, CoreFSMStateBase* base);

			//! set FSM start state
			void	setStartState(const KigsID& id, u32 blockindex = -1);

			//! get state given by its name (by default in current block)
			CoreFSMStateBase* getState(const KigsID& id, u32 blockindex = -1) const;

			//! get state on current state stack given by its name 
			CoreFSMStateBase* getStackedState(const KigsID& id) const;

			//! get state on current state stack given by pos : pos = 0 => currentState, pos = 1 => mCurrentState[mCurrentState.size()-2] ...
			CoreFSMStateBase* getStackedStateAt(size_t pos) const;

			void activateTransition(const KigsID& id);

			//! init method
			void	InitModifiable()override;

			//! update method
			void Update(const Time::Timer& timer, void* addParam) override;

			//! destructor
			virtual ~CoreFSM();

			static	void initStaticCoreFSMInstances();
			static	void closeStaticCoreFSMInstances();

			// return block index
			u32	addBlock();

			void setCurrentBlock(u32 index);

		protected:

			//! state transition management
			//! push the given state on the stack
			void	pushCurrentState(CoreFSMStateBase*, u32 newblockIndex = -1);
			//! change the current state on the stack
			void	changeCurrentState(CoreFSMStateBase*);
			//! pop the state on the stack => current state is the new stack back
			void	popCurrentState();

			//! the object the FSM is attached to
			CoreModifiable* mAttachedObject = nullptr;

			// to make it possible to have sereval time the same state (with a different context) in an FSM, manage list of blocks with their own stack
			struct FSMBlock
			{
				u32								mFromBlock = -1;
				//! state stack
				std::vector<CoreFSMStateBase*>	mCurrentState;
			};

			std::vector<FSMBlock>	mFSMBlock;
			u32						mCurrentBlockIndex = 0;

			//! map of possible states for this FSM 
			std::unordered_map<KigsID, std::vector<std::pair<CoreFSMStateBase*, u32>>>	mPossibleStates;

			friend class CoreFSMStateBase;
			static SP<CoreFSMTransition> mPopTransition;

#ifdef DEBUG_COREFSM

			struct trackStateChange
			{
				double					mTime;
				FSMStateSpecialOrder	mCause;
				CoreFSMStateBase* mState;
			};

			CircularBuffer<trackStateChange>	mStateChangeBuffer;

		public:
			void	dumpLastStates();

#endif
		};

#define getFSMState(fsm,baseclass,state) (CoreFSMStateClass(baseclass, state)*)fsm->getState(#state);
	}
}