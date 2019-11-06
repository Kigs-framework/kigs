#ifndef _COREACTIONFUNCTION_H_
#define _COREACTIONFUNCTION_H_

#include "CoreAction.h"
#include "CoreValue.h"
#include "CoreItemOperator.h"

class CoreItemAnimationContext : public CoreItemEvaluationContext
{
public:
	// current "global" time
	kdouble		myTime;
	// current action start time
	kdouble		myActionStartTime;
};

template<typename dataType, int dimension>
class CoreActionFunction : public CoreAction
{
public:

	CoreActionFunction() : CoreAction()
	{
		int i;
		for (i = 0; i < dimension; i++)
		{
			myFunctions[i] = 0;
		}
		myContext.myTime = myContext.myActionStartTime = -1.0;
	}

	virtual ~CoreActionFunction()
	{
		int i;
		for (i = 0; i < dimension; i++)
		{
			if (myFunctions[i])
			{
				myFunctions[i]->Destroy();
				myFunctions[i] = 0;
			}
		}

	}

	virtual void init(CoreSequence* sequence, CoreVector* params);

	virtual void	setStartTime(kdouble t)
	{
		CoreAction::setStartTime(t);
		if (myContext.myActionStartTime < -1.0)
		{
			myContext.myActionStartTime = t;
		}
	}

protected:

	virtual bool	protectedUpdate(kdouble time);

	CoreItem*	myFunctions[dimension];

	CoreItemAnimationContext	myContext;
};

template<>
inline bool	CoreActionFunction<kfloat,1>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	SetCoreItemOperatorContext(&myContext);
	myContext.myTime = time;
	if (myFunctions[0])
	{
		kfloat result = *myFunctions[0];
		myTarget->setValue(myParamID, result);
	}
	ReleaseCoreItemOperatorContext();
	return false;
}

template<>
inline bool	CoreActionFunction<Point2D,2>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	SetCoreItemOperatorContext(&myContext);
	myContext.myTime = time;
	Point2D result;
	if (myTarget->getArrayValue(myParamID, &result.x,2))
	{
		int i;
		for (i = 0; i < 2; i++)
		{
			if (myFunctions[i])
			{
				result[i] = *myFunctions[i];
			}
		}
		myTarget->setArrayValue(myParamID, &result.x,2);
	}
	ReleaseCoreItemOperatorContext();
	return false;
}


template<>
inline bool	CoreActionFunction<Point3D,3>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	SetCoreItemOperatorContext(&myContext);
	myContext.myTime = time; 
	Point3D result;
	if (myTarget->getArrayValue(myParamID, &result.x, 3))
	{
		int i;
		for (i = 0; i < 3; i++)
		{
			if (myFunctions[i])
			{
				result[i] = *myFunctions[i];
			}
		}
		myTarget->setArrayValue(myParamID, &result.x, 3);
	}
	ReleaseCoreItemOperatorContext();
	return false;
}

template<>
inline bool	CoreActionFunction<Quaternion,4>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	SetCoreItemOperatorContext(&myContext);
	myContext.myTime = time; 
	Quaternion result;
	if (myTarget->getArrayValue(myParamID, &result.V.x, 4))
	{
		int i;
		for (i = 0; i < 4; i++)
		{
			if (myFunctions[i])
			{
				result[i] = *myFunctions[i];
			}
		}
		myTarget->setArrayValue(myParamID, &result.V.x, 4);
	}
	ReleaseCoreItemOperatorContext();
	return false;
}



typedef CoreActionFunction < kfloat, 1 > CoreActionFunction1D;
typedef CoreActionFunction < Point2D, 2 > CoreActionFunction2D;
typedef CoreActionFunction < Point3D, 3 > CoreActionFunction3D;
typedef CoreActionFunction < Quaternion, 4 > CoreActionFunction4D;



// return current context time if available
template<typename operandType>
class ActionTimeOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const
	{
		if (myCurrentCoreItemEvaluationContext)
		{
			CoreItemAnimationContext& currentContext = *((CoreItemAnimationContext*)myCurrentCoreItemEvaluationContext);

			return (operandType)(currentContext.myTime-currentContext.myActionStartTime);
		}
		return ((operandType)0);
	}
	static CoreVector* create()
	{
		return new ActionTimeOperator<operandType>();
	}

protected:
};
/*
template<typename operandType>
class SequenceTimeOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const
	{
		if (myCurrentCoreItemEvaluationContext)
		{
			CoreItemAnimationContext& currentContext = *((CoreItemAnimationContext*)myCurrentCoreItemEvaluationContext);

			return (operandType)(currentContext.myTime - currentContext.mySequenceStartTime);
		}
		return ((operandType)0);
	}

	static CoreVector* create()
	{
		return new SequenceTimeOperator<operandType>();
	}

protected:

};
*/
#endif //_COREACTIONFUNCTION_H_