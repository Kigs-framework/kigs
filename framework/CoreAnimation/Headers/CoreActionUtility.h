#ifndef _COREACTIONUTILITY_H_
#define _COREACTIONUTILITY_H_

#include "CoreAction.h"
#include "CoreValue.h"



// ****************************************
// * CoreActionRemoveFromParent class
// * --------------------------------------
/**
* \class	CoreActionRemoveFromParent
* \file		CoreActionUtility.h
* \ingroup CoreAnimation
* \brief	Remove the object owning this sequence from its parent of the given type.
*/
// ****************************************

class CoreActionRemoveFromParent : public CoreAction
{
public:
	CoreActionRemoveFromParent() : CoreAction()
	{mParentTypeID = 0xFFFFFFFF;}
	virtual void init(CoreSequence* sequence,CoreVector* params);
protected:

	virtual bool	protectedUpdate(kdouble time);
	unsigned int	mParentTypeID;
};

// ****************************************
// * CoreActionSendMessage class
// * --------------------------------------
/**
* \class	CoreActionSendMessage
* \file		CoreActionUtility.h
* \ingroup CoreAnimation
* \brief	Post a message ( notification )
*/
// ****************************************

class CoreActionSendMessage : public CoreAction
{
public:
	CoreActionSendMessage() : CoreAction()
	{;}
	virtual void init(CoreSequence* sequence,CoreVector* params);
protected:

	virtual bool	protectedUpdate(kdouble time);
	kstl::string	mMessage;
	usString		mParam;
};

// ****************************************
// * CoreActionEmitSignal class
// * --------------------------------------
/**
* \class	CoreActionEmitSignal
* \file		CoreActionUtility.h
* \ingroup CoreAnimation
* \brief	Emit a signal.
*/
// ****************************************
class CoreActionEmitSignal : public CoreAction
{
public:
	CoreActionEmitSignal() : CoreAction()
	{
		;
	}
	virtual void init(CoreSequence* sequence, CoreVector* params);
protected:

	virtual bool	protectedUpdate(kdouble time);
	kstl::string	mSignal;
	usString		mParam;
};

// ****************************************
// * CoreActionCombo class
// * --------------------------------------
/**
* \class	CoreActionCombo
* \file		CoreActionUtility.h
* \ingroup CoreAnimation
* \brief	Play all children animation together.
*/
// ****************************************

class CoreActionCombo : public CoreAction
{
public:
	CoreActionCombo() : CoreAction()
	{;}

	virtual ~CoreActionCombo();

	virtual void init(CoreSequence* sequence,CoreVector* params);

	virtual void	setStartTime(kdouble t);


protected:

	virtual bool	protectedUpdate(kdouble time);
	kstl::vector<SP<CoreAction>>	mList;
};

// ****************************************
// * CoreActionSerie class
// * --------------------------------------
/**
* \class	CoreActionSerie
* \file		CoreActionUtility.h
* \ingroup CoreAnimation
* \brief	Play each children animation one after the other.
*/
// ****************************************

class CoreActionSerie : public CoreAction
{
public:
	CoreActionSerie() : CoreAction() , mCurrentActionIndex(0)
	{;}

	virtual ~CoreActionSerie();

	virtual void init(CoreSequence* sequence,CoreVector* params);

	virtual void	setStartTime(kdouble t);

protected:

	virtual bool				protectedUpdate(kdouble time);
	kstl::vector<SP<CoreAction>>	mList;
	unsigned int				mCurrentActionIndex;
};

// ****************************************
// * CoreActionForLoop class
// * --------------------------------------
/**
* \class	CoreActionForLoop
* \file		CoreActionUtility.h
* \ingroup CoreAnimation
* \brief	Manage a "for" loop.
*/
// ****************************************
class CoreActionForLoop : public CoreAction
{
public:
	CoreActionForLoop() : CoreAction() , mLoopCount(0),mCurrentLoopIndex(0),mActionToLoop(0)
	{;}

	virtual ~CoreActionForLoop();

	virtual void	setStartTime(kdouble t);

	virtual void init(CoreSequence* sequence,CoreVector* params);

protected:

	virtual bool				protectedUpdate(kdouble time);
	int							mLoopCount;
	int							mCurrentLoopIndex;
	SP<CoreAction>					mActionToLoop;
};

// ****************************************
// * CoreActionDoWhile class
// * --------------------------------------
/**
* \class	CoreActionDoWhile
* \file		CoreActionUtility.h
* \ingroup CoreAnimation
* \brief	Manage a "do while" loop.
*/
// ****************************************
class CoreActionDoWhile : public CoreAction
{
public:
	CoreActionDoWhile() : CoreAction() , mActionToLoop(0),mIsZeroDuration(false)
	{;}

	virtual ~CoreActionDoWhile();

	virtual void	setStartTime(kdouble t);

	virtual void init(CoreSequence* sequence,CoreVector* params);

protected:

	virtual bool				protectedUpdate(kdouble time);
	SP<CoreAction>				mActionToLoop;
	bool						mIsZeroDuration;
};


#endif //_COREACTIONUTILITY_H_