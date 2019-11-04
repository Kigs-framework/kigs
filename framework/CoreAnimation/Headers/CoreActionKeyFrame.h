#ifndef _COREACTIONKEYFRAME_H_
#define _COREACTIONKEYFRAME_H_

#include "CoreAction.h"
#include "CoreValue.h"

class CoreActionKeyFrameBase : public CoreAction
{
public:

	CoreActionKeyFrameBase() : CoreAction(),myKeyFrameCount(0),myLastKeyIndex(-1),myTimeArray(0)
	{
	}

	virtual void	setStartTime(kdouble t)
	{
		CoreAction::setStartTime(t);
		myLastKeyIndex=-1;
	}

	virtual ~CoreActionKeyFrameBase()
	{
		if(myTimeArray)
		{
			delete[]	myTimeArray;
		}
	}


protected:

	virtual bool	protectedUpdate(kdouble time);

	virtual void	protectedSetValue(int index)=0;

	unsigned int		myKeyFrameCount;
	int					myLastKeyIndex;

	kdouble*			myTimeArray;

};

template<typename dataType>
class CoreActionKeyFrame : public CoreActionKeyFrameBase
{
public:

	CoreActionKeyFrame() : CoreActionKeyFrameBase(),myKeyFrameArray(0)
	{}

	virtual void init(CoreSequence* sequence,CoreVector* params);

	virtual ~CoreActionKeyFrame()
	{
		if(myKeyFrameArray)
		{
			delete[]	myKeyFrameArray;
		}
	}

protected:

	inline void	protectedSetValue(int index);

	dataType*			myKeyFrameArray;
	
};

template<>
inline void	CoreActionKeyFrame<kfloat>::protectedSetValue(int index)
{
	myTarget->setValue(myParamID,myKeyFrameArray[index]);
}

template<>
inline void	CoreActionKeyFrame<int>::protectedSetValue(int index)
{
	myTarget->setValue(myParamID,myKeyFrameArray[index]);
}

template<>
inline void	CoreActionKeyFrame<bool>::protectedSetValue(int index)
{
	myTarget->setValue(myParamID,myKeyFrameArray[index]);
}

template<>
inline void	CoreActionKeyFrame<Point2D>::protectedSetValue(int index)
{
	myTarget->setArrayValue(myParamID,&(myKeyFrameArray[index].x),2);
}


template<>
inline void	CoreActionKeyFrame<Point3D>::protectedSetValue(int index)
{
	myTarget->setArrayValue(myParamID,&(myKeyFrameArray[index].x),3);
}

template<>
inline void	CoreActionKeyFrame<Quaternion>::protectedSetValue(int index)
{
	myTarget->setArrayValue(myParamID,&(myKeyFrameArray[index].V.x),4);
}

template<>
inline void CoreActionKeyFrame<kfloat>::init(CoreSequence* sequence,CoreVector* params)
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

	myKeyFrameCount=(unsigned int)L_values.size();
	
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
inline void CoreActionKeyFrame<int>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
		
	kstl::string readstring;
	(*params)[0].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

	// stock in list before creating the final array
	kstl::vector<int>	L_values;
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
inline void CoreActionKeyFrame<bool>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
		
	kstl::string readstring;
	(*params)[0].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

	// stock in list before creating the final array
	kstl::vector<bool>	L_values;
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
inline void CoreActionKeyFrame<Point2D>::init(CoreSequence* sequence,CoreVector* params)
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
inline void CoreActionKeyFrame<Point3D>::init(CoreSequence* sequence,CoreVector* params)
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
inline void CoreActionKeyFrame<Quaternion>::init(CoreSequence* sequence,CoreVector* params)
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

#endif //_COREACTIONKEYFRAME_H_