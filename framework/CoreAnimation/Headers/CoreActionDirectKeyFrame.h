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

	virtual void init(CoreSequence* sequence,CoreVector* params);

protected:

};

template<>
inline void CoreActionDirectKeyFrame<kfloat>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();

	kstl::string readstring;
	(*params)[0].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

	// stock in list before creating the final array
	kstl::vector<float>		L_values;
	kstl::vector<kdouble>	L_times;
	float readfloat;
	unsigned int i;
	for(i=1;i<params->size();i+=2) // read each (time + val) couples
	{
		(*params)[i].getValue(readfloat);
		L_times.push_back(readfloat);
		(*params)[i+1].getValue(readfloat);
		L_values.push_back(readfloat);
	}

	if(L_values.size() == 0)
	{
		// mhh not good
		return;
	}

	myKeyFrameCount= (unsigned int)L_values.size();
	
	// duration correspond to last keyframe time
	myDuration=L_times[L_times.size()-1];

	myKeyFrameArray=new float[L_values.size()];
	myTimeArray=new kdouble[L_values.size()];

	for(i=0;i<L_values.size();i++) 
	{
		myKeyFrameArray[i]=L_values[i];
		myTimeArray[i]=L_times[i];
	}
}


template<>
inline void CoreActionDirectKeyFrame<int>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();

	kstl::string readstring;
	(*params)[0].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

	// stock in list before creating the final array
	kstl::vector<int>		L_values;
	kstl::vector<kdouble>	L_times;
	float readfloat;
	int readint;
	unsigned int i;
	for(i=1;i<params->size();i+=2) // read each (time + val) couples
	{
		(*params)[i].getValue(readfloat);
		L_times.push_back(readfloat);
		(*params)[i+1].getValue(readint);
		L_values.push_back(readint);
	}

	if(L_values.size() == 0)
	{
		// mhh not good
		return;
	}

	myKeyFrameCount= (unsigned int)L_values.size();
	
	// duration correspond to last keyframe time
	myDuration=L_times[L_times.size()-1];

	myKeyFrameArray=new int[L_values.size()];
	myTimeArray=new kdouble[L_values.size()];

	for(i=0;i<L_values.size();i++) 
	{
		myKeyFrameArray[i]=L_values[i];
		myTimeArray[i]=L_times[i];
	}
}

template<>
inline void CoreActionDirectKeyFrame<bool>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();

	kstl::string readstring;
	(*params)[0].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

	// stock in list before creating the final array
	kstl::vector<bool>		L_values;
	kstl::vector<kdouble>	L_times;
	float readfloat;
	int readint;
	unsigned int i;
	for(i=1;i<params->size();i+=2) // read each (time + val) couples
	{
		(*params)[i].getValue(readfloat);
		L_times.push_back(readfloat);
		(*params)[i+1].getValue(readint);
		L_values.push_back(readint);
	}

	if(L_values.size() == 0)
	{
		// mhh not good
		return;
	}

	myKeyFrameCount= (unsigned int)L_values.size();
	
	// duration correspond to last keyframe time
	myDuration=L_times[L_times.size()-1];

	myKeyFrameArray=new bool[L_values.size()];
	myTimeArray=new kdouble[L_values.size()];

	for(i=0;i<L_values.size();i++) 
	{
		myKeyFrameArray[i]=L_values[i];
		myTimeArray[i]=L_times[i];
	}
}


template<>
inline void CoreActionDirectKeyFrame<Point2D>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
		
	kstl::string readstring;
	(*params)[0].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

	// stock in list before creating the final array
	kstl::vector<Point2D>	L_values;
	kstl::vector<kdouble>	L_times;
	float readfloat;
	Point2D	readPoint;
	
	unsigned int i;
	for(i=1;i<params->size();i+=2) // read each (time + val) couples
	{
		(*params)[i].getValue(readfloat);
		L_times.push_back(readfloat);
		((CoreVector&)(*params)[i+1]).getPoint2D(readPoint);
		L_values.push_back(readPoint);
	}

	if(L_values.size() == 0)
	{
		// mhh not good
		return;
	}
		
	myKeyFrameCount= (unsigned int)L_values.size();
	myDuration=L_times[L_times.size()-1];

	myKeyFrameArray=new Point2D[L_values.size()];
	myTimeArray=new kdouble[L_values.size()];

	for(i=0;i<L_values.size();i++) 
	{
		myKeyFrameArray[i]=L_values[i];
		myTimeArray[i]=L_times[i];
	}
}

template<>
inline void CoreActionDirectKeyFrame<Point3D>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
		
	kstl::string readstring;
	(*params)[0].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

	// stock in list before creating the final array
	kstl::vector<Point3D>	L_values;
	kstl::vector<kdouble>	L_times;
	float readfloat;
	Point3D	readPoint;
	
	unsigned int i;
	for(i=1;i<params->size();i+=2) // read each (time + val) couples
	{
		(*params)[i].getValue(readfloat);
		L_times.push_back(readfloat);
		((CoreVector&)(*params)[i+1]).getPoint3D(readPoint);
		L_values.push_back(readPoint);

	}

	if(L_values.size() == 0)
	{
		// mhh not good
		return;
	}
	myKeyFrameCount= (unsigned int)L_values.size();
	myDuration=L_times[L_times.size()-1];

	myKeyFrameArray=new Point3D[L_values.size()];
	myTimeArray=new kdouble[L_values.size()];

	for(i=0;i<L_values.size();i++) 
	{
		myKeyFrameArray[i]=L_values[i];
		myTimeArray[i]=L_times[i];
	}
}

template<>
inline void CoreActionDirectKeyFrame<Quaternion>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
		
	kstl::string readstring;
	(*params)[0].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

	// stock in list before creating the final array
	kstl::vector<Quaternion>	L_values;
	kstl::vector<kdouble>	L_times;
	float readfloat;
	Quaternion	readPoint;
	
	unsigned int i;
	for(i=1;i<params->size();i+=2) // read each (time + val) couples
	{
		(*params)[i].getValue(readfloat);
		L_times.push_back(readfloat);
		((CoreVector&)(*params)[i+1]).getPoint4D(readPoint);
		L_values.push_back(readPoint);

	}

	if(L_values.size() == 0)
	{
		// mhh not good
		return;
	}
	myKeyFrameCount= (unsigned int)L_values.size();
	myDuration=L_times[L_times.size()-1];

	myKeyFrameArray=new Quaternion[L_values.size()];
	myTimeArray=new kdouble[L_values.size()];

	for(i=0;i<L_values.size();i++) 
	{
		myKeyFrameArray[i]=L_values[i];
		myTimeArray[i]=L_times[i];
	}
}



#endif //_COREACTIONDIRECTKEYFRAME_H_