#ifndef _COREVALUE_H
#define _COREVALUE_H

#include "Core.h"
#include "CoreItem.h"
#include "usString.h"

// ****************************************
// * CoreValue class
// * --------------------------------------
/**
* \class	CoreValue
* \ingroup  KigsCore
* \brief	
* \author	ukn
* \version ukn
* \date	ukn
*/
// ****************************************

#ifdef _MSC_VER
#pragma warning( disable : 4996 )

#endif
class CoreVector;

template<class T,class BaseClass>
class CoreValueBase : public BaseClass
{
protected:
	explicit CoreValueBase():BaseClass(CoreItem::COREVALUE),m_Value(0){}

	CoreValueBase(CoreItem::COREITEM_TYPE _type, const T& _value) : BaseClass(_type), m_Value(_value){}
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

	virtual inline operator unsigned int() const override
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

/*	virtual bool getValue(bool& _value) const {return false;}

	virtual bool getValue(kfloat& _value) const {return false;}

	virtual bool getValue(int& _value) const { return false; }

	virtual bool getValue(unsigned int& _value) const { return false; }

	virtual bool getValue(kstl::string& _value) const {return false;}

	virtual bool getValue(usString& _value) const { return false; }

	virtual bool getValue(Point2D& _value) const { return false; }

	virtual bool getValue(Point3D& _value) const { return false; }*/

	virtual bool operator==(const CoreItem& other) const override
	{
		return (m_Value == other.operator T());
	}

	virtual kstl::string toString() const override { return "";}

	virtual bool isString() const override {return false;}

	T& getByRef()
	{
		return m_Value;
	}

	T* getByPointer()
	{
		return &m_Value;
	}

	operator T* ()
	{
		return &m_Value;
	}

	virtual void*	getContainerStruct() override
	{
		return &m_Value;
	}

protected:
	T  m_Value;
};


template<>
inline CoreValueBase<bool,CoreItem>::operator bool() const
{
	return m_Value;
}

template<>
inline CoreValueBase<bool,CoreNamedItem>::operator bool() const
{
	return m_Value;
}

template<>
inline CoreValueBase<bool,CoreItem>::operator int() const
{
	return m_Value?1:0;
}

template<>
inline CoreValueBase<bool, CoreItem>::operator unsigned int() const
{
	return m_Value ? 1 : 0;
}

template<>
inline CoreValueBase<bool,CoreNamedItem>::operator  int() const
{
	return m_Value ? 1 : 0;
}

template<>
inline CoreValueBase<bool, CoreNamedItem>::operator unsigned int() const
{
	return m_Value ? 1 : 0;
}

template<>
inline CoreValueBase<bool,CoreItem>::operator  kfloat() const
{
	return m_Value ? 1.0f : 0.0f;
}

template<>
inline CoreValueBase<bool, CoreItem>::operator double() const
{
	return m_Value ? 1.0 : 0.0;
}

template<>
inline CoreValueBase<bool,CoreNamedItem>::operator  kfloat() const
{
	return m_Value ? 1.0f : 0.0f;
}

template<>
inline CoreValueBase<bool, CoreNamedItem>::operator double() const
{
	return m_Value ? 1.0 : 0.0;
}

template<>
inline CoreValueBase<bool,CoreItem>::operator  kstl::string() const
{
	return m_Value ? "true" : "false";
}

template<>
inline CoreValueBase<bool,CoreNamedItem>::operator  kstl::string() const
{
	return m_Value ? "true" : "false";
}


template<>
inline CoreValueBase<bool, CoreItem>::operator  usString() const
{
	return m_Value ? usString("true") : usString("false");
}

template<>
inline CoreValueBase<bool, CoreNamedItem>::operator  usString() const
{
	return m_Value ? usString("true") : usString("false");
}

template<>
inline CoreValueBase<kfloat,CoreItem>::operator  kfloat() const
{
	return m_Value;
}

template<>
inline CoreValueBase<kfloat,CoreNamedItem>::operator  kfloat() const
{
	return m_Value;
}

template<>
inline CoreValueBase<kfloat, CoreItem>::operator double() const
{
	return (double)m_Value;
}

template<>
inline CoreValueBase<kfloat, CoreNamedItem>::operator double() const
{
	return (double)m_Value;
}

template<>
inline CoreValueBase<kfloat,CoreItem>::operator  int() const
{
	return (int)m_Value;
}

template<>
inline CoreValueBase<kfloat,CoreNamedItem>::operator  int() const
{
	return (int)m_Value;
}

template<>
inline CoreValueBase<kfloat, CoreItem>::operator unsigned int() const
{
	return (unsigned int)m_Value;
}

template<>
inline CoreValueBase<kfloat, CoreNamedItem>::operator unsigned int() const
{
	return (unsigned int)m_Value;
}

template<>
inline CoreValueBase<kfloat,CoreItem>::operator  kstl::string() const
{
	char L_Buffer[64] = {0};
	snprintf(L_Buffer, 64, "%f",(kfloat)m_Value);
	return L_Buffer;
}

template<>
inline CoreValueBase<kfloat,CoreNamedItem>::operator  kstl::string() const
{
	char L_Buffer[64] = {0};
	snprintf(L_Buffer, 64, "%f",(kfloat)m_Value);
	return L_Buffer;
}

template<>
inline CoreValueBase<kfloat, CoreItem>::operator  usString() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%f", (kfloat)m_Value);
	return usString(L_Buffer);
}

template<>
inline CoreValueBase<kfloat, CoreNamedItem>::operator  usString() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%f", (kfloat)m_Value);
	return usString(L_Buffer);
}

template<>
inline CoreValueBase<kfloat, CoreNamedItem>::operator  Point2D() const 
{
	Point2D _value;
	_value.x = (kfloat)m_Value;
	_value.y = (kfloat)m_Value;
	return _value;
}

template<>
inline CoreValueBase<kfloat, CoreNamedItem>::operator Point3D()  const
{
	Point3D _value;
	_value.x = (kfloat)m_Value;
	_value.y = (kfloat)m_Value;
	_value.z = (kfloat)m_Value;
	return _value;
}

template<>
inline CoreValueBase<kfloat, CoreNamedItem>::operator Vector4D()  const
{
	Vector4D _value;
	_value.x = (kfloat)m_Value;
	_value.y = (kfloat)m_Value;
	_value.z = (kfloat)m_Value;
	_value.w = (kfloat)m_Value;
	return _value;
}


template<>
inline CoreValueBase<kfloat, CoreItem>::operator  Point2D() const
{
	Point2D _value;
	_value.x = (kfloat)m_Value;
	_value.y = (kfloat)m_Value;
	return _value;
}

template<>
inline CoreValueBase<kfloat, CoreItem>::operator Point3D()  const
{
	Point3D _value;
	_value.x = (kfloat)m_Value;
	_value.y = (kfloat)m_Value;
	_value.z = (kfloat)m_Value;
	return _value;
}

template<>
inline CoreValueBase<kfloat, CoreItem>::operator Vector4D()  const
{
	Vector4D _value;
	_value.x = (kfloat)m_Value;
	_value.y = (kfloat)m_Value;
	_value.z = (kfloat)m_Value;
	_value.w = (kfloat)m_Value;
	return _value;
}



template<>
inline CoreValueBase<Point2D, CoreNamedItem>::operator  Point2D() const
{
	Point2D _value;
	_value.x = m_Value.x;
	_value.y = m_Value.y;
	return _value;
}

template<>
inline CoreValueBase<Point2D, CoreNamedItem>::operator Point3D()  const
{
	Point3D _value;
	_value.x = m_Value.x;
	_value.y = m_Value.y;
	_value.z = 0.0f;
	return _value;
}

template<>
inline CoreValueBase<Point2D, CoreNamedItem>::operator Vector4D()  const
{
	Vector4D _value;
	_value.x = m_Value.x;
	_value.y = m_Value.y;
	_value.z = 0.0f;
	_value.w = 0.0f;
	return _value;
}

template<>
inline CoreValueBase<Point2D, CoreItem>::operator  Point2D() const
{
	Point2D _value;
	_value.x = m_Value.x;
	_value.y = m_Value.y;
	return _value;
}

template<>
inline CoreValueBase<Point2D, CoreItem>::operator Point3D()  const
{
	Point3D _value;
	_value.x = m_Value.x;
	_value.y = m_Value.y;
	_value.z = 0.0f;
	return _value;
}

template<>
inline CoreValueBase<Point2D, CoreItem>::operator Vector4D()  const
{
	Vector4D _value;
	_value.x = m_Value.x;
	_value.y = m_Value.y;
	_value.z = 0.0f;
	_value.w = 0.0f;
	return _value;
}


template<>
inline CoreValueBase<Point3D, CoreNamedItem>::operator  Point2D() const
{
	Point2D _value;
	_value.x = m_Value.x;
	_value.y = m_Value.y;
	return _value;
}

template<>
inline CoreValueBase<Point3D, CoreNamedItem>::operator Point3D()  const
{
	Point3D _value;
	_value.x = m_Value.x;
	_value.y = m_Value.y;
	_value.z = m_Value.z;
	return _value;
}

template<>
inline CoreValueBase<Point3D, CoreItem>::operator  Point2D() const
{
	Point2D _value;
	_value.x = m_Value.x;
	_value.y = m_Value.y;
	return _value;
}

template<>
inline CoreValueBase<Point3D, CoreItem>::operator Point3D()  const
{
	Point3D _value;
	_value.x = m_Value.x;
	_value.y = m_Value.y;
	_value.z = m_Value.z;
	return _value;
}



template<>
inline CoreValueBase<int,CoreItem>::operator  int() const
{
	return m_Value;
}

template<>
inline CoreValueBase<int,CoreNamedItem>::operator  int() const
{
	return m_Value;
}

template<>
inline CoreValueBase<int, CoreItem>::operator unsigned int() const
{
	return (unsigned int)m_Value;
}

template<>
inline CoreValueBase<int, CoreNamedItem>::operator unsigned int() const
{
	return  (unsigned int)m_Value;
}
template<>
inline CoreValueBase<int,CoreItem>::operator  kfloat() const
{
	return  (kfloat)m_Value;
}
template<>
inline CoreValueBase<int,CoreNamedItem>::operator  kfloat() const
{
	return  (kfloat)m_Value;
}

template<>
inline CoreValueBase<int,CoreItem>::operator bool()  const
{
	return (m_Value != 0);
}
template<>
inline CoreValueBase<int,CoreNamedItem>::operator bool()  const
{
	return (m_Value != 0);
}
template<>
inline CoreValueBase<int,CoreItem>::operator  kstl::string() const
{
	char L_Buffer[64] = {0};
	snprintf(L_Buffer, 64, "%i",(int)m_Value);
	return L_Buffer;
}
template<>
inline CoreValueBase<int,CoreNamedItem>::operator  kstl::string() const
{
	char L_Buffer[64] = {0};
	snprintf(L_Buffer, 64, "%i",(int)m_Value);
	return L_Buffer;
}

template<>
inline CoreValueBase<int, CoreItem>::operator  usString() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", (int)m_Value);
	return usString(L_Buffer);
}
template<>
inline CoreValueBase<int, CoreNamedItem>::operator  usString() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", (int)m_Value);
	return usString(L_Buffer);
}


template<>
inline CoreValueBase<unsigned int, CoreItem>::operator int() const
{
	return m_Value;
}

template<>
inline CoreValueBase<unsigned int, CoreNamedItem>::operator int() const
{
	return m_Value;
}

template<>
inline CoreValueBase<unsigned int, CoreItem>::operator unsigned int() const
{
	return (unsigned int)m_Value;
}

template<>
inline CoreValueBase<unsigned int, CoreNamedItem>::operator unsigned int() const
{
	return  (unsigned int)m_Value;
}
template<>
inline CoreValueBase<unsigned int, CoreItem>::operator kfloat() const
{
	return  (kfloat)m_Value;
}
template<>
inline CoreValueBase<unsigned int, CoreNamedItem>::operator kfloat() const
{
	return  (kfloat)m_Value;
}

template<>
inline CoreValueBase<unsigned int, CoreItem>::operator bool()  const
{
	return (m_Value != 0);
}
template<>
inline CoreValueBase<unsigned int, CoreNamedItem>::operator bool()  const
{
	return (m_Value != 0);
}
template<>
inline CoreValueBase<unsigned int, CoreItem>::operator kstl::string() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%u", (unsigned int)m_Value);
	return L_Buffer;
}
template<>
inline CoreValueBase<unsigned int, CoreNamedItem>::operator kstl::string() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%u", (unsigned int)m_Value);
	return L_Buffer;
}

template<>
inline CoreValueBase<unsigned int, CoreItem>::operator usString() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%u", (unsigned int)m_Value);
	return usString(L_Buffer);
}
template<>
inline CoreValueBase<unsigned int, CoreNamedItem>::operator usString() const
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%u", (unsigned int)m_Value);
	return usString(L_Buffer);
}



template<>
inline CoreValueBase<kstl::string,CoreItem>::operator  kstl::string() const
{
	return m_Value;
}
template<>
inline CoreValueBase<kstl::string,CoreNamedItem>::operator  kstl::string() const
{
	return m_Value;
}

template<>
inline CoreValueBase<kstl::string, CoreItem>::operator  usString() const
{
	usString tmpval;
	tmpval=m_Value;
	return tmpval;
}
template<>
inline CoreValueBase<kstl::string, CoreNamedItem>::operator  usString() const
{
	usString tmpval;
	tmpval = m_Value;
	return tmpval;
}



template<>
inline CoreValueBase<usString, CoreItem>::operator  kstl::string() const
{
	return m_Value.ToString();
}
template<>
inline CoreValueBase<usString, CoreNamedItem>::operator  kstl::string() const
{
	return m_Value.ToString();
}

template<>
inline CoreValueBase<usString, CoreItem>::operator  usString() const
{
	return m_Value;
}
template<>
inline CoreValueBase<usString, CoreNamedItem>::operator  usString() const
{
	return m_Value;
}

template<>
inline bool CoreValueBase<usString, CoreNamedItem>::operator==(const CoreItem& other) const
{
	usString otherval;
	other.getValue(otherval);
	return (m_Value == otherval);
}

template<>
inline bool CoreValueBase<usString, CoreItem>::operator==(const CoreItem& other) const
{
	usString otherval;
	other.getValue(otherval);
	return (m_Value == otherval);
}

template<>
inline kstl::string CoreValueBase<kstl::string,CoreItem>::toString() const
{
	return m_Value;
}
template<>
inline kstl::string CoreValueBase<kstl::string,CoreNamedItem>::toString() const
{
	return m_Value;
}

template<>
inline kstl::string CoreValueBase<usString, CoreItem>::toString() const
{
	return m_Value.ToString();
}
template<>
inline kstl::string CoreValueBase<usString, CoreNamedItem>::toString() const
{
	return m_Value.ToString();
}

template<>
inline CoreValueBase<kstl::string,CoreItem>::operator  int() const
{
	return atoi(m_Value.c_str());
}

template<>
inline CoreValueBase<kstl::string,CoreNamedItem>::operator  int() const
{
	return atoi(m_Value.c_str());
}

template<>
inline CoreValueBase<kstl::string, CoreItem>::operator  kfloat() const
{
	return  (kfloat)atof(m_Value.c_str());
}

template<>
inline CoreValueBase<kstl::string, CoreNamedItem>::operator  kfloat() const
{
	return (kfloat)atof(m_Value.c_str());
}

template<>
inline CoreValueBase<kstl::string, CoreItem>::operator unsigned int() const
{
	return (unsigned int)atoi(m_Value.c_str());
}

template<>
inline CoreValueBase<kstl::string, CoreNamedItem>::operator unsigned int() const
{
	return  (unsigned int)atoi(m_Value.c_str());
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
	return atoi(m_Value.ToString().c_str());
}

template<>
inline CoreValueBase<usString, CoreNamedItem>::operator  int() const
{
	return atoi(m_Value.ToString().c_str());
}

template<>
inline CoreValueBase<usString, CoreItem>::operator  kfloat() const
{
	return (kfloat)atof(m_Value.ToString().c_str());
}

template<>
inline CoreValueBase<usString, CoreNamedItem>::operator  kfloat() const
{
	return (kfloat)atof(m_Value.ToString().c_str());
}

template<>
inline CoreValueBase<usString, CoreItem>::operator unsigned int() const
{
	return (unsigned int)atoi(m_Value.ToString().c_str());
}

template<>
inline CoreValueBase<usString, CoreNamedItem>::operator unsigned int() const
{
	return (unsigned int)atoi(m_Value.ToString().c_str());
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

template<typename T>
class CoreNamedValue : public CoreValueBase<T,CoreNamedItem>
{
public:
	
	CoreNamedValue(const T& _value, const kstl::string& _name) : CoreValueBase<T, CoreNamedItem>(CoreItem::CORENAMEDVALUE, _value)
	{
		CoreNamedItem::m_Name = _name;
	}

	CoreNamedValue(const kstl::string& _name) : CoreValueBase<T, CoreNamedItem>(CoreItem::CORENAMEDVALUE)
	{
		CoreNamedItem::m_Name = _name;
	}

	/*template<typename valType>
	CoreNamedValue& operator= (const valType& _value)
	{
		this->m_Value = _value;
		return *this;
	}*/
	virtual CoreItem& operator=(const bool& other)
	{
		this->m_Value = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const kfloat& other)
	{
		this->m_Value = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const int& other)
	{
		this->m_Value = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const unsigned int& other)
	{
		this->m_Value = (T)other;
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
	this->m_Value = L_Buffer;
	return *this;
}

template<>
inline CoreItem& CoreNamedValue<usString>::operator= (const kfloat& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%f", (kfloat)_value);
	this->m_Value = usString(L_Buffer);
	return *this;
}
template<>
inline CoreItem& CoreNamedValue<kstl::string>::operator= (const int& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", _value);
	this->m_Value = L_Buffer;
	return *this;
}

template<>
inline CoreItem& CoreNamedValue<usString>::operator= (const int& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", _value);
	this->m_Value = usString(L_Buffer);
	return *this;
}

template<>
inline CoreItem& CoreNamedValue<kstl::string>::operator= (const unsigned int& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", _value);
	this->m_Value = L_Buffer;
	return *this;
}

template<>
inline CoreItem& CoreNamedValue<usString>::operator= (const unsigned int& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", _value);
	this->m_Value = usString(L_Buffer);
	return *this;
}

template<>
inline CoreItem& CoreNamedValue<kstl::string>::operator= (const bool& _value)
{
	if (_value)
		this->m_Value = "true";
	else
		this->m_Value = "false";
	return *this;
}

template<>
inline CoreItem& CoreNamedValue<usString>::operator= (const bool& _value)
{
	if (_value)
		this->m_Value = usString("true");
	else
		this->m_Value = usString("false");
	return *this;
}


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
	CoreValue& operator= (const valType& _value)
	{
		this->m_Value = _value;
		return *this;
	}*/
	virtual CoreItem& operator=(const bool& other)
	{
		this->m_Value = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const kfloat& other)
	{
		this->m_Value = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const int& other)
	{
		this->m_Value = (T)other;
		return *this;
	}
	virtual CoreItem& operator=(const unsigned int& other)
	{
		this->m_Value = (T)other;
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
	this->m_Value = L_Buffer;
	return *this;
}

template<>
inline CoreItem& CoreValue<kstl::string>::operator= (const kstl::string& _value)
{
	this->m_Value = _value;
	return *this;
}


template<>
inline CoreItem& CoreValue<usString>::operator= (const kfloat& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%f", (kfloat)_value);
	this->m_Value = usString(L_Buffer);
	return *this;
}

template<>
inline CoreItem& CoreValue<kstl::string>::operator= (const int& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", _value);
	this->m_Value = L_Buffer;
	return *this;
}

template<>
inline CoreItem& CoreValue<usString>::operator= (const int& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", _value);
	this->m_Value = usString(L_Buffer);
	return *this;
}

template<>
inline CoreItem& CoreValue<kstl::string>::operator= (const unsigned int& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", _value);
	this->m_Value = L_Buffer;
	return *this;
}

template<>
inline CoreItem& CoreValue<usString>::operator= (const unsigned int& _value)
{
	char L_Buffer[64] = { 0 };
	snprintf(L_Buffer, 64, "%i", _value);
	this->m_Value = usString(L_Buffer);
	return *this;
}

template<>
inline CoreItem& CoreValue<kstl::string>::operator= (const bool& _value)
{
	if(_value)
		this->m_Value = "true";
	else 
		this->m_Value = "false";
	return *this;
}

template<>
inline CoreItem& CoreValue<usString>::operator= (const bool& _value)
{
	if (_value)
		this->m_Value = usString("true");
	else
		this->m_Value = usString("false");
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
	this->m_Value.x = this->m_Value.y = this->m_Value.z = _value;
	return *this;
}
template<>
inline CoreItem& CoreValue<Point3D>::operator= (const int& _value)
{
	return *this;
}
template<>
inline CoreItem& CoreValue<Point3D>::operator= (const unsigned int& _value)
{
	return *this;
}

template<>
inline CoreItem& CoreValue<Point3D>::operator=(const Point2D& other)
{
	this->m_Value.x = other.x;
	this->m_Value.y = other.y;

	return *this;
}
template<>
inline CoreItem& CoreValue<Point3D>::operator=(const Point3D& other)
{
	this->m_Value = other;
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
	this->m_Value.x = this->m_Value.y = _value;
	return *this;
}
template<>
inline CoreItem& CoreValue<Point2D>::operator= (const int& _value)
{
	return *this;
}
template<>
inline CoreItem& CoreValue<Point2D>::operator= (const unsigned int& _value)
{
	return *this;
}

template<>
inline CoreItem& CoreValue<Point2D>::operator=(const Point2D& other)
{
	this->m_Value = other;
	return *this;
}
template<>
inline CoreItem& CoreValue<Point2D>::operator=(const Point3D& other)
{
	this->m_Value.x = other.x;
	this->m_Value.y = other.y;
	return *this;
}
#endif // _COREVALUE_H