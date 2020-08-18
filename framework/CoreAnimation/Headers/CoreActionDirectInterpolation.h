#ifndef _COREACTIONDIRECTINTERPOLATION_H_
#define _COREACTIONDIRECTINTERPOLATION_H_

#include "CoreActionRawInterpolation.h"

template<typename dataType>
class CoreActionDirectLinear : public CoreActionRawLinear<dataType>
{
public:
	
	virtual void init(CoreSequence* sequence,CoreVector* params)
	{
		this->mTarget = sequence->getTarget();
#ifdef _DEBUG // test parameters count
		// kdouble duration,kfloat vStart,kfloat vEnd,unsigned int paramID => 4 params
		if (!(params->size() == 4))
		{
			return;
		}
#endif

		float readfloat;
		(*params)[0]->getValue(readfloat);
		this->mDuration = readfloat;

		dataType	readPoint;
		(*params)[1]->getValue(readPoint);
		this->myStart = readPoint;

		(*params)[2]->getValue(readPoint);
		this->myEnd = readPoint;

		kstl::string readstring;
		(*params)[3]->getValue(readstring);

		this->mTarget = this->checkSubTarget(readstring);

		this->mParamID = CharToID::GetID(readstring);

	}

protected:

};


// hermite interpolation
// h(t) = f0 (2t^3 - 3t^2 +1 ) + f1 (-2t^3 + 3t^2) + f'0 (t^3 - 2t^2 + t) + f'1 (t^3 - t^2)

template<typename dataType>
class CoreActionDirectHermite : public CoreActionRawHermite<dataType>
{
public:

	virtual void init(CoreSequence* sequence,CoreVector* params)
	{
		this->mTarget = sequence->getTarget();
#ifdef _DEBUG // test parameters count
		if (!(params->size() == 6))
		{
			return;
		}
#endif

		float readfloat;
		(*params)[0]->getValue(readfloat);
		this->mDuration = readfloat;

		dataType	readPoint;
		(*params)[1]->getValue(readPoint);
		this->p[0] = readPoint;

		(*params)[2]->getValue(readPoint);
		this->p[1] = readPoint;

		(*params)[3]->getValue(readPoint);
		this->p[2] = readPoint;

		(*params)[4]->getValue(readPoint);
		this->p[3] = readPoint;

		kstl::string readstring;
		(*params)[5]->getValue(readstring);
		this->mTarget = this->checkSubTarget(readstring);

		this->mParamID = CharToID::GetID(readstring);
	}

protected:

};

template<typename dataType>
class CoreActionDirectSetValue : public CoreActionRawSetValue<dataType>
{
public:

	virtual void init(CoreSequence* sequence,CoreVector* params)
	{
		this->mTarget = sequence->getTarget();
#ifdef _DEBUG // test parameters count
		// kdouble duration,kfloat vSet,unsigned int paramID => 3 params
		if (!(params->size() == 3))
		{
			return;
		}
#endif


		float readfloat;
		(*params)[0]->getValue(readfloat);
		this->mDuration = readfloat;

		dataType	readPoint;
		(*params)[1]->getValue(readPoint);
		this->mySet = readPoint;

		kstl::string readstring;
		(*params)[2]->getValue(readstring);
		this->mTarget = this->checkSubTarget(readstring);

		this->mParamID = CharToID::GetID(readstring);

	}

protected:

};

#endif //_COREACTIONDIRECTINTERPOLATION_H_