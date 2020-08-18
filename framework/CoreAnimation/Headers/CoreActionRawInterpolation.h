#ifndef _COREACTIONRAWINTERPOLATION_H_
#define _COREACTIONRAWINTERPOLATION_H_

#include "CoreAction.h"
#include "CoreValue.h"

// interpolate raw values (directly set value on pointers)

template<typename dataType>
class CoreActionRawLinear : public CoreAction
{
public:
	
	CoreActionRawLinear() : CoreAction(), myPParamID(0)
	{}

	virtual void init(CoreSequence* sequence,CoreVector* params){}; // empty, raw action can not be directly init with json

protected:

	virtual bool	protectedUpdate(kdouble time)
	{
		CoreAction::protectedUpdate(time);
		if (myPParamID==0)
		{
			CoreModifiableAttribute* attr = mTarget->getAttribute(mParamID);
			myPParamID = (dataType*)attr->getRawValue();
		}
		if (myPParamID)
		{
			dataType result = myStart + (myEnd - myStart)*(kfloat)((time - mStartTime) / mDuration);
			*myPParamID = result;
		}
		return false;
	}

	dataType			myStart,myEnd;
	dataType*			myPParamID;
};

// hermite interpolation
// h(t) = f0 (2t^3 - 3t^2 +1 ) + f1 (-2t^3 + 3t^2) + f'0 (t^3 - 2t^2 + t) + f'1 (t^3 - t^2)

template<typename dataType>
class CoreActionRawHermite : public CoreAction
{
public:
	
	CoreActionRawHermite() : CoreAction(), myPParamID(0)
	{}

	virtual void init(CoreSequence* sequence,CoreVector* params){};// empty, raw action can not be directly init with json

	inline void coefs(kfloat& a0,kfloat& a1,kfloat& b0,kfloat& b1, kfloat t) // t in [0,1]
	{
		kfloat t2=t*t;
		kfloat t3=t2*t;
		a0=2*t3-3*t2+1;
		a1=-2*t3+3*t2;
		b0=t3-2*t2+t;
		b1=t3-t2;
	}

protected:

	virtual bool	protectedUpdate(kdouble time)
	{
		CoreAction::protectedUpdate(time);

		if (myPParamID == 0)
		{
			CoreModifiableAttribute* attr = mTarget->getAttribute(mParamID);
			myPParamID = (dataType*)attr->getRawValue();
		}
		if (myPParamID)
		{

			kfloat t = (kfloat)((time - mStartTime) / mDuration);

			kfloat a[4];
			coefs(a[0], a[1], a[2], a[3], t);

			dataType result = p[0] * a[0] + p[1] * a[1] + p[2] * a[2] + p[3] * a[3];

			*myPParamID = result;
		}
		return false;
	}

	dataType			p[4];
	dataType*			myPParamID;
};

template<typename dataType>
class CoreActionRawSetValue : public CoreAction
{
public:
	
	CoreActionRawSetValue() : CoreAction(), myPParamID(0)
	{}

	virtual void init(CoreSequence* sequence,CoreVector* params){};// empty, raw action can not be directly init with json

protected:

	virtual bool	protectedUpdate(kdouble time)
	{
		CoreAction::protectedUpdate(time);
		if (myPParamID == 0)
		{
			CoreModifiableAttribute* attr = mTarget->getAttribute(mParamID);
			myPParamID = (dataType*)attr->getRawValue();
		}
		if (myPParamID)
		{
			// wait the end of the action
			if ((time + TimeEpsilon) >= (mStartTime + mDuration))
			{
				setIsDone();
				*myPParamID = mySet;
				return true;
			}
		}
		return false;
	}

	dataType			mySet;
	dataType*			myPParamID;
};


#endif //_COREACTIONRAWINTERPOLATION_H_