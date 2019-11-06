#ifndef _COREACTIONDIRECTINTERPOLATION_H_
#define _COREACTIONDIRECTINTERPOLATION_H_

#include "CoreActionRawInterpolation.h"

template<typename dataType>
class CoreActionDirectLinear : public CoreActionRawLinear<dataType>
{
public:
	
	virtual void init(CoreSequence* sequence,CoreVector* params);

protected:

};

template<>
inline void CoreActionDirectLinear<kfloat>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vStart,kfloat vEnd,unsigned int paramID => 4 params
	if(!(params->size() == 4))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0].getValue(readfloat);
	myDuration=readfloat;

	(*params)[1].getValue(readfloat);
	myStart=readfloat;

	(*params)[2].getValue(readfloat);
	myEnd=readfloat;

	kstl::string readstring;
	(*params)[3].getValue(readstring);

	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

}


template<>
inline void CoreActionDirectLinear<Point2D>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vStart,kfloat vEnd,unsigned int paramID => 4 params
	if(!(params->size() == 4))
	{
		return;
	}
#endif

	float readfloat;
	(*params)[0].getValue(readfloat);
	myDuration=readfloat;

	Point2D	readPoint;
	((CoreVector&)(*params)[1]).getPoint2D(readPoint);
	myStart=readPoint;

	((CoreVector&)(*params)[2]).getPoint2D(readPoint);
	myEnd=readPoint;
	
	kstl::string readstring;
	(*params)[3].getValue(readstring);

	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

}

template<>
inline void CoreActionDirectLinear<Point3D>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vStart,kfloat vEnd,unsigned int paramID => 4 params
	if(!(params->size() == 4))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0].getValue(readfloat);
	myDuration=readfloat;

	Point3D	readPoint;
	((CoreVector&)(*params)[1]).getPoint3D(readPoint);
	myStart=readPoint;

	((CoreVector&)(*params)[2]).getPoint3D(readPoint);
	myEnd=readPoint;

	kstl::string readstring;
	(*params)[3].getValue(readstring);

	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

}

template<>
inline void CoreActionDirectLinear<Quaternion>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	// kdouble duration,kfloat vStart,kfloat vEnd,unsigned int paramID => 4 params
	if(!(params->size() == 4))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0].getValue(readfloat);
	myDuration=readfloat;

	Quaternion	readPoint;
	((CoreVector&)(*params)[1]).getPoint4D(readPoint);
	myStart=readPoint;

	((CoreVector&)(*params)[2]).getPoint4D(readPoint);
	myEnd=readPoint;

	kstl::string readstring;
	(*params)[3].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

}

// hermite interpolation
// h(t) = f0 (2t^3 - 3t^2 +1 ) + f1 (-2t^3 + 3t^2) + f'0 (t^3 - 2t^2 + t) + f'1 (t^3 - t^2)

template<typename dataType>
class CoreActionDirectHermite : public CoreActionRawHermite<dataType>
{
public:

	virtual void init(CoreSequence* sequence,CoreVector* params);

protected:

};


template<>
inline void CoreActionDirectHermite<kfloat>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	if(!(params->size() == 6))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0].getValue(readfloat);
	myDuration=readfloat;

	(*params)[1].getValue(readfloat);
	p[0]=readfloat;

	(*params)[2].getValue(readfloat);
	p[1]=readfloat;

	(*params)[3].getValue(readfloat);
	p[2]=readfloat;

	(*params)[4].getValue(readfloat);
	p[3]=readfloat;

	kstl::string readstring;
	(*params)[5].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);
}

template<>
inline void CoreActionDirectHermite<Point2D>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	if(!(params->size() == 6))
	{
		return;
	}
#endif

	float readfloat;
	(*params)[0].getValue(readfloat);
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
	(*params)[5].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);
}

template<>
inline void CoreActionDirectHermite<Point3D>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	if(!(params->size() == 6))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0].getValue(readfloat);
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
	(*params)[5].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

}

template<>
inline void CoreActionDirectHermite<Quaternion>::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();
#ifdef _DEBUG // test parameters count
	if(!(params->size() == 6))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0].getValue(readfloat);
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
	(*params)[5].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

}

template<typename dataType>
class CoreActionDirectSetValue : public CoreActionRawSetValue<dataType>
{
public:

	virtual void init(CoreSequence* sequence,CoreVector* params);

protected:

};


template<>
inline void CoreActionDirectSetValue<kfloat>::init(CoreSequence* sequence,CoreVector* params)
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
	(*params)[0].getValue(readfloat);
	myDuration=readfloat;

	(*params)[1].getValue(readfloat);
	mySet=readfloat;

	kstl::string readstring;
	(*params)[2].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

}

template<>
inline void CoreActionDirectSetValue<int>::init(CoreSequence* sequence,CoreVector* params)
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
	(*params)[0].getValue(readfloat);
	myDuration=readfloat;

	int readint;

	(*params)[1].getValue(readint);
	mySet=readint;

	kstl::string readstring;
	(*params)[2].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

}


template<>
inline void CoreActionDirectSetValue<bool>::init(CoreSequence* sequence,CoreVector* params)
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
	(*params)[0].getValue(readfloat);
	myDuration=readfloat;

	int readint;
	(*params)[1].getValue(readint);
	mySet=readint;

	kstl::string readstring;
	(*params)[2].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);

}


template<>
inline void CoreActionDirectSetValue<Point2D>::init(CoreSequence* sequence,CoreVector* params)
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
	(*params)[0].getValue(readfloat);
	myDuration=readfloat;

	Point2D	readPoint;
	((CoreVector&)(*params)[1]).getPoint2D(readPoint);
	mySet=readPoint;

	kstl::string readstring;
	(*params)[2].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID = CharToID::GetID(readstring);

}

template<>
inline void CoreActionDirectSetValue<Point3D>::init(CoreSequence* sequence,CoreVector* params)
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
	(*params)[0].getValue(readfloat);
	myDuration=readfloat;

	Point3D	readPoint;
	((CoreVector&)(*params)[1]).getPoint3D(readPoint);
	mySet=readPoint;

	kstl::string readstring;
	(*params)[2].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID = CharToID::GetID(readstring);

}

template<>
inline void CoreActionDirectSetValue<Quaternion>::init(CoreSequence* sequence,CoreVector* params)
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
	(*params)[0].getValue(readfloat);
	myDuration=readfloat;

	Quaternion	readPoint;
	((CoreVector&)(*params)[1]).getPoint4D(readPoint);
	mySet=readPoint;

	kstl::string readstring;
	(*params)[2].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID = CharToID::GetID(readstring);

}



#endif //_COREACTIONDIRECTINTERPOLATION_H_