#ifndef _COREACTIONFUNCTION_H_
#define _COREACTIONFUNCTION_H_

#include "CoreAction.h"
#include "CoreValue.h"
#include "CoreItemOperator.h"

// ****************************************
// * CoreItemAnimationContext class
// * --------------------------------------
/**
* \file	CoreActionFunction.h
* \class	CoreItemAnimationContext
* \ingroup CoreAnimation
* \brief	Special evaluation context for animations
*
*/
// ****************************************

class CoreItemAnimationContext : public CoreItemEvaluationContext
{
public:
	// current "global" time
	kdouble		mTime;
	// current action start time
	kdouble		mActionStartTime;
};

// ****************************************
// * CoreActionFunction class
// * --------------------------------------
/**
* \file	CoreActionFunction.h
* \class	CoreActionFunction
* \ingroup CoreAnimation
* \brief	Manage a CoreItemOperator kind of computation.
*
*/
// ****************************************

template<typename dataType, int dimension>
class CoreActionFunction : public CoreAction
{
public:

	CoreActionFunction() : CoreAction()
	{
		int i;
		for (i = 0; i < dimension; i++)
		{
			mFunctions[i] = nullptr;
		}
		mContext.mTime = mContext.mActionStartTime = -1.0;
	}

	virtual ~CoreActionFunction()
	{
		int i;
		for (i = 0; i < dimension; i++)
		{
			mFunctions[i] = nullptr;
		}

	}

	virtual void init(CoreSequence* sequence, CoreVector* params);

	virtual void	setStartTime(kdouble t)
	{
		CoreAction::setStartTime(t);
		if (mContext.mActionStartTime < -1.0)
		{
			mContext.mActionStartTime = t;
		}
	}

protected:

	virtual bool	protectedUpdate(kdouble time)
	{
		CoreAction::protectedUpdate(time);
		CoreItemEvaluationContext::SetContext(&mContext);
		mContext.mTime = time;
		dataType result;

		auto ptr = mTarget.lock();

		if (ptr && ptr->getValue(mParamID, result))
		{

			if (mHasUniqueMultidimensionnalFunc)
			{
				result = mFunctions[0]->operator dataType();
			}
			else
			{
				int i;
				for (i = 0; i < dimension; i++)
				{
					if (mFunctions[i])
					{
						result[i] = (float)*mFunctions[i];
					}
				}
			}
			ptr->setValue(mParamID, result);
		}
		CoreItemEvaluationContext::ReleaseContext();
		return false;
	}

	CoreItemSP	mFunctions[dimension];

	bool		mHasUniqueMultidimensionnalFunc=false;

	CoreItemAnimationContext	mContext;
};


template<>
inline bool	CoreActionFunction<kfloat,1>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	CoreItemEvaluationContext::SetContext(&mContext);
	mContext.mTime = time;
	if (mFunctions[0])
	{
		kfloat result = (kfloat)*mFunctions[0];
		auto ptr = mTarget.lock(); if (ptr) ptr->setValue(mParamID, result);
	}
	CoreItemEvaluationContext::ReleaseContext();
	return false;
}


typedef CoreActionFunction < kfloat, 1 > CoreActionFunction1D;
typedef CoreActionFunction < Point2D, 2 > CoreActionFunction2D;
typedef CoreActionFunction < Point3D, 3 > CoreActionFunction3D;
typedef CoreActionFunction < Vector4D, 4 > CoreActionFunction4D;


// ****************************************
// * ActionTimeOperator class
// * --------------------------------------
/**
* \file	CoreActionFunction.h
* \class	ActionTimeOperator
* \ingroup CoreAnimation
* \brief	Return current context time if available
*
*/
// ****************************************

template<typename operandType>
class ActionTimeOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const
	{
		if (CoreItemEvaluationContext::GetContext())
		{
			CoreItemAnimationContext& currentContext = *((CoreItemAnimationContext*)CoreItemEvaluationContext::GetContext());

			return (operandType)(currentContext.mTime-currentContext.mActionStartTime);
		}
		return ((operandType)0);
	}
	static std::unique_ptr<CoreVector> create()
	{
		return std::unique_ptr<CoreVector>(new ActionTimeOperator<operandType>());
	}

protected:
};

#endif //_COREACTIONFUNCTION_H_