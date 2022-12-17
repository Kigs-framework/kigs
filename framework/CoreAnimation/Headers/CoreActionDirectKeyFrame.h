#ifndef _COREACTIONDIRECTKEYFRAME_H_
#define _COREACTIONDIRECTKEYFRAME_H_

#include "CoreAction.h"
#include "CoreValue.h"
#include "CoreActionKeyFrame.h"

// ****************************************
// * CoreActionRawKeyFrame class
// * --------------------------------------
/**
* \file	CoreActionDirectKeyFrame.h
* \class	CoreActionRawKeyFrame
* \ingroup CoreAnimation
* \brief	Base class for CoreActionDirectKeyFrame
*
*/
// ****************************************
template<typename dataType>
class CoreActionRawKeyFrame : public CoreActionKeyFrameBase
{
public:
	
	CoreActionRawKeyFrame() : CoreActionKeyFrameBase(), mPParamID(0)
	{}

	virtual ~CoreActionRawKeyFrame()
	{
		if(mKeyFrameArray)
			delete[] mKeyFrameArray;
	}

	virtual void init(CoreSequence* sequence,CoreVector* params){}; // empty, raw action can not be directly init with json

protected:

	virtual void	protectedSetValue(int index)
	{
		auto ptr = mTarget.lock();
		if (ptr)
		{
			if (mPParamID == 0)
			{
				CoreModifiableAttribute* attr = ptr->getAttribute(mParamID);
				mPParamID = (dataType*)attr->getRawValue();
			}
			if (mPParamID)
			{
				*mPParamID = mKeyFrameArray[index];
			}
		}
	}

	dataType*			mPParamID;
	dataType*			mKeyFrameArray;
};

// ****************************************
// * CoreActionDirectKeyFrame class
// * --------------------------------------
/**
* \file	CoreActionDirectKeyFrame.h
* \class	CoreActionDirectKeyFrame
* \ingroup CoreAnimation
* \brief	Direct access to animated value ( without setvalue )
*
*/
// ****************************************

template<typename dataType>
class CoreActionDirectKeyFrame : public CoreActionRawKeyFrame<dataType>
{
public:
	
	CoreActionDirectKeyFrame() : CoreActionRawKeyFrame<dataType>()
	{}

	virtual void init(CoreSequence* sequence,CoreVector* params) override
	{
		this->mTarget = sequence->getTarget();

		std::string readstring;
		(*params)[0]->getValue(readstring);
		this->mTarget = this->checkSubTarget(readstring);

		this->mParamID = CharToID::GetID(readstring);

		// stock in list before creating the final array
		std::vector<dataType>	L_values;
		std::vector<double>	L_times;
		float readfloat;
		dataType	readPoint;

		unsigned int i;
		for (i = 1; i < params->size(); i += 2) // read each (time + val) couples
		{
			(*params)[i]->getValue(readfloat);
			L_times.push_back(readfloat);
			(*params)[i + 1]->getValue(readPoint);
			L_values.push_back(readPoint);
		}

		if (L_values.size() == 0)
		{
			// mhh not good
			return;
		}

		this->mKeyFrameCount = (unsigned int)L_values.size();
		this->mDuration = L_times[L_times.size() - 1];

		this->mKeyFrameArray = new dataType[L_values.size()];
		this->mTimeArray = new double[L_values.size()];

		for (i = 0; i < L_values.size(); i++)
		{
			this->mKeyFrameArray[i] = L_values[i];
			this->mTimeArray[i] = L_times[i];
		}
	}

protected:

};

#endif //_COREACTIONDIRECTKEYFRAME_H_