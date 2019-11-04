#ifndef _COREACTIONUTILITY_H_
#define _COREACTIONUTILITY_H_

#include "CoreAction.h"
#include "CoreValue.h"


class CoreActionRemoveFromParent : public CoreAction
{
public:
	CoreActionRemoveFromParent() : CoreAction()
	{myParentTypeID = 0xFFFFFFFF;}
	virtual void init(CoreSequence* sequence,CoreVector* params);
protected:

	virtual bool	protectedUpdate(kdouble time);
	unsigned int	myParentTypeID;
};


class CoreActionSendMessage : public CoreAction
{
public:
	CoreActionSendMessage() : CoreAction()
	{;}
	virtual void init(CoreSequence* sequence,CoreVector* params);
protected:

	virtual bool	protectedUpdate(kdouble time);
	kstl::string	myMessage;
	usString		myParam;
};

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
	kstl::string	mySignal;
	usString		myParam;
};

// several actions launched together

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
	kstl::vector<CoreAction*>	myList;
};

// one action to rule them all
class CoreActionSerie : public CoreAction
{
public:
	CoreActionSerie() : CoreAction() , myCurrentActionIndex(0)
	{;}

	virtual ~CoreActionSerie();

	virtual void init(CoreSequence* sequence,CoreVector* params);

	virtual void	setStartTime(kdouble t);

protected:

	virtual bool				protectedUpdate(kdouble time);
	kstl::vector<CoreAction*>	myList;
	unsigned int				myCurrentActionIndex;
};

// Loops

// do it again !
class CoreActionForLoop : public CoreAction
{
public:
	CoreActionForLoop() : CoreAction() , myLoopCount(0),myCurrentLoopIndex(0),myActionToLoop(0)
	{;}

	virtual ~CoreActionForLoop();

	virtual void	setStartTime(kdouble t);

	virtual void init(CoreSequence* sequence,CoreVector* params);

protected:

	virtual bool				protectedUpdate(kdouble time);
	int							myLoopCount;
	int							myCurrentLoopIndex;
	CoreAction*					myActionToLoop;
};

// do while
class CoreActionDoWhile : public CoreAction
{
public:
	CoreActionDoWhile() : CoreAction() , myActionToLoop(0),myIsZeroDuration(false)
	{;}

	virtual ~CoreActionDoWhile();

	virtual void	setStartTime(kdouble t);

	virtual void init(CoreSequence* sequence,CoreVector* params);

protected:

	virtual bool				protectedUpdate(kdouble time);
	CoreAction*					myActionToLoop;
	bool						myIsZeroDuration;
};


#endif //_COREACTIONUTILITY_H_