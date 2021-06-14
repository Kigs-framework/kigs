#pragma once
#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "AttributePacking.h"

class CoreFSM;
class CoreFSMStateBase;


class CoreFSMTransition : public CoreModifiable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(CoreFSMTransition, CoreModifiable, CoreFSM);
	DECLARE_CONSTRUCTOR(CoreFSMTransition);

	friend class CoreFSM;
	friend class CoreFSMStateBase;

	void	setState(const KigsID& gotoState)
	{
		mGotoState = gotoState;
	}
	const KigsID& getState() const
	{
		return mGotoState;
	}

	virtual void	start()
	{
		mIsRunning = true;
	}
	virtual void	stop()
	{
		mIsRunning = false;
	}

protected:
	virtual bool checkTransition(CoreModifiable* currentParentClass) = 0;

	KigsID				mGotoState="";

	maEnum<3>			mTransitionBehavior = BASE_ATTRIBUTE(TransitionBehavior, "Normal", "Pop", "Push");

	bool				mIsRunning = false;
};

// TODO ?
class CoreFSMOnSignalTransition : public CoreFSMTransition
{
public:
	DECLARE_CLASS_INFO(CoreFSMOnSignalTransition, CoreFSMTransition, CoreFSM);
	DECLARE_INLINE_CONSTRUCTOR(CoreFSMOnSignalTransition)
	{

	}

	bool checkTransition(CoreModifiable* currentParentClass) override
	{
		if (!mIsRunning)
		{
			KIGS_ERROR("check a not started transition", 1);
		}
		if (mSignalReceived)
		{
			mSignalReceived = false;
			return true;
		}
		return false;
	}

	void	start() override
	{
		ParentClassType::start();
		mSignalReceived = false;
	}
	void	stop() override
	{
		ParentClassType::stop();
		mSignalReceived = false;
	}

protected:

	bool		mSignalReceived = false;
};

class CoreFSMOnEventTransition : public CoreFSMTransition
{
public:
	DECLARE_CLASS_INFO(CoreFSMOnEventTransition, CoreFSMTransition, CoreFSM);
	DECLARE_INLINE_CONSTRUCTOR(CoreFSMOnEventTransition)
	{

	}

	bool checkTransition(CoreModifiable* currentParentClass) override
	{
		if (!mIsRunning)
		{
			KIGS_ERROR("check a not started transition", 1);
		}
		if (mEventReceived)
		{
			mEventReceived = false;
			return true;
		}
		return false;
	}

	virtual void	start() override;
	virtual void	stop() override;

	void	EventReceived()
	{
		mEventReceived = true;
	}

	WRAP_METHODS(EventReceived);

protected:

	bool		mEventReceived = false;
	maString	mEventName = BASE_ATTRIBUTE(EventName,"");
};

class CoreFSMDelayTransition : public CoreFSMTransition
{
public:
	DECLARE_CLASS_INFO(CoreFSMDelayTransition, CoreFSMTransition, CoreFSM);
	DECLARE_INLINE_CONSTRUCTOR(CoreFSMDelayTransition)
	{

	}

	bool checkTransition(CoreModifiable* currentParentClass) override;
	double getRemainingTime();

	virtual void	start() override;

protected:

	maDouble	mDelay = BASE_ATTRIBUTE(Delay, 1.0);

};

class CoreFSMOnValueTransition : public CoreFSMTransition
{
public:
	DECLARE_CLASS_INFO(CoreFSMOnValueTransition, CoreFSMTransition, CoreFSM);
	DECLARE_INLINE_CONSTRUCTOR(CoreFSMOnValueTransition)
	{

	}

	bool checkTransition(CoreModifiable* currentParentClass) override;

protected:

	maString	mValueName = BASE_ATTRIBUTE(ValueName, "");

};

class CoreFSMOnMethodTransition : public CoreFSMTransition
{
public:
	DECLARE_CLASS_INFO(CoreFSMOnMethodTransition, CoreFSMTransition, CoreFSM);
	DECLARE_INLINE_CONSTRUCTOR(CoreFSMOnMethodTransition)
	{

	}

	bool checkTransition(CoreModifiable* currentParentClass) override;

protected:

	maString	mMethodName = BASE_ATTRIBUTE(MethodName, "");

};