#pragma once
#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "AttributePacking.h"

class CoreFSM;
class CoreFSMStateBase;

// ****************************************
// * CoreFSMTransition class
// * --------------------------------------
/**
 * \class	CoreFSMTransition
 * \file	CoreFSMTransition.h
 * \ingroup CoreFSM
 * \brief	FSM transition base class.
 *
 */
 // ****************************************
class CoreFSMTransition : public CoreModifiable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(CoreFSMTransition, CoreModifiable, CoreFSM);
	DECLARE_CONSTRUCTOR(CoreFSMTransition);

	SIGNALS(ExecuteTransition);

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

	void	executeTransition(CoreFSMStateBase* from);

	virtual bool checkTransition(CoreModifiable* currentParentClass) = 0;

	KigsID				mGotoState="";

	maEnum<3>			mTransitionBehavior = BASE_ATTRIBUTE(TransitionBehavior, "Normal", "Pop", "Push");

	bool				mIsRunning = false;
};

// ****************************************
// * CoreFSMOnSignalTransition class
// * --------------------------------------
/**
 * \class	CoreFSMOnSignalTransition
 * \file	CoreFSMTransition.h
 * \ingroup CoreFSM
 * \brief	FSM transition activated on signal
 *
 */
 // ****************************************
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

	void	start() override;
	void	stop() override;

	WRAP_METHODS(receiveSignal);

protected:

	void	receiveSignal()
	{
		mSignalReceived = true;
	}

	bool			mSignalReceived = false;
	maReference		mConnectedClass = BASE_ATTRIBUTE(ConnectedClass, "");
	maString		mSignal = BASE_ATTRIBUTE(Signal, "");
};

// ****************************************
// * CoreFSMOnEventTransition class
// * --------------------------------------
/**
 * \class	CoreFSMOnEventTransition
 * \file	CoreFSMTransition.h
 * \ingroup CoreFSM
 * \brief	FSM transition activated on event
 *
 */
 // ****************************************
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

// ****************************************
// * CoreFSMDelayTransition class
// * --------------------------------------
/**
 * \class	CoreFSMDelayTransition
 * \file	CoreFSMTransition.h
 * \ingroup CoreFSM
 * \brief	FSM transition activated after given delay
 *
 */
 // ****************************************

class CoreFSMDelayTransition : public CoreFSMTransition
{
public:
	DECLARE_CLASS_INFO(CoreFSMDelayTransition, CoreFSMTransition, CoreFSM);
	DECLARE_INLINE_CONSTRUCTOR(CoreFSMDelayTransition)
	{

	}

	bool checkTransition(CoreModifiable* currentParentClass) override;
	double getRemainingTime();
	double getElapsedTime();

	virtual void	start() override;

protected:

	maDouble	mDelay = BASE_ATTRIBUTE(Delay, 1.0);

};

// ****************************************
// * CoreFSMOnValueTransition class
// * --------------------------------------
/**
 * \class	CoreFSMOnValueTransition
 * \file	CoreFSMTransition.h
 * \ingroup CoreFSM
 * \brief	FSM transition activated when a CoreAttribute is set on parent instance
 *
 */
 // ****************************************
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
	maBool		mNotValue = BASE_ATTRIBUTE(NotValue, false);

};

// ****************************************
// * CoreFSMOnMethodTransition class
// * --------------------------------------
/**
 * \class	CoreFSMOnMethodTransition
 * \file	CoreFSMTransition.h
 * \ingroup CoreFSM
 * \brief	FSM transition activated when a CoreMethod on parent instance return true
 *
 */
 // ****************************************
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

// ****************************************
// * CoreFSMInternalSetTransition class
// * --------------------------------------
/**
 * \class	CoreFSMInternalSetTransition
 * \file	CoreFSMTransition.h
 * \ingroup CoreFSM
 * \brief	FSM transition activated by the state itself
 *
 */
 // ****************************************

class CoreFSMInternalSetTransition : public CoreFSMTransition
{
public:
	DECLARE_CLASS_INFO(CoreFSMInternalSetTransition, CoreFSMTransition, CoreFSM);
	DECLARE_INLINE_CONSTRUCTOR(CoreFSMInternalSetTransition)
	{

	}

	bool checkTransition(CoreModifiable* currentParentClass) override;

protected:

};