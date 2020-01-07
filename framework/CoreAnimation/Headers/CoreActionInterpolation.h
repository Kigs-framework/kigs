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

	virtual void init(CoreSequence* sequence,CoreVector* params);

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

	virtual bool	protectedUpdate(kdouble time);

	void	protectedRelativeInit();

	dataType			myStart,myEnd;

	enum InterpolationFlags
	{
		StartRelative	=	1,
		EndRelative		=	2
	};

	bool	IsRelative()
	{
		return myActionFlags&(StartRelative|EndRelative);
	}

	bool IsStartRelative()
	{
		return myActionFlags&StartRelative;
	}
	bool IsEndRelative()
	{
		return myActionFlags&EndRelative;
	}
};

template<>
bool	CoreActionLinear<kfloat>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	kfloat result = (kfloat)(myStart + (myEnd - myStart)*((float)((time - myStartTime) / myDuration)));
	myTarget->setValue(myParamID,result);
	return false;
}

template<>
bool	CoreActionLinear<Point2D>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	Point2D result = (myStart + (myEnd - myStart)*((float)((time - myStartTime) / myDuration)));
	myTarget->setArrayValue(myParamID,&result.x,2);
	return false;
}


template<>
bool	CoreActionLinear<Point3D>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	Point3D result = (myStart + (myEnd - myStart)*((float)((time - myStartTime) / myDuration)));
	myTarget->setArrayValue(myParamID,&result.x,3);
	return false;
}

template<>
bool	CoreActionLinear<Quaternion>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	Quaternion result = (myStart + (myEnd - myStart)*((float)((time - myStartTime) / myDuration)));
	myTarget->setArrayValue(myParamID,&result.V.x,4);
	return false;
}

template<>
void	CoreActionLinear<kfloat>::protectedRelativeInit()
{
	CheckDelayTarget();
	kfloat currentval;
	if (myTarget->getValue(myParamID, currentval))
	{
		if (IsStartRelative())
			myStart += currentval;

		if (IsEndRelative())
			myEnd += currentval;
	}
}

template<>
void	CoreActionLinear<Point2D>::protectedRelativeInit()
{
	CheckDelayTarget();
	Point2D currentval;
	if (myTarget->getArrayValue(myParamID, &currentval.x,2))
	{
		if (IsStartRelative())
			myStart += currentval;

		if (IsEndRelative())
			myEnd += currentval;
	}
}


template<>
void	CoreActionLinear<Point3D>::protectedRelativeInit()
{
	CheckDelayTarget();
	Point3D currentval;
	if (myTarget->getArrayValue(myParamID, &currentval.x, 3))
	{
		if (IsStartRelative())
			myStart += currentval;
		
		if (IsEndRelative())
			myEnd += currentval;
	}
}

template<>
void	CoreActionLinear<Quaternion>::protectedRelativeInit()
{
	CheckDelayTarget();
	Quaternion currentval;
	if (myTarget->getArrayValue(myParamID, &currentval.V.x, 4))
	{
		if (IsStartRelative())
			myStart += currentval;
		
		if (IsEndRelative())
			myEnd += currentval;
	}
}


template<>
void CoreActionLinear<kfloat>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vStart,kfloat vEnd,unsigned int paramID => 4 params
	if((params->size()<4))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0]->getValue(readfloat);
	myDuration=readfloat;

	(*params)[1]->getValue(readfloat);
	myStart=readfloat;

	(*params)[2]->getValue(readfloat);
	myEnd=readfloat;

	kstl::string readstring;
	(*params)[3]->getValue(readstring);

	myTarget = checkSubTarget(readstring);
	myParamID=CharToID::GetID(readstring);

	// additionnal relative parameter ?
	if (params->size() == 5)
	{
		int readint;
		(*params)[4]->getValue(readint);

		switch (readint)
		{
		case 0:
			myActionFlags&= 0xFFFFFFFF^3;
			break;
		case 2:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			myActionFlags |= (unsigned int)(StartRelative);
			break;
		case 3:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			myActionFlags |= (unsigned int)(EndRelative);
			break;
		default:
			myActionFlags |= 3;
			break;
		}
	}
}

template<>
void CoreActionLinear<Point2D>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vStart,kfloat vEnd,unsigned int paramID => 4 params
	if ((params->size()<4))
	{
		return;
	}
#endif


	float readfloat;
	(*params)[0]->getValue(readfloat);
	myDuration=readfloat;

	Point2D	readPoint;
	((CoreVector&)(*params)[1]).getPoint2D(readPoint);
	myStart=readPoint;

	((CoreVector&)(*params)[2]).getPoint2D(readPoint);
	myEnd=readPoint;
	
	kstl::string readstring;
	(*params)[3]->getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

	// additionnal relative parameter ?
	if (params->size() == 5)
	{
		int readint;
		(*params)[4]->getValue(readint);
		switch (readint)
		{
		case 0:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			break;
		case 2:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			myActionFlags |= (unsigned int)(StartRelative);
			break;
		case 3:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			myActionFlags |= (unsigned int)(EndRelative);
			break;
		default:
			myActionFlags |= 3;
			break;
		}
	}
}

template<>
void CoreActionLinear<Point3D>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vStart,kfloat vEnd,unsigned int paramID => 4 params
	if ((params->size()<4))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0]->getValue(readfloat);
	myDuration=readfloat;

	Point3D	readPoint;
	((CoreVector&)(*params)[1]).getPoint3D(readPoint);
	myStart=readPoint;

	((CoreVector&)(*params)[2]).getPoint3D(readPoint);
	myEnd=readPoint;

	kstl::string readstring;
	(*params)[3]->getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

	// additionnal relative parameter ?
	if (params->size() == 5)
	{
		int readint;
		(*params)[4]->getValue(readint);
		switch (readint)
		{
		case 0:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			break;
		case 2:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			myActionFlags |= (unsigned int)(StartRelative);
			break;
		case 3:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			myActionFlags |= (unsigned int)(EndRelative);
			break;
		default:
			myActionFlags |= 3;
			break;
		}
	}
}

template<>
void CoreActionLinear<Quaternion>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vStart,kfloat vEnd,unsigned int paramID => 4 params
	if ((params->size()<4))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0]->getValue(readfloat);
	myDuration=readfloat;

	Quaternion	readPoint;
	((CoreVector&)(*params)[1]).getPoint4D(readPoint);
	myStart=readPoint;

	((CoreVector&)(*params)[2]).getPoint4D(readPoint);
	myEnd=readPoint;

	kstl::string readstring;
	(*params)[3]->getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

	// additionnal relative parameter ?
	if (params->size() == 5)
	{
		int readint;
		(*params)[4]->getValue(readint);
		switch (readint)
		{
		case 0:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			break;
		case 2:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			myActionFlags |= (unsigned int)(StartRelative);
			break;
		case 3:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			myActionFlags |= (unsigned int)(EndRelative);
			break;
		default:
			myActionFlags |= 3;
			break;
		}
	}
}

// hermite interpolation
// h(t) = f0 (2t^3 - 3t^2 +1 ) + f1 (-2t^3 + 3t^2) + f'0 (t^3 - 2t^2 + t) + f'1 (t^3 - t^2)

template<typename dataType>
class CoreActionHermite : public CoreAction
{
public:

	CoreActionHermite() : CoreAction()
	{}

	virtual void init(CoreSequence* sequence,CoreVector* params);

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

	virtual bool	protectedUpdate(kdouble time);

	void	protectedRelativeInit();

	dataType			p[4];

	enum InterpolationFlags
	{
		StartRelative = 1,
		EndRelative = 2
	};


	bool	IsRelative()
	{
		return myActionFlags&(StartRelative | EndRelative);
	}

	bool IsStartRelative()
	{
		return myActionFlags&StartRelative;
	}
	bool IsEndRelative()
	{
		return myActionFlags&EndRelative;
	}
};

template<>
bool	CoreActionHermite<kfloat>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	kfloat t=(kfloat)(((time-myStartTime)/myDuration));
	
	kfloat a[4];
	coefs(a[0],a[1],a[2],a[3],t);
	
	kfloat result=p[0]*a[0]+p[1]*a[1]+p[2]*a[2]+p[3]*a[3];

	myTarget->setValue(myParamID,result);
	return false;
}

template<>
bool	CoreActionHermite<Point2D>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	kfloat t=(kfloat)((time-myStartTime)/myDuration);
	kfloat a[4];
	coefs(a[0],a[1],a[2],a[3],t);
	
	Point2D result=p[0]*a[0]+p[1]*a[1]+p[2]*a[2]+p[3]*a[3];

	myTarget->setArrayValue(myParamID,&result.x,2);
	return false;
}


template<>
bool	CoreActionHermite<Point3D>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	kfloat t=(kfloat)((time-myStartTime)/myDuration);
	kfloat a[4];
	coefs(a[0],a[1],a[2],a[3],t);
	
	Point3D result=p[0]*a[0]+p[1]*a[1]+p[2]*a[2]+p[3]*a[3];

	myTarget->setArrayValue(myParamID,&result.x,3);
	return false;
}

template<>
bool	CoreActionHermite<Quaternion>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	kfloat t=(kfloat)((time-myStartTime)/myDuration);
	kfloat a[4];
	coefs(a[0],a[1],a[2],a[3],t);
	
	Quaternion result=p[0]*a[0]+p[1]*a[1]+p[2]*a[2]+p[3]*a[3];

	myTarget->setArrayValue(myParamID,&result.V.x,4);
	return false;
}


template<>
void	CoreActionHermite<kfloat>::protectedRelativeInit()
{
	CheckDelayTarget();
	kfloat currentval;
	if (myTarget->getValue(myParamID, currentval))
	{
		if (IsStartRelative())
			p[0] += currentval;
		
		if (IsEndRelative())
			p[1] += currentval;
	}
}

template<>
void	CoreActionHermite<Point2D>::protectedRelativeInit()
{
	CheckDelayTarget();
	Point2D currentval;
	if (myTarget->getArrayValue(myParamID, &currentval.x, 2))
	{
		if (IsStartRelative())
			p[0] += currentval;
		
		if (IsEndRelative())
			p[1] += currentval;
	}
}


template<>
void	CoreActionHermite<Point3D>::protectedRelativeInit()
{
	CheckDelayTarget();
	Point3D currentval;
	if (myTarget->getArrayValue(myParamID, &currentval.x, 3))
	{
		if (IsStartRelative())
			p[0] += currentval;
		
		if (IsEndRelative())
			p[1] += currentval;
	}
}

template<>
void	CoreActionHermite<Quaternion>::protectedRelativeInit()
{
	CheckDelayTarget();
	Quaternion currentval;
	if (myTarget->getArrayValue(myParamID, &currentval.V.x, 4))
	{
		if (IsStartRelative())
			p[0] += currentval;
		
		if (IsEndRelative())
			p[1] += currentval;
	}
}

template<>
void CoreActionHermite<kfloat>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	if((params->size() < 6))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0]->getValue(readfloat);
	myDuration=readfloat;

	(*params)[1]->getValue(readfloat);
	p[0]=readfloat;

	(*params)[2]->getValue(readfloat);
	p[1]=readfloat;

	(*params)[3]->getValue(readfloat);
	p[2]=readfloat;

	(*params)[4]->getValue(readfloat);
	p[3]=readfloat;

	kstl::string readstring;
	(*params)[5]->getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

	// additionnal relative parameter ?
	if (params->size() == 7)
	{
		int readint;
		(*params)[6]->getValue(readint);
		switch (readint)
		{
		case 0:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			break;
		case 2:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			myActionFlags |= (unsigned int)(StartRelative);
			break;
		case 3:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			myActionFlags |= (unsigned int)(EndRelative);
			break;
		default:
			myActionFlags |= 3;
			break;
		}
	}
}

template<>
void CoreActionHermite<Point2D>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	if ((params->size() < 6))
	{
		return;
	}
#endif

	float readfloat;
	(*params)[0]->getValue(readfloat);
	myDuration=readfloat;

	Point2D	readPoint;
	((CoreVector&)(*params)[1]).getPoint2D(readPoint);
	p[0]=readPoint;

	((CoreVector&)(*params)[2]).getPoint2D(readPoint);
	p[1]=readPoint;
	
	((CoreVector&)(*params)[3]).getPoint2D(readPoint);
	p[2]=readPoint;

	((CoreVector&)(*params)[4]).getPoint2D(readPoint);
	p[3]=readPoint;
	
	kstl::string readstring;
	(*params)[5]->getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

	// additionnal relative parameter ?
	if (params->size() == 7)
	{
		int readint;
		(*params)[6]->getValue(readint);
		switch (readint)
		{
		case 0:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			break;
		case 2:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			myActionFlags |= (unsigned int)(StartRelative);
			break;
		case 3:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			myActionFlags |= (unsigned int)(EndRelative);
			break;
		default:
			myActionFlags |= 3;
			break;
		}
	}
}

template<>
void CoreActionHermite<Point3D>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	if ((params->size() < 6))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0]->getValue(readfloat);
	myDuration=readfloat;

	Point3D	readPoint;
	((CoreVector&)(*params)[1]).getPoint3D(readPoint);
	p[0]=readPoint;

	((CoreVector&)(*params)[2]).getPoint3D(readPoint);
	p[1]=readPoint;

	((CoreVector&)(*params)[3]).getPoint3D(readPoint);
	p[2]=readPoint;

	((CoreVector&)(*params)[4]).getPoint3D(readPoint);
	p[3]=readPoint;

	kstl::string readstring;
	(*params)[5]->getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

	// additionnal relative parameter ?
	if (params->size() == 7)
	{
		int readint;
		(*params)[6]->getValue(readint);
		switch (readint)
		{
		case 0:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			break;
		case 2:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			myActionFlags |= (unsigned int)(StartRelative);
			break;
		case 3:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			myActionFlags |= (unsigned int)(EndRelative);
			break;
		default:
			myActionFlags |= 3;
			break;
		}
	}
}

template<>
void CoreActionHermite<Quaternion>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	if ((params->size() < 6))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0]->getValue(readfloat);
	myDuration=readfloat;

	Quaternion	readPoint;
	((CoreVector&)(*params)[1]).getPoint4D(readPoint);
	p[0]=readPoint;

	((CoreVector&)(*params)[2]).getPoint4D(readPoint);
	p[1]=readPoint;

	((CoreVector&)(*params)[3]).getPoint4D(readPoint);
	p[2]=readPoint;

	((CoreVector&)(*params)[4]).getPoint4D(readPoint);
	p[3]=readPoint;

	kstl::string readstring;
	(*params)[5]->getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

	// additionnal relative parameter ?
	if (params->size() == 7)
	{
		int readint;
		(*params)[6]->getValue(readint);
		switch (readint)
		{
		case 0:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			break;
		case 2:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			myActionFlags |= (unsigned int)(StartRelative);
			break;
		case 3:
			myActionFlags &= 0xFFFFFFFF ^ 3;
			myActionFlags |= (unsigned int)(EndRelative);
			break;
		default:
			myActionFlags |= 3;
			break;
		}
	}
}

template<typename dataType>
class CoreActionSetValue : public CoreAction
{
public:

	CoreActionSetValue() : CoreAction()
	{}

	virtual void init(CoreSequence* sequence,CoreVector* params);

protected:

	virtual bool	protectedUpdate(kdouble time);

	dataType			mySet;

};

template<>
bool	CoreActionSetValue<kfloat>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	// wait the end of the action
	if((time+TimeEpsilon)>=(myStartTime+myDuration))
	{
		setIsDone();
		myTarget->setValue(myParamID,mySet);
		return true;
	}
	return false;
}

template<>
bool	CoreActionSetValue<kstl::string>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	// wait the end of the action
	if ((time + TimeEpsilon) >= (myStartTime + myDuration))
	{
		setIsDone();
		myTarget->setValue(myParamID, mySet);
		return true;
	}
	return false;
}

template<>
bool	CoreActionSetValue<int>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	// wait the end of the action
	if((time+TimeEpsilon)>=(myStartTime+myDuration))
	{
		setIsDone();
		myTarget->setValue(myParamID,mySet);
		return true;
	}
	return false;
}

template<>
bool	CoreActionSetValue<bool>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	// wait the end of the action
	if((time+TimeEpsilon)>=(myStartTime+myDuration))
	{
		setIsDone();
		myTarget->setValue(myParamID,mySet);
		return true;
	}
	return false;
}


template<>
bool	CoreActionSetValue<Point2D>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	// wait the end of the action
	if((time+TimeEpsilon)>=(myStartTime+myDuration))
	{
		setIsDone();
		myTarget->setArrayValue(myParamID,&mySet.x,2);
		return true;
	}
	return false;
}


template<>
bool	CoreActionSetValue<Point3D>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	if((time+TimeEpsilon)>=(myStartTime+myDuration))
	{
		setIsDone();
		myTarget->setArrayValue(myParamID,&mySet.x,3);
		return true;
	}
	return false;
}

template<>
bool	CoreActionSetValue<Quaternion>::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	if((time+TimeEpsilon)>=(myStartTime+myDuration))
	{
		setIsDone();
		myTarget->setArrayValue(myParamID,&mySet.V.x,4);
		return true;
	}
	return false;
}

template<>
void CoreActionSetValue<kfloat>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vSet,unsigned int paramID => 3 params
	if(!(params->size() == 3))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0]->getValue(readfloat);
	myDuration=readfloat;

	(*params)[1]->getValue(readfloat);
	mySet=readfloat;

	kstl::string readstring;
	(*params)[2]->getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);
}

template<>
void CoreActionSetValue<kstl::string>::init(CoreSequence* sequence, CoreVector* params)
{
	myTarget = sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vSet,unsigned int paramID => 3 params
	if (!(params->size() == 3))
	{
		return;
	}
#endif
	float readfloat;
	kstl::string readstring;
	(*params)[0]->getValue(readfloat);
	myDuration = readfloat;

	(*params)[1]->getValue(readstring);
	mySet = readstring;

	(*params)[2]->getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID = CharToID::GetID(readstring);
}


template<>
void CoreActionSetValue<int>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vSet,unsigned int paramID => 3 params
	if(!(params->size() == 3))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0]->getValue(readfloat);
	myDuration=readfloat;

	int readint;
	(*params)[1]->getValue(readint);
	mySet=readint;

	kstl::string readstring;
	(*params)[2]->getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);
}

template<>
void CoreActionSetValue<bool>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vSet,unsigned int paramID => 3 params
	if(!(params->size() == 3))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0]->getValue(readfloat);
	myDuration=readfloat;

	int readint;
	(*params)[1]->getValue(readint);
	mySet=readint;

	kstl::string readstring;
	(*params)[2]->getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);
}


template<>
void CoreActionSetValue<Point2D>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vSet,unsigned int paramID => 3 params
	if(!(params->size() == 3))
	{
		return;
	}
#endif


	float readfloat;
	(*params)[0]->getValue(readfloat);
	myDuration=readfloat;

	Point2D	readPoint;
	((CoreVector&)(*params)[1]).getPoint2D(readPoint);
	mySet=readPoint;

	kstl::string readstring;
	(*params)[2]->getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);
}

template<>
void CoreActionSetValue<Point3D>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vSet,unsigned int paramID => 3 params
	if(!(params->size() == 3))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0]->getValue(readfloat);
	myDuration=readfloat;

	Point3D	readPoint;
	((CoreVector&)(*params)[1]).getPoint3D(readPoint);
	mySet=readPoint;

	kstl::string readstring;
	(*params)[2]->getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);
}

template<>
void CoreActionSetValue<Quaternion>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vSet,unsigned int paramID => 3 params
	if(!(params->size() == 3))
	{
		return;
	}
#endif

	float readfloat;
	(*params)[0]->getValue(readfloat);
	myDuration=readfloat;

	Quaternion	readPoint;
	((CoreVector&)(*params)[1]).getPoint4D(readPoint);
	mySet=readPoint;

	kstl::string readstring;
	(*params)[2]->getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);
}



#endif //_COREACTIONINTERPOLATION_H_