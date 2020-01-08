#ifndef _COREACTIONDIRECTKEYFRAME_H_
#define _COREACTIONDIRECTKEYFRAME_H_

#include "CoreAction.h"
#include "CoreValue.h"
#include "CoreActionKeyFrame.h"


template<typename dataType>
class CoreActionRawKeyFrame : public CoreActionKeyFrameBase
{
public:
	
	CoreActionRawKeyFrame() : CoreActionKeyFrameBase(), myPParamID(0)
	{}

	virtual ~CoreActionRawKeyFrame()
	{
		if(myKeyFrameArray)
			delete[] myKeyFrameArray;
	}

	virtual void init(CoreSequence* sequence,CoreVector* params){}; // empty, raw action can not be directly init with json

protected:

	virtual void	protectedSetValue(int index)
	{
		if (myPParamID == 0)
		{
			CoreModifiableAttribute* attr = myTarget->getAttribute(myParamID);
			myPParamID = (dataType*)attr->getRawValue();
		}
		if (myPParamID)
		{
			*myPParamID = myKeyFrameArray[index];
		}
	}

	dataType*			myPParamID;
	dataType*			myKeyFrameArray;
};

template<typename dataType>
class CoreActionDirectKeyFrame : public CoreActionRawKeyFrame<dataType>
{
public:
	
	CoreActionDirectKeyFrame() : CoreActionRawKeyFrame<dataType>()
	{}

	virtual void init(CoreSequence* sequence,CoreVector* params) override
	{
		this->myTarget = sequence->getTarget();

		kstl::string readstring;
		(*params)[0]->getValue(readstring);
		this->myTarget = this->checkSubTarget(readstring);

		this->myParamID = CharToID::GetID(readstring);

		// stock in list before creating the final array
		kstl::vector<dataType>	L_values;
		kstl::vector<kdouble>	L_times;
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

		this->myKeyFrameCount = (unsigned int)L_values.size();
		this->myDuration = L_times[L_times.size() - 1];

		this->myKeyFrameArray = new dataType[L_values.size()];
		this->myTimeArray = new kdouble[L_values.size()];

		for (i = 0; i < L_values.size(); i++)
		{
			this->myKeyFrameArray[i] = L_values[i];
			this->myTimeArray[i] = L_times[i];
		}
	}

protected:

};

#endif //_COREACTIONDIRECTKEYFRAME_H_