#ifndef _COREACTIONINTERPOLATION_H_
#define _COREACTIONINTERPOLATION_H_

#include "CoreAction.h"
#include "CoreValue.h"

template<typename dataType>
class CoreActionLinear : public CoreAction
{
public:

	CoreActionLinear() : CoreAction()
	{}

	virtual void init(CoreSequence* sequence,CoreVector* params)
	{
		mTarget = sequence->getTarget();
#ifdef _DEBUG // test parameters count
		// kdouble duration,kfloat vStart,kfloat vEnd,unsigned int paramID => 4 params
		if ((params->size() < 4))
		{
			return;
		}
#endif


		float readfloat;
		(*params)[0]->getValue(readfloat);
		mDuration = readfloat;

		dataType	readPoint;
		(*params)[1]->getValue(readPoint);
		mStart = readPoint;

		(*params)[2]->getValue(readPoint);
		mEnd = readPoint;

		kstl::string readstring;
		(*params)[3]->getValue(readstring);
		mTarget = checkSubTarget(readstring);

		mParamID = CharToID::GetID(readstring);

		// additionnal relative parameter ?
		if (params->size() == 5)
		{
			int readint;
			(*params)[4]->getValue(readint);
			switch (readint)
			{
			case 0:
				mActionFlags &= 0xFFFFFFFF ^ 3;
				break;
			case 2:
				mActionFlags &= 0xFFFFFFFF ^ 3;
				mActionFlags |= (unsigned int)(StartRelative);
				break;
			case 3:
				mActionFlags &= 0xFFFFFFFF ^ 3;
				mActionFlags |= (unsigned int)(EndRelative);
				break;
			default:
				mActionFlags |= 3;
				break;
			}
		}
	}

	virtual void	setStartTime(kdouble t)
	{
		CoreAction::setStartTime(t);
		if (IsRelative())
		{
			// recompute start and end from current val
			protectedRelativeInit();
		}
	}

protected:

	virtual bool	protectedUpdate(kdouble time)
	{
		CoreAction::protectedUpdate(time);
		dataType result = (dataType)(mStart + (mEnd - mStart) * ((float)((time - mStartTime) / mDuration)));
		mTarget->setValue(mParamID, result);
		return false;
	}

	void	protectedRelativeInit()
	{
		CheckDelayTarget();
		dataType currentval;
		if (mTarget->getValue(mParamID, currentval))
		{
			if (IsStartRelative())
				mStart += currentval;

			if (IsEndRelative())
				mEnd += currentval;
		}
	}

	dataType			mStart,mEnd;

	enum InterpolationFlags
	{
		StartRelative	=	1,
		EndRelative		=	2
	};

	bool	IsRelative()
	{
		return mActionFlags&(StartRelative|EndRelative);
	}

	bool IsStartRelative()
	{
		return mActionFlags&StartRelative;
	}
	bool IsEndRelative()
	{
		return mActionFlags&EndRelative;
	}
};

// hermite interpolation
// h(t) = f0 (2t^3 - 3t^2 +1 ) + f1 (-2t^3 + 3t^2) + f'0 (t^3 - 2t^2 + t) + f'1 (t^3 - t^2)

template<typename dataType>
class CoreActionHermite : public CoreAction
{
public:

	CoreActionHermite() : CoreAction()
	{}

	virtual void init(CoreSequence* sequence,CoreVector* params)
	{
		mTarget = sequence->getTarget();
#ifdef _DEBUG // test parameters count
		if ((params->size() < 6))
		{
			return;
		}
#endif

		float readfloat;
		(*params)[0]->getValue(readfloat);
		mDuration = readfloat;

		dataType	readPoint;
		(*params)[1]->getValue(readPoint);
		p[0] = readPoint;

		(*params)[2]->getValue(readPoint);
		p[1] = readPoint;

		(*params)[3]->getValue(readPoint);
		p[2] = readPoint;

		(*params)[4]->getValue(readPoint);
		p[3] = readPoint;

		kstl::string readstring;
		(*params)[5]->getValue(readstring);
		mTarget = checkSubTarget(readstring);

		mParamID = CharToID::GetID(readstring);

		// additionnal relative parameter ?
		if (params->size() == 7)
		{
			int readint;
			(*params)[6]->getValue(readint);
			switch (readint)
			{
			case 0:
				mActionFlags &= 0xFFFFFFFF ^ 3;
				break;
			case 2:
				mActionFlags &= 0xFFFFFFFF ^ 3;
				mActionFlags |= (unsigned int)(StartRelative);
				break;
			case 3:
				mActionFlags &= 0xFFFFFFFF ^ 3;
				mActionFlags |= (unsigned int)(EndRelative);
				break;
			default:
				mActionFlags |= 3;
				break;
			}
		}
	}


	inline void coefs(kfloat& a0,kfloat& a1,kfloat& b0,kfloat& b1, kfloat t) // t in [0,1]
	{
		kfloat t2=t*t;
		kfloat t3=t2*t;
		a0=2.0f*t3-3.0f*t2+1.0f;
		a1=-2.0f*t3+3.0f*t2;
		b0=t3-2.0f*t2+t;
		b1=t3-t2;
	}

	virtual void	setStartTime(kdouble t)
	{
		CoreAction::setStartTime(t);
		if (IsRelative())
		{
			// recompute start and end from current val
			protectedRelativeInit();
		}
	}

protected:

	virtual bool	protectedUpdate(kdouble time)
	{
		CoreAction::protectedUpdate(time);
		kfloat t = (kfloat)((time - mStartTime) / mDuration);
		kfloat a[4];
		coefs(a[0], a[1], a[2], a[3], t);

		dataType result = p[0] * a[0] + p[1] * a[1] + p[2] * a[2] + p[3] * a[3];

		mTarget->setValue(mParamID, result);
		return false;
	}

	void	protectedRelativeInit()
	{
		CheckDelayTarget();
		dataType currentval;
		if (mTarget->getValue(mParamID, currentval))
		{
			if (IsStartRelative())
				p[0] += currentval;

			if (IsEndRelative())
				p[1] += currentval;
		}
	}

	dataType			p[4];

	enum InterpolationFlags
	{
		StartRelative = 1,
		EndRelative = 2
	};


	bool	IsRelative()
	{
		return mActionFlags&(StartRelative | EndRelative);
	}

	bool IsStartRelative()
	{
		return mActionFlags&StartRelative;
	}
	bool IsEndRelative()
	{
		return mActionFlags&EndRelative;
	}
};

template<typename dataType>
class CoreActionSetValue : public CoreAction
{
public:

	CoreActionSetValue() : CoreAction()
	{}

	virtual void init(CoreSequence* sequence,CoreVector* params)
	{
		mTarget = sequence->getTarget();
#ifdef _DEBUG // test parameters count
		// kdouble duration,kfloat vSet,unsigned int paramID => 3 params
		if (!(params->size() == 3))
		{
			return;
		}
#endif


		float readfloat;
		(*params)[0]->getValue(readfloat);
		mDuration = readfloat;

		dataType	readVal;
		(*params)[1]->getValue(readVal);
		mSet = readVal;

		kstl::string readstring;
		(*params)[2]->getValue(readstring);
		mTarget = checkSubTarget(readstring);

		mParamID = CharToID::GetID(readstring);
	}

protected:

	virtual bool	protectedUpdate(kdouble time)
	{
		CoreAction::protectedUpdate(time);
		// wait the end of the action
		if ((time + TimeEpsilon) >= (mStartTime + mDuration))
		{
			setIsDone();
			mTarget->setValue(mParamID, mSet);
			return true;
		}
		return false;
	}

	dataType			mSet;

};

#endif //_COREACTIONINTERPOLATION_H_