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
			myFunctions[i] = nullptr;
		}
		myContext.myTime = myContext.myActionStartTime = -1.0;
	}

	virtual ~CoreActionFunction()
	{
		int i;
		for (i = 0; i < dimension; i++)
		{
			myFunctions[i] = nullptr;
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

	virtual bool	protectedUpdate(kdouble time)
	{
		CoreAction::protectedUpdate(time);
		SetCoreItemOperatorContext(&myContext);
		myContext.myTime = time;
		dataType result;
		if (myTarget->getValue(myParamID, result))
		{

			if (myHasUniqueMultidimensionnalFunc)
			{
				result = (dataType)myFunctions[0];
			}
			else
			{
				int i;
				for (i = 0; i < dimension; i++)
				{
					if (myFunctions[i])
					{
						result[i] = (float)myFunctions[i];
					}
				}
			}
			myTarget->setValue(myParamID, result);
		}
		ReleaseCoreItemOperatorContext();
		return false;
	}

	CoreItemSP	myFunctions[dimension];

	bool		myHasUniqueMultidimensionnalFunc=false;

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
		kfloat result = (kfloat)myFunctions[0];
		myTarget->setValue(myParamID, result);
	}
	ReleaseCoreItemOperatorContext();
	return false;
}


typedef CoreActionFunction < kfloat, 1 > CoreActionFunction1D;
typedef CoreActionFunction < Point2D, 2 > CoreActionFunction2D;
typedef CoreActionFunction < Point3D, 3 > CoreActionFunction3D;
typedef CoreActionFunction < Vector4D, 4 > CoreActionFunction4D;



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

#endif //_COREACTIONFUNCTION_H_