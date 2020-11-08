#ifndef _COREVALUE_H
#define _COREVALUE_H

#include "Core.h"
#include "CoreItem.h"
#include "usString.h"



#ifdef _MSC_VER
#pragma warning( disable : 4996 )

#endif
class CoreVector;

// ****************************************
// * CoreValueBase class
// * --------------------------------------
/**
* \class	CoreValueBase
* \file		CoreValue.h
* \ingroup  Core
* \brief	Base class for CoreValue 
*/
// ****************************************

template<class T,class BaseClass>
class CoreValueBase : public BaseClass
{
protected:
	explicit CoreValueBase():BaseClass(CoreItem::COREVALUE),mValue(0){}

	CoreValueBase(CoreItem::COREITEM_TYPE _type, const T& _value) : BaseClass(_type), mValue(_value){}
	CoreValueBase(CoreItem::COREITEM_TYPE _type) : BaseClass(_type){}
public:
	
	virtual void set(int key, const CoreItemSP& toinsert) override
	{
		KIGS_ERROR("set called on CoreValue",1);
	}
	virtual void set(const kstl::string& key, const CoreItemSP& toinsert) override
	{
		KIGS_ERROR("set called on CoreValue", 1);
	}
	virtual void set(const usString& key, const CoreItemSP& toinsert) override
	{
		KIGS_ERROR("set called on CoreValue", 1);
	}

	virtual void erase(int key) override
	{
		KIGS_ERROR("erase called on CoreValue", 1);
	}
	virtual void erase(const kstl::string& key) override
	{
		KIGS_ERROR("erase called on CoreValue", 1);
	}
	virtual void erase(const usString& key) override
	{
		KIGS_ERROR("erase called on CoreValue", 1);
	}

	virtual inline operator bool() const override
	{
		return false;
	}

	virtual inline operator kfloat() const override
	{
		return 0.0f;
	}

	virtual inline operator double() const override
	{
		return 0.0f;
	}

	virtual inline operator int() const override
	{
		return 0;
	}

	virtual inline operator s64() const override
	{
		return 0;
	}

	virtual inline operator unsigned int() const override
	{
		return 0;
	}

	virtual inline operator u64() const override
	{
		return 0;
	}

	virtual inline operator kstl::string() const override
	{
		return "";
	}

	virtual inline operator usString() const override
	{
		return usString("");
	}

	virtual inline operator Point2D() const override
	{
		Point2D result;
		return result;
	}

	virtual inline operator Point3D() const override
	{
		Point3D result;
		return result;
	}

	virtual inline operator Vector4D() const override
	{
		Vector4D result;
		return result;
	}


	virtual bool operator==(const CoreItem& other) const override
	{
		return (mValue == other.operator T());
	}

	virtual kstl::string toString() const override { return "";}

	virtual bool isString() const override {return false;}

	T& getByRef()
	{
		return mValue;
	}

	T* getByPointer()
	{
		return &mValue;
	}

	operator T* ()
	{
		return &mValue;
	}

	virtual void*	getContainerStruct() override
	{
		return &mValue;
	}

protected:
	T  mValue;
};

template<>
inline CoreValueBase<bool,CoreItem>::operator bool() const
{
	return mValue;
}

template<>
inline CoreValueBase<bool,CoreNamedItem>::operator bool() const
{
	return mValue;
}

template<>
inline CoreValueBase<bool,CoreItem>::operator int() const
{
	return mValue?1:0;
}

template<>
inline CoreValueBase<bool, CoreItem>::operator s64() const
{
	return mValue ? 1 : 0;
}

template<>
inline CoreValueBase<bool, CoreItem>::operator unsigned int() const
{
	return mValue ? 1 : 0;
}

template<>
inline CoreValueBase<bool, CoreItem>::operator u64() const
{
	return mValue ? 1 : 0;
}

template<>
inline CoreValueBase<bool,CoreNamedItem>::operator  int() const
{
	return mValue ? 1 : 0;
}

template<>
inline CoreValueBase<bool, CoreNamedItem>::operator unsigned int() const
{
	return mValue ? 1 : 0;
}

template<>
inline CoreValueBase<bool,CoreItem>::operator  kfloat() const
{
	return mValue ? 1.0f : 0.0f;
}

template<>
inline CoreValueBase<bool, CoreItem>::operator double() const
{
	return mValue ? 1.0 : 0.0;
}

template<>
inline CoreValueBase<bool,CoreNamedItem>::operator  kfloat() const
{
	return mValue ? 1.0f : 0.0f;
}

template<>
inline CoreValueBase<bool, CoreNamedItem>::operator double() const
{
	return mValue ? 1.0 : 0.0;
}

template<>
inline CoreValueBase<bool,CoreItem>::operator  kstl::string() const
{
	return mValue ? "true" : "false";
}

template<>
inline CoreValueBase<bool,CoreNamedItem>::operator  kstl::string() const
{
	return mValue ? "true" : "false";
}


template<>
inline CoreValueBase<bool, CoreItem>::operator  usString() const
{
	return mValue ? usString("true") : usString("false");
}

template<>
inline CoreValueBase<bool, CoreNamedItem>::operator  usString() const
{
	return mValue ? usString("true") : usString("false");
}

#define DEFINE_COREVALUE_CAST_OPERATOR(basetype,itemtype,casttype) \
template<> \
inline CoreValueBase<basetype, itemtype>::operator casttype() const \
{ \
	return (casttype)mValue; \
} 

DEFINE_COREVALUE_CAST_OPERATOR(kfloat, CoreItem, kfloat);
DEFINE_COREVALUE_CAST_OPERATOR(kfloat, CoreNamedItem, kfloat);
DEFINE_COREVALUE_CAST_OPERATOR(kfloat, CoreItem, double);
DEFINE_COREVALUE_CAST_OPERATOR(kfloat, CoreNamedItem, double);
DEFINE_COREVALUE_CAST_OPERATOR(kfloat, CoreItem, int);
DEFINE_COREVALUE_CAST_OPERATOR(kfloat, CoreNamedItem, int);
DEFINE_COREVALUE_CAST_OPERATOR(kfloat, CoreItem, s64);
DEFINE_COREVALUE_CAST_OPERATOR(kfloat, CoreNamedItem, s64);
DEFINE_COREVALUE_CAST_OPERATOR(kfloat, CoreItem, unsigned int);
DEFINE_COREVALUE_CAST_OPERATOR(kfloat, CoreNamedItem, unsigned int);
DEFINE_COREVALUE_CAST_OPERATOR(kfloat, CoreItem, u64);
DEFINE_COREVALUE_CAST_OPERATOR(kfloat, CoreNamedItem, u64);

template<>
inline CoreValueBase<kfloat,CoreItem>::operator  kstl::string() const
{
	char L_Buffer[64] = {0};
	snprintf(L_Buffer, 64, "%f",(kfloat)mValue);
	return L_Buffer;
}

template<>
inline CoreValueBase<kfloat,CoreNamedItem>::operator  kstl::string() const
{
	char L_Buffer[64] = {0};
	snprintf(L_Buffer, 64, "%f",(kfloat)mValue);
	return L_Buffer;
}

template<>
inline CoreValueBase<kfloat, CoreItem>::operator  usString() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%f", (kfloat)mValue);
	return usString(L_Buffer);
}

template<>
inline CoreValueBase<kfloat, CoreNamedItem>::operator  usString() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%f", (kfloat)mValue);
	return usString(L_Buffer);
}

template<>
inline CoreValueBase<kfloat, CoreNamedItem>::operator  Point2D() const 
{
	Point2D _value;
	_value.x = (kfloat)mValue;
	_value.y = (kfloat)mValue;
	return _value;
}

template<>
inline CoreValueBase<kfloat, CoreNamedItem>::operator Point3D()  const
{
	Point3D _value;
	_value.x = (kfloat)mValue;
	_value.y = (kfloat)mValue;
	_value.z = (kfloat)mValue;
	return _value;
}

template<>
inline CoreValueBase<kfloat, CoreNamedItem>::operator Vector4D()  const
{
	Vector4D _value;
	_value.x = (kfloat)mValue;
	_value.y = (kfloat)mValue;
	_value.z = (kfloat)mValue;
	_value.w = (kfloat)mValue;
	return _value;
}


template<>
inline CoreValueBase<kfloat, CoreItem>::operator  Point2D() const
{
	Point2D _value;
	_value.x = (kfloat)mValue;
	_value.y = (kfloat)mValue;
	return _value;
}

template<>
inline CoreValueBase<kfloat, CoreItem>::operator Point3D()  const
{
	Point3D _value;
	_value.x = (kfloat)mValue;
	_value.y = (kfloat)mValue;
	_value.z = (kfloat)mValue;
	return _value;
}

template<>
inline CoreValueBase<kfloat, CoreItem>::operator Vector4D()  const
{
	Vector4D _value;
	_value.x = (kfloat)mValue;
	_value.y = (kfloat)mValue;
	_value.z = (kfloat)mValue;
	_value.w = (kfloat)mValue;
	return _value;
}

template<>
inline CoreValueBase<Point2D, CoreNamedItem>::operator  Point2D() const
{
	Point2D _value;
	_value.x = mValue.x;
	_value.y = mValue.y;
	return _value;
}

template<>
inline CoreValueBase<Point2D, CoreNamedItem>::operator Point3D()  const
{
	Point3D _value;
	_value.x = mValue.x;
	_value.y = mValue.y;
	_value.z = 0.0f;
	return _value;
}

template<>
inline CoreValueBase<Point2D, CoreNamedItem>::operator Vector4D()  const
{
	Vector4D _value;
	_value.x = mValue.x;
	_value.y = mValue.y;
	_value.z = 0.0f;
	_value.w = 0.0f;
	return _value;
}

template<>
inline CoreValueBase<Point2D, CoreItem>::operator  Point2D() const
{
	Point2D _value;
	_value.x = mValue.x;
	_value.y = mValue.y;
	return _value;
}

template<>
inline CoreValueBase<Point2D, CoreItem>::operator Point3D()  const
{
	Point3D _value;
	_value.x = mValue.x;
	_value.y = mValue.y;
	_value.z = 0.0f;
	return _value;
}

template<>
inline CoreValueBase<Point2D, CoreItem>::operator Vector4D()  const
{
	Vector4D _value;
	_value.x = mValue.x;
	_value.y = mValue.y;
	_value.z = 0.0f;
	_value.w = 0.0f;
	return _value;
}


template<>
inline CoreValueBase<Point3D, CoreNamedItem>::operator  Point2D() const
{
	Point2D _value;
	_value.x = mValue.x;
	_value.y = mValue.y;
	return _value;
}

template<>
inline CoreValueBase<Point3D, CoreNamedItem>::operator Point3D()  const
{
	Point3D _value;
	_value.x = mValue.x;
	_value.y = mValue.y;
	_value.z = mValue.z;
	return _value;
}

template<>
inline CoreValueBase<Point3D, CoreItem>::operator  Point2D() const
{
	Point2D _value;
	_value.x = mValue.x;
	_value.y = mValue.y;
	return _value;
}

template<>
inline CoreValueBase<Point3D, CoreItem>::operator Point3D()  const
{
	Point3D _value;
	_value.x = mValue.x;
	_value.y = mValue.y;
	_value.z = mValue.z;
	return _value;
}

DEFINE_COREVALUE_CAST_OPERATOR(int, CoreItem, kfloat);
DEFINE_COREVALUE_CAST_OPERATOR(int, CoreNamedItem, kfloat);
DEFINE_COREVALUE_CAST_OPERATOR(int, CoreItem, double);
DEFINE_COREVALUE_CAST_OPERATOR(int, CoreNamedItem, double);
DEFINE_COREVALUE_CAST_OPERATOR(int, CoreItem, int);
DEFINE_COREVALUE_CAST_OPERATOR(int, CoreNamedItem, int);
DEFINE_COREVALUE_CAST_OPERATOR(int, CoreItem, s64);
DEFINE_COREVALUE_CAST_OPERATOR(int, CoreNamedItem, s64);
DEFINE_COREVALUE_CAST_OPERATOR(int, CoreItem, unsigned int);
DEFINE_COREVALUE_CAST_OPERATOR(int, CoreNamedItem, unsigned int);
DEFINE_COREVALUE_CAST_OPERATOR(int, CoreItem, u64);
DEFINE_COREVALUE_CAST_OPERATOR(int, CoreNamedItem, u64);


template<>
inline CoreValueBase<int,CoreItem>::operator bool()  const
{
	return (mValue != 0);
}
template<>
inline CoreValueBase<int,CoreNamedItem>::operator bool()  const
{
	return (mValue != 0);
}
template<>
inline CoreValueBase<int,CoreItem>::operator  kstl::string() const
{
	char L_Buffer[64] = {0};
	snprintf(L_Buffer, 64, "%i",(int)mValue);
	return L_Buffer;
}
template<>
inline CoreValueBase<int,CoreNamedItem>::operator  kstl::string() const
{
	char L_Buffer[64] = {0};
	snprintf(L_Buffer, 64, "%i",(int)mValue);
	return L_Buffer;
}

template<>
inline CoreValueBase<int, CoreItem>::operator  usString() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", (int)mValue);
	return usString(L_Buffer);
}
template<>
inline CoreValueBase<int, CoreNamedItem>::operator  usString() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", (int)mValue);
	return usString(L_Buffer);
}

DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreItem, kfloat);
DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreNamedItem, kfloat);
DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreItem, double);
DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreNamedItem, double);
DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreItem, int);
DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreNamedItem, int);
DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreItem, s64);
DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreNamedItem, s64);
DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreItem, unsigned int);
DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreNamedItem, unsigned int);
DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreItem, u64);
DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreNamedItem, u64);


template<>
inline CoreValueBase<s64, CoreItem>::operator bool()  const
{
	return (mValue != 0);
}
template<>
inline CoreValueBase<s64, CoreNamedItem>::operator bool()  const
{
	return (mValue != 0);
}
template<>
inline CoreValueBase<s64, CoreItem>::operator kstl::string() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%lli", (s64)mValue);
	return L_Buffer;
}
template<>
inline CoreValueBase<s64, CoreNamedItem>::operator kstl::string() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%lli", (s64)mValue);
	return L_Buffer;
}

template<>
inline CoreValueBase<s64, CoreItem>::operator usString() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%lli", (s64)mValue);
	return usString(L_Buffer);
}
template<>
inline CoreValueBase<s64, CoreNamedItem>::operator usString() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%lli", (s64)mValue);
	return usString(L_Buffer);
}


DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreItem, kfloat);
DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreNamedItem, kfloat);
DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreItem, double);
DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreNamedItem, double);
DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreItem, int);
DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreNamedItem, int);
DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreItem, s64);
DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreNamedItem, s64);
DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreItem, unsigned int);
DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreNamedItem, unsigned int);
DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreItem, u64);
DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreNamedItem, u64);

template<>
inline CoreValueBase<unsigned int, CoreItem>::operator bool()  const
{
	return (mValue != 0);
}
template<>
inline CoreValueBase<unsigned int, CoreNamedItem>::operator bool()  const
{
	return (mValue != 0);
}
template<>
inline CoreValueBase<unsigned int, CoreItem>::operator kstl::string() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%u", (unsigned int)mValue);
	return L_Buffer;
}
template<>
inline CoreValueBase<unsigned int, CoreNamedItem>::operator kstl::string() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%u", (unsigned int)mValue);
	return L_Buffer;
}

template<>
inline CoreValueBase<unsigned int, CoreItem>::operator usString() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%u", (unsigned int)mValue);
	return usString(L_Buffer);
}
template<>
inline CoreValueBase<unsigned int, CoreNamedItem>::operator usString() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%u", (unsigned int)mValue);
	return usString(L_Buffer);
}

DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreItem, kfloat);
DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreNamedItem, kfloat);
DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreItem, double);
DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreNamedItem, double);
DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreItem, int);
DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreNamedItem, int);
DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreItem, s64);
DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreNamedItem, s64);
DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreItem, unsigned int);
DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreNamedItem, unsigned int);
DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreItem, u64);
DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreNamedItem, u64);


template<>
inline CoreValueBase<u64, CoreItem>::operator bool()  const
{
	return (mValue != 0);
}
template<>
inline CoreValueBase<u64, CoreNamedItem>::operator bool()  const
{
	return (mValue != 0);
}
template<>
inline CoreValueBase<u64, CoreItem>::operator kstl::string() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%llu", (u64)mValue);
	return L_Buffer;
}
template<>
inline CoreValueBase<u64, CoreNamedItem>::operator kstl::string() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%llu", (u64)mValue);
	return L_Buffer;
}

template<>
inline CoreValueBase<u64, CoreItem>::operator usString() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%llu", (u64)mValue);
	return usString(L_Buffer);
}
template<>
inline CoreValueBase<u64, CoreNamedItem>::operator usString() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%llu", (u64)mValue);
	return usString(L_Buffer);
}


template<>
inline CoreValueBase<kstl::string,CoreItem>::operator  kstl::string() const
{
	return mValue;
}
template<>
inline CoreValueBase<kstl::string,CoreNamedItem>::operator  kstl::string() const
{
	return mValue;
}

template<>
inline CoreValueBase<kstl::string, CoreItem>::operator  usString() const
{
	usString tmpval;
	tmpval=mValue;
	return tmpval;
}
template<>
inline CoreValueBase<kstl::string, CoreNamedItem>::operator  usString() const
{
	usString tmpval;
	tmpval = mValue;
	return tmpval;
}



template<>
inline CoreValueBase<usString, CoreItem>::operator  kstl::string() const
{
	return mValue.ToString();
}
template<>
inline CoreValueBase<usString, CoreNamedItem>::operator  kstl::string() const
{
	return mValue.ToString();
}

template<>
inline CoreValueBase<usString, CoreItem>::operator  usString() const
{
	return mValue;
}
template<>
inline CoreValueBase<usString, CoreNamedItem>::operator  usString() const
{
	return mValue;
}

template<>
inline bool CoreValueBase<usString, CoreNamedItem>::operator==(const CoreItem& other) const
{
	usString otherval;
	other.getValue(otherval);
	return (mValue == otherval);
}

template<>
inline bool CoreValueBase<usString, CoreItem>::operator==(const CoreItem& other) const
{
	usString otherval;
	other.getValue(otherval);
	return (mValue == otherval);
}

template<>
inline kstl::string CoreValueBase<kstl::string,CoreItem>::toString() const
{
	return mValue;
}
template<>
inline kstl::string CoreValueBase<kstl::string,CoreNamedItem>::toString() const
{
	return mValue;
}

template<>
inline kstl::string CoreValueBase<usString, CoreItem>::toString() const
{
	return mValue.ToString();
}
template<>
inline kstl::string CoreValueBase<usString, CoreNamedItem>::toString() const
{
	return mValue.ToString();
}

template<>
inline CoreValueBase<kstl::string,CoreItem>::operator  int() const
{
	return atoi(mValue.c_str());
}

template<>
inline CoreValueBase<kstl::string,CoreNamedItem>::operator  int() const
{
	return atoi(mValue.c_str());
}

template<>
inline CoreValueBase<kstl::string, CoreItem>::operator  kfloat() const
{
	return  (kfloat)atof(mValue.c_str());
}

template<>
inline CoreValueBase<kstl::string, CoreNamedItem>::operator  kfloat() const
{
	return (kfloat)atof(mValue.c_str());
}

template<>
inline CoreValueBase<kstl::string, CoreItem>::operator unsigned int() const
{
	return (unsigned int)atoi(mValue.c_str());
}

template<>
inline CoreValueBase<kstl::string, CoreNamedItem>::operator unsigned int() const
{
	return  (unsigned int)atoi(mValue.c_str());
}

template<>
inline bool CoreValueBase<kstl::string,CoreItem>::isString() const
{
	return true;
}

template<>
inline bool CoreValueBase<kstl::string,CoreNamedItem>::isString() const
{
	return true;
}

// usString
template<>
inline CoreValueBase<usString, CoreItem>::operator  int() const
{
	return atoi(mValue.ToString().c_str());
}

template<>
inline CoreValueBase<usString, CoreNamedItem>::operator  int() const
{
	return atoi(mValue.ToString().c_str());
}

template<>
inline CoreValueBase<usString, CoreItem>::operator  kfloat() const
{
	return (kfloat)atof(mValue.ToString().c_str());
}

template<>
inline CoreValueBase<usString, CoreNamedItem>::operator  kfloat() const
{
	return (kfloat)atof(mValue.ToString().c_str());
}

template<>
inline CoreValueBase<usString, CoreItem>::operator unsigned int() const
{
	return (unsigned int)atoi(mValue.ToString().c_str());
}

template<>
inline CoreValueBase<usString, CoreNamedItem>::operator unsigned int() const
{
	return (unsigned int)atoi(mValue.ToString().c_str());
}

template<>
inline bool CoreValueBase<usString, CoreItem>::isString() const
{
	return true;
}

template<>
inline bool CoreValueBase<usString, CoreNamedItem>::isString() const
{
	return true;
}

// ****************************************
// * CoreNamedValue class
// * --------------------------------------
/**
* \class	CoreNamedValue
* \file		CoreValue.h
* \ingroup  Core
* \brief	CoreNamedItem managing a single value
*/
// ****************************************

template<typename T>
class CoreNamedValue : public CoreValueBase<T,CoreNamedItem>
{
public:
	
	CoreNamedValue(const T& _value, const kstl::string& _name) : CoreValueBase<T, CoreNamedItem>(CoreItem::CORENAMEDVALUE, _value)
	{
		CoreNamedItem::mName = _name;
	}

	CoreNamedValue(const kstl::string& _name) : CoreValueBase<T, CoreNamedItem>(CoreItem::CORENAMEDVALUE)
	{
		CoreNamedItem::mName = _name;
	}

	/*template<typename valType>
	CoreNamedValue& operator= (const valType& mValue)
	{
		this->mValue = mValue;
		return *this;
	}*/
	virtual CoreItem& operator=(const bool& other)
	{
		this->mValue = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const kfloat& other)
	{
		this->mValue = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const int& other)
	{
		this->mValue = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const s64& other)
	{
		this->mValue = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const unsigned int& other)
	{
		this->mValue = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const u64& other)
	{
		this->mValue = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const kstl::string& other)
	{
		return *this;
	}
	virtual CoreItem& operator=(const usString& other)
	{
		return *this;
	}
	virtual CoreItem& operator=(const Point2D& other)
	{
		return *this;
	}
	virtual CoreItem& operator=(const Point3D& other)
	{
		return *this;
	}


protected:

};

template<> 
inline CoreItem& CoreNamedValue<kstl::string>::operator= (const kfloat& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%f", (kfloat)_value);
	this->mValue = L_Buffer;
	return *this;
}

template<>
inline CoreItem& CoreNamedValue<usString>::operator= (const kfloat& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%f", (kfloat)_value);
	this->mValue = usString(L_Buffer);
	return *this;
}
template<>
inline CoreItem& CoreNamedValue<kstl::string>::operator= (const int& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", _value);
	this->mValue = L_Buffer;
	return *this;
}

template<>
inline CoreItem& CoreNamedValue<kstl::string>::operator= (const s64& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%lli", _value);
	this->mValue = L_Buffer;
	return *this;
}

template<>
inline CoreItem& CoreNamedValue<usString>::operator= (const int& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", _value);
	this->mValue = usString(L_Buffer);
	return *this;
}

template<>
inline CoreItem& CoreNamedValue<usString>::operator= (const s64& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%lli", _value);
	this->mValue = usString(L_Buffer);
	return *this;
}

template<>
inline CoreItem& CoreNamedValue<kstl::string>::operator= (const unsigned int& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", _value);
	this->mValue = L_Buffer;
	return *this;
}

template<>
inline CoreItem& CoreNamedValue<usString>::operator= (const unsigned int& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", _value);
	this->mValue = usString(L_Buffer);
	return *this;
}

template<>
inline CoreItem& CoreNamedValue<kstl::string>::operator= (const u64& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%llu", _value);
	this->mValue = L_Buffer;
	return *this;
}

template<>
inline CoreItem& CoreNamedValue<usString>::operator= (const u64& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%llu", _value);
	this->mValue = usString(L_Buffer);
	return *this;
}

template<>
inline CoreItem& CoreNamedValue<kstl::string>::operator= (const bool& _value)
{
	if (_value)
		this->mValue = "true";
	else
		this->mValue = "false";
	return *this;
}

template<>
inline CoreItem& CoreNamedValue<usString>::operator= (const bool& _value)
{
	if (_value)
		this->mValue = usString("true");
	else
		this->mValue = usString("false");
	return *this;
}


// ****************************************
// * CoreValue class
// * --------------------------------------
/**
* \class	CoreValue
* \file		CoreValue.h
* \ingroup  Core
* \brief	CoreItem managing a single value
*/
// ****************************************

template<typename T>
class CoreValue : public CoreValueBase<T,CoreItem>
{
public:
	
	CoreValue(const T& _value) :
	CoreValueBase<T,CoreItem>(CoreItem::COREVALUE,_value)
	{
	}

	CoreValue() :
		CoreValueBase<T, CoreItem>(CoreItem::COREVALUE)
	{
	}

	virtual CoreModifiableAttribute* createAttribute(CoreModifiable*);

	/*template<typename valType>
	CoreValue& operator= (const valType& mValue)
	{
		this->mValue = mValue;
		return *this;
	}*/
	virtual CoreItem& operator=(const bool& other)
	{
		this->mValue = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const kfloat& other)
	{
		this->mValue = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const int& other)
	{
		this->mValue = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const s64& other)
	{
		this->mValue = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const unsigned int& other)
	{
		this->mValue = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const u64& other)
	{
		this->mValue = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const kstl::string& other)
	{
		KIGS_WARNING("trying to assign string value to non string CoreValue", 2);
		return *this;
	}
	virtual CoreItem& operator=(const usString& other)
	{
		KIGS_WARNING("trying to assign usString value to non usString CoreValue", 2);
		return *this;
	}
	virtual CoreItem& operator=(const Point2D& other)
	{
		KIGS_WARNING("trying to assign Point2D value to non Point2D CoreValue", 2);
		return *this;
	}
	virtual CoreItem& operator=(const Point3D& other)
	{
		KIGS_WARNING("trying to assign Point3D value to non Point3D CoreValue", 2);
		return *this;
	}

	virtual ~CoreValue()
	{}
};

template<>
inline CoreItem& CoreValue<kstl::string>::operator= (const kfloat& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%f", (kfloat)_value);
	this->mValue = L_Buffer;
	return *this;
}

template<>
inline CoreItem& CoreValue<kstl::string>::operator= (const kstl::string& _value)
{
	this->mValue = _value;
	return *this;
}


template<>
inline CoreItem& CoreValue<usString>::operator= (const kfloat& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%f", (kfloat)_value);
	this->mValue = usString(L_Buffer);
	return *this;
}

template<>
inline CoreItem& CoreValue<kstl::string>::operator= (const int& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", _value);
	this->mValue = L_Buffer;
	return *this;
}

template<>
inline CoreItem& CoreValue<usString>::operator= (const int& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", _value);
	this->mValue = usString(L_Buffer);
	return *this;
}

template<>
inline CoreItem& CoreValue<kstl::string>::operator= (const s64& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%lli", _value);
	this->mValue = L_Buffer;
	return *this;
}

template<>
inline CoreItem& CoreValue<usString>::operator= (const s64& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%lli", _value);
	this->mValue = usString(L_Buffer);
	return *this;
}

template<>
inline CoreItem& CoreValue<kstl::string>::operator= (const unsigned int& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", _value);
	this->mValue = L_Buffer;
	return *this;
}

template<>
inline CoreItem& CoreValue<usString>::operator= (const unsigned int& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", _value);
	this->mValue = usString(L_Buffer);
	return *this;
}

template<>
inline CoreItem& CoreValue<kstl::string>::operator= (const u64& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%llu", _value);
	this->mValue = L_Buffer;
	return *this;
}

template<>
inline CoreItem& CoreValue<usString>::operator= (const u64& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%llu", _value);
	this->mValue = usString(L_Buffer);
	return *this;
}


template<>
inline CoreItem& CoreValue<kstl::string>::operator= (const bool& _value)
{
	if(_value)
		this->mValue = "true";
	else 
		this->mValue = "false";
	return *this;
}

template<>
inline CoreItem& CoreValue<usString>::operator= (const bool& _value)
{
	if (_value)
		this->mValue = usString("true");
	else
		this->mValue = usString("false");
	return *this;
	return *this;
}

template<>
inline CoreItem& CoreValue<Point3D>::operator= (const bool& _value)
{
	return *this;
}
template<>
inline CoreItem& CoreValue<Point3D>::operator= (const float& _value)
{
	this->mValue.x = this->mValue.y = this->mValue.z = _value;
	return *this;
}
template<>
inline CoreItem& CoreValue<Point3D>::operator= (const int& _value)
{
	return *this;
}
template<>
inline CoreItem& CoreValue<Point3D>::operator= (const s64& _value)
{
	return *this;
}
template<>
inline CoreItem& CoreValue<Point3D>::operator= (const unsigned int& _value)
{
	return *this;
}
template<>
inline CoreItem& CoreValue<Point3D>::operator= (const u64& _value)
{
	return *this;
}


template<>
inline CoreItem& CoreValue<Point3D>::operator=(const Point2D& other)
{
	this->mValue.x = other.x;
	this->mValue.y = other.y;

	return *this;
}
template<>
inline CoreItem& CoreValue<Point3D>::operator=(const Point3D& other)
{
	this->mValue = other;
	return *this;
}

template<>
inline CoreItem& CoreValue<Point2D>::operator= (const bool& _value)
{
	return *this;
}
template<>
inline CoreItem& CoreValue<Point2D>::operator= (const float& _value)
{
	this->mValue.x = this->mValue.y = _value;
	return *this;
}
template<>
inline CoreItem& CoreValue<Point2D>::operator= (const int& _value)
{
	return *this;
}
template<>
inline CoreItem& CoreValue<Point2D>::operator= (const s64& _value)
{
	return *this;
}
template<>
inline CoreItem& CoreValue<Point2D>::operator= (const unsigned int& _value)
{
	return *this;
}
template<>
inline CoreItem& CoreValue<Point2D>::operator= (const u64& _value)
{
	return *this;
}
template<>
inline CoreItem& CoreValue<Point2D>::operator=(const Point2D& other)
{
	this->mValue = other;
	return *this;
}
template<>
inline CoreItem& CoreValue<Point2D>::operator=(const Point3D& other)
{
	this->mValue.x = other.x;
	this->mValue.y = other.y;
	return *this;
}
#endif // _COREVALUE_H