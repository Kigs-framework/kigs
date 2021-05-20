#ifndef _COREITEM_H
#define _COREITEM_H

#include "GenericRefCountedBaseClass.h"
#include "SmartPointer.h"
#include "usString.h"
#include "TecLibs/Tec3D.h"


class CoreModifiable;
class CoreModifiableAttribute;
class CoreItemIteratorBase;
class CoreItemIterator;
class CoreItem;


// ****************************************
// * CoreItemSP class
// * --------------------------------------
/**
* \class	CoreItemSP
* \file     CoreItem.h
* \ingroup Core
* \brief	SmartPointer class to manage CoreItem
*/
// ****************************************


class CoreItemSP : public std::shared_ptr<CoreItem>
{
public:
	using std::shared_ptr<CoreItem>::shared_ptr;

	// Auto cast
	template<typename U>
	operator SmartPointer<U>()
	{
		return std::static_pointer_cast<U>(*this);
	}

	explicit operator CoreItem& ()
	{
		return *get();
	}
	
	CoreItemSP(bool);
	CoreItemSP(float);
	CoreItemSP(double);
	CoreItemSP(s32);
	CoreItemSP(u32);
	CoreItemSP(s64);
	CoreItemSP(u64);
	CoreItemSP(std::string);
	CoreItemSP(usString);
	CoreItemSP(v2f);
	CoreItemSP(v3f);
	CoreItemSP(v4f);


	CoreItemSP operator[](int i) const;
	CoreItemSP operator[](const char* key) const;
	CoreItemSP operator[](const std::string& key) const;
	CoreItemSP operator[](const usString& key) const;

	// NOTE(antoine) i've disabled these conversion operators, they can be error prone
	// - the shared_ptr must not be null
	// - shared_ptr already has a bool operator which is commonly used in patterns like this : if(myCoreItemSP) { doSomething... }
	/*operator bool() const;
	operator float() const;
	operator double() const;
	operator int() const;
	operator unsigned int() const;
	operator s64() const;
	operator u64() const;
	operator std::string() const;
	operator usString() const;
	operator v2f() const;
	operator v3f() const;
	operator v4f() const;*/

	CoreItemIterator begin() const;
	CoreItemIterator end() const;
};



class CoreItemIteratorBase : public std::enable_shared_from_this<CoreItemIteratorBase>
{
public:
	CoreItemIteratorBase() : mAttachedCoreItem(nullptr), mPos(0xFFFFFFFF)
	{
		
	}

	CoreItemIteratorBase(const CoreItemIteratorBase& other) : mAttachedCoreItem(other.mAttachedCoreItem), mPos(other.mPos)
	{

	}

	virtual CoreItemIteratorBase* clone()
	{
		CoreItemIteratorBase* result = new CoreItemIteratorBase(mAttachedCoreItem, mPos);
		return result;
	}

	virtual CoreItemSP operator*() const;

	virtual CoreItemIteratorBase& operator=(const CoreItemIteratorBase & other);

	virtual CoreItemIteratorBase& operator+(const int decal)
	{
		mPos = 0xFFFFFFFF;
		return *this;
	}

	virtual CoreItemIteratorBase& operator++()
	{
		mPos = 0xFFFFFFFF;
		return *this;
	}

	virtual CoreItemIteratorBase operator++(int)
	{
		CoreItemIteratorBase	tmp(*this);
		operator++();
		return tmp;
	}

	virtual bool operator==(const CoreItemIteratorBase & other) const
	{
		if ((mAttachedCoreItem == other.mAttachedCoreItem) &&
			(mPos == other.mPos))
		{
			return true;
		}
		return false;
	}

	virtual bool operator!=(const CoreItemIteratorBase & other) const
	{
		if ((mAttachedCoreItem != other.mAttachedCoreItem) ||
			(mPos != other.mPos))
		{
			return true;
		}
		return false;
	}
	
	virtual bool	getKey(std::string& returnedkey)
	{
		returnedkey = getName();
		return false;
	}

	virtual bool	getKey(usString& returnedkey)
	{
		returnedkey = getName();
		return false;
	}

	virtual std::string getName() const
	{
		return "";
	}

	virtual ~CoreItemIteratorBase()
	{

	}
protected:

	friend class CoreItem;
	friend class CoreItemIterator;

	CoreItemIteratorBase(const CoreItemSP& item, unsigned int pos) : mAttachedCoreItem(item), mPos(pos)
	{

	}

	CoreItemSP		mAttachedCoreItem;
	unsigned int	mPos;
};

class CoreItemIterator : public SmartPointer<CoreItemIteratorBase>
{
public:

	CoreItemIterator(CoreItemIteratorBase* point) : SmartPointer<CoreItemIteratorBase>(point)
	{

	}

	virtual CoreItemSP operator*() const;

	virtual CoreItemIterator& operator=(const CoreItemIterator& other);

	virtual CoreItemIterator& operator+(const int decal);

	virtual CoreItemIterator& operator++();

	virtual CoreItemIterator operator++(int);

	virtual bool operator==(const CoreItemIterator& other) const;

	virtual bool operator!=(const CoreItemIterator& other) const;

	virtual bool	getKey(std::string& returnedkey);

	virtual bool	getKey(usString& returnedkey);

protected:


};

// ****************************************
// * CoreItem class
// * --------------------------------------
/**
* \class	CoreItem
* \file     CoreItem.h
* \ingroup Core
* \brief	Super class for CoreItem to retreivre CoreItem Values
*/
// ****************************************
class CoreItem : public GenericRefCountedBaseClass
{
public:
	enum COREITEM_TYPE
	{
		UNKNOWN				= 0,
		CORENAMEDITEMMASK	= 1,
		COREVECTOR			= 2,
		CORENAMEDVECTOR		= 3,
		COREMAP				= 4,
		CORENAMEDMAP		= 5,
		COREVALUE			= 8,
		CORENAMEDVALUE		= 9
	};

	virtual void set(int key, const CoreItemSP& toinsert) {};
	virtual void set(const std::string& key, const CoreItemSP& toinsert) {};
	virtual void set(const usString& key, const CoreItemSP& toinsert) {};

	virtual void erase(int key) {};
	virtual void erase(const std::string& key) {};
	virtual void erase(const usString& key) {};

	CoreItemSP SharedFromThis() 
	{ 
#ifdef _DEBUG
		try
		{
			return std::static_pointer_cast<CoreItem>(shared_from_this());
		}
		catch (const std::bad_weak_ptr&)
		{
			// NOTE(antoine) : Possible reasons :
			// It's forbidden to call shared_from_this inside the constructor
			// If the object was new'ed manually and not yet assigned to a shared_ptr, consider using the Make* functions below instead, or std::make_shared/MakeRefCounted
			__debugbreak();
		}
		return nullptr;
#else
		return std::static_pointer_cast<CoreItem>(shared_from_this());
#endif	
	}



//	CoreItemSP SharedFromThis() const { return std::static_pointer_cast<CoreItem>(shared_from_this()); }

	typedef size_t size_type;

	virtual operator bool() const;

	virtual operator kfloat() const;

	virtual operator double() const;

	virtual operator int() const;

	virtual operator s64() const;

	virtual operator unsigned int() const;

	virtual operator u64() const;

#ifndef _MSC_VER
	// bug in VC++ can not have explicit virtual cast operator
	explicit
#endif
	virtual operator std::string() const;

	virtual operator usString() const;
	virtual operator v2f() const;
	virtual operator v3f() const;
	virtual operator v4f() const;

	bool getValue(bool& _value) const {
		_value = (bool)*this;
		return true;
	}

	bool getValue(kfloat& _value) const {
		_value = (kfloat)*this;
		return true;}

	bool getValue(int& _value) const {
		_value = (int)*this;
		return true; }

	bool getValue(s64& _value) const {
		_value = (s64)*this;
		return true;
	}

	bool getValue(unsigned int& _value) const {
		_value = (unsigned int)*this;
		return true; }

	bool getValue(u64& _value) const {
		_value = (u64)*this;
		return true;
	}

	bool getValue(std::string& _value) const {
		_value = (std::string)*this;
		return true; }

	bool getValue(usString& _value) const {
		_value = (usString)*this;
		return true; }

	bool getValue(v2f& _value) const {
		_value = (v2f)*this;
		return true; }

	bool getValue(v3f& _value) const {
		_value = this->operator v3f();
		return true; }

	bool getValue(v4f& _value) const {
		_value = this->operator v4f();
		return true;
	}

	virtual bool operator==(const CoreItem& other) const
	{
		return false;
	}

	// empty assignement with value
	virtual CoreItem& operator=(const bool& other);
	virtual CoreItem& operator=(const kfloat& other);
	virtual CoreItem& operator=(const int& other);
	virtual CoreItem& operator=(const s64& other);
	virtual CoreItem& operator=(const unsigned int& other);
	virtual CoreItem& operator=(const u64& other);
	virtual CoreItem& operator=(const std::string& other);
	virtual CoreItem& operator=(const usString& other);
	virtual CoreItem& operator=(const v2f& other);
	virtual CoreItem& operator=(const v3f& other);
	virtual CoreItem& operator=(const v4f& other);

	virtual std::string toString() const { return ""; }
	
	virtual bool empty() const
	{
		return true;
	}

	virtual size_type size() const
	{
		return 1;
	}
	
	virtual bool isString() const {return false;}

	virtual CoreModifiableAttribute* createAttribute(CoreModifiable*)
	{
		return 0;
	}

	COREITEM_TYPE	GetType() const {return mType;}

	// operator [] needs to be overloaded on vectors and maps
	virtual CoreItemSP operator[](int i);

	CoreItemSP operator[](const char* key)
	{
		return (*this)[std::string(key)];
	}

	virtual CoreItemSP operator[](const std::string& key);

	virtual CoreItemSP operator[](const usString& key);

	// operator [] needs to be overloaded on vectors and maps

	virtual CoreItemIterator begin()
	{
		return CoreItemIterator(new CoreItemIteratorBase(SharedFromThis(), 0));
	}

	virtual CoreItemIterator end()
	{
		return CoreItemIterator(new CoreItemIteratorBase(SharedFromThis(), 0xFFFFFFFF));
	}

	virtual void*	getContainerStruct() = 0;

protected:

	CoreItem(COREITEM_TYPE _type) : mType(_type)
	{
	
	}


	COREITEM_TYPE	mType;
};


// ****************************************
// * CoreNamedItem class
// * --------------------------------------
/**
* \class	CoreNamedItem
* \file     CoreItem.h
* \ingroup Core
* \brief	Base class for CoreItem with a name
*/
// ****************************************
class CoreNamedItem : public CoreItem
{
protected:
	CoreNamedItem(COREITEM_TYPE _type,const std::string& _name) : CoreItem(_type),mName(_name)
	{
		
	}

	CoreNamedItem(COREITEM_TYPE _type) : CoreItem(_type),mName("")
	{
		
	}
public:
	virtual std::string getName() const
	{
		return mName;
	}

	void	setName(const std::string& _name)
	{
		mName=_name;
	}


	virtual CoreItemSP operator[](const std::string& key);

	virtual CoreItemSP operator[](const usString& key);


protected:
	std::string	mName;
};


#define DECLARE_MAKE_COREVALUE(type) CoreItemSP MakeCoreValue(const type& value);\
CoreItemSP MakeCoreNamedValue(const type& value, const std::string& name);\
inline CoreItemSP::CoreItemSP(type value)\
{\
	*this = MakeCoreValue(value);\
}

CoreItemSP MakeCoreMap();
CoreItemSP MakeCoreNamedMap(const std::string& name);
CoreItemSP MakeCoreMapUS();
CoreItemSP MakeCoreNamedMapUS(const std::string& name);
CoreItemSP MakeCoreVector();
CoreItemSP MakeCoreNamedVector(const std::string& name);

DECLARE_MAKE_COREVALUE(bool);
DECLARE_MAKE_COREVALUE(s32);
DECLARE_MAKE_COREVALUE(u32);
DECLARE_MAKE_COREVALUE(s64);
DECLARE_MAKE_COREVALUE(u64);
DECLARE_MAKE_COREVALUE(float);
DECLARE_MAKE_COREVALUE(double);
DECLARE_MAKE_COREVALUE(std::string);
DECLARE_MAKE_COREVALUE(usString);
DECLARE_MAKE_COREVALUE(v2f);
DECLARE_MAKE_COREVALUE(v3f);
DECLARE_MAKE_COREVALUE(v4f);

CoreItemSP MakeCoreValue(const std::string& value, CoreModifiable* owner);
CoreItemSP MakeCoreValue(const usString& value, CoreModifiable* owner);

template<typename smartPointOn, typename ... Args>
CoreItemSP MakeCoreItemOfType(Args&& ... args)
{
	return std::make_shared<smartPointOn>(std::forward<decltype(args)>(args)...);
}



// operator [] needs to be overloaded on vectors and maps
inline CoreItemSP CoreItemSP::operator[](int i) const
{
	if(get())
		return get()->operator[](i);

	return nullptr;
}

inline CoreItemSP CoreItemSP::operator[](const char* key) const
{
	if (get())
		return get()->operator[](key);

	return nullptr;
}

inline CoreItemSP CoreItemSP::operator[](const std::string& key) const
{
	if (get())
		return get()->operator[](key);

	return nullptr;
}

inline CoreItemSP CoreItemSP::operator[](const usString& key) const
{
	if (get())
		return get()->operator[](key);

	return nullptr;
}

/*inline CoreItemSP::operator bool() const
{
	return get()->operator bool();
}

inline CoreItemSP::operator float() const
{
	return get()->operator kfloat();
}

inline CoreItemSP::operator double() const
{
	return get()->operator double();
}

inline CoreItemSP::operator int() const
{
	return get()->operator int();
}

inline CoreItemSP::operator unsigned int() const
{
	return get()->operator unsigned int();
}

inline CoreItemSP::operator s64() const
{
	return get()->operator s64();
}

inline CoreItemSP::operator u64() const
{
	return get()->operator u64();
}

inline CoreItemSP::operator std::string() const
{
	return get()->operator std::string();
}

inline CoreItemSP::operator usString() const
{
	return get()->operator usString();
}

inline CoreItemSP::operator v2f() const
{
	return get()->operator v2f();
}

inline CoreItemSP::operator v3f() const
{
	return get()->operator v3f();
} 

inline CoreItemSP::operator v4f() const
{
	return get()->operator v4f();
}*/

inline CoreItemIterator CoreItemSP::begin() const
{
	return get()->begin();
}

inline CoreItemIterator CoreItemSP::end() const
{
	return get()->end();
}


#endif // _COREITEM_H