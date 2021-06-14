#pragma once
#include "CoreModifiable.h"


void	initCoreFSM();

class CoreFSMStateBase;

class CoreFSM : public CoreModifiable
{
public:

	enum class FSMStateSpecialOrder
	{
		NORMAL_TRANSITION	= 0,
		POP_TRANSITION		= 1,
		PUSH_TRANSITION		= 2
	};

	DECLARE_CLASS_INFO(CoreFSM, CoreModifiable, CoreFSM);
	DECLARE_CONSTRUCTOR(CoreFSM);

	void	addState(const KigsID& id, CoreFSMStateBase* base);
	void	setStartState(const KigsID& id);

	CoreFSMStateBase* getState(const KigsID& id);

	void	InitModifiable()override;

	void Update(const Timer& timer, void* addParam) override;

	virtual ~CoreFSM();

protected:

	void	pushCurrentState(CoreFSMStateBase*);
	void	changeCurrentState(CoreFSMStateBase*);
	void	popCurrentState();

	CoreModifiable*		mAttachedObject = nullptr;
	std::vector<CoreFSMStateBase*> mCurrentState;

	std::unordered_map<KigsID, CoreFSMStateBase*>	mPossibleStates;
};