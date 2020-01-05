#ifndef _COREITEM_H
#define _COREITEM_H

#include "RefCountedBaseClass.h"
#include "SmartPointer.h"
#include "usString.h"
#include "TecLibs/Tec3D.h"

class CoreItem;

class CoreItemSP : public SmartPointer<CoreItem>
{
public:
	CoreItemSP() : SmartPointer<CoreItem>(nullptr){}
	CoreItemSP(CoreItem* it) : SmartPointer<CoreItem>(it, StealRefTag{}) {}
	CoreItemSP(CoreItem* it, StealRefTag stealref) : SmartPointer<CoreItem>(it, stealref) {}
	CoreItemSP(CoreItem* it, GetRefTag getref) : SmartPointer<CoreItem>(it, getref) {}
	CoreItemSP(std::nullptr_t) : SmartPointer<CoreItem>(nullptr) {}

	operator CoreItem&()
	{
		return *get();
	}

	// operator [] needs to be overloaded on vectors and maps
	inline CoreItemSP operator[](int i) const;

	inline CoreItemSP operator[](const char* key) const;

	inline CoreItemSP operator[](const kstl::string& key) const;

	inline CoreItemSP operator[](const usString& key) const;


};


class CoreItemIteratorBase : public GenericRefCountedBaseClass
{
public:

	CoreItemIteratorBase() : GenericRefCountedBaseClass()
		,myAttachedCoreItem(nullptr)
		, myPos(0xFFFFFFFF)
	{
		
	}

	CoreItemIteratorBase(const CoreItemIteratorBase& other) : GenericRefCountedBaseClass()
		,myAttachedCoreItem(other.myAttachedCoreItem)
		, myPos(other.myPos)
	{

	}

	virtual CoreItemSP operator*() const;

	virtual CoreItemIteratorBase& operator=(const CoreItemIteratorBase & other);

	virtual CoreItemIteratorBase& operator+(const int decal)
	{
		myPos = 0xFFFFFFFF;
		return *this;
	}

	virtual CoreItemIteratorBase& operator++()
	{
		myPos = 0xFFFFFFFF;
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
		if ((myAttachedCoreItem == other.myAttachedCoreItem) &&
			(myPos == other.myPos))
		{
			return true;
		}
		return false;
	}

	virtual bool operator!=(const CoreItemIteratorBase & other) const
	{
		if ((myAttachedCoreItem != other.myAttachedCoreItem) ||
			(myPos != other.myPos))
		{
			return true;
		}
		return false;
	}
	
	virtual bool	getKey(kstl::string& returnedkey)
	{
		returnedkey = getName();
		return false;
	}

	virtual bool	getKey(usString& returnedkey)
	{
		returnedkey = getName();
		return false;
	}

	virtual kstl::string getName() const
	{
		return "";
	}

	virtual CoreItemIteratorBase*	clone()
	{
		CoreItemIteratorBase*	result = new CoreItemIteratorBase(myAttachedCoreItem, myPos);
		return result;
	}
	virtual ~CoreItemIteratorBase()
	{

	}
protected:

	friend class CoreItem;

	CoreItemIteratorBase(CoreItemSP item, unsigned int pos) : GenericRefCountedBaseClass()
		,myAttachedCoreItem(item)
		, myPos(pos)
	{

	}


	CoreItemSP		myAttachedCoreItem;
	unsigned int	myPos;
};

// same iterator for all CoreItem
class CoreItemIterator : public SmartPointer<CoreItemIteratorBase>
{
public:

	CoreItemIterator(CoreItemIteratorBase* point) : SmartPointer<CoreItemIteratorBase>(point, StealRefTag{})
	{

	}

	virtual CoreItemSP operator*() const;

	virtual CoreItemIterator& operator=(const CoreItemIterator & other);

	virtual CoreItemIterator& operator+(const int decal);

	virtual CoreItemIterator& operator++();

	virtual CoreItemIterator operator++(int);

	virtual bool operator==(const CoreItemIterator & other) const;

	virtual bool operator!=(const CoreItemIterator & other) const;

	virtual bool	getKey(kstl::string& returnedkey);

	virtual bool	getKey(usString& returnedkey);

protected:


};


// ****************************************
// * CoreItem class
// * --------------------------------------
/**
* \class	CoreItem
* \ingroup KigsCore
* \brief	Super class for CoreItem to retreivre CoreItem Values
* \author	ukn
* \version ukn
* \date	ukn
*/
// ****************************************
class CoreItem : public RefCountedBaseClass
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



	typedef size_t size_type;

	virtual operator bool() const;

	virtual operator kfloat() const;

	virtual operator int() const;

	virtual operator unsigned int() const;

#ifndef _MSC_VER
	// bug in VC++ can not have explicit virtual cast operator
	explicit
#endif
	virtual operator kstl::string() const;

	virtual operator usString() const;

	virtual operator Point2D() const;

	virtual operator Point3D() const;


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

	bool getValue(unsigned int& _value) const {
		_value = (unsigned int)*this;
		return true; }

	bool getValue(kstl::string& _value) const {
		_value = (kstl::string)*this;
		return true; }

	bool getValue(usString& _value) const {
		_value = (usString)*this;
		return true; }

	bool getValue(Point2D& _value) const {
		_value = (Point2D)*this;
		return true; }

	bool getValue(Point3D& _value) const {
		_value = (Point3D)*this;
		return true; }

	virtual bool operator==(const CoreItem& other) const
	{
		return false;
	}

	// empty assignement with value
	virtual CoreItem& operator=(const bool& other)
	{
		return *this;
	}
	virtual CoreItem& operator=(const kfloat& other)
	{
		return *this;
	}
	virtual CoreItem& operator=(const int& other)
	{
		return *this;
	}
	virtual CoreItem& operator=(const unsigned int& other)
	{
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



	virtual kstl::string toString() const { return ""; }
	
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

	COREITEM_TYPE	GetType() const {return m_eType;}

	bool isSubType(const KigsID& cid) const override {if(KigsID("CoreItem") == cid)return true;  return RefCountedBaseClass::isSubType(cid);}

	// operator [] needs to be overloaded on vectors and maps
	virtual CoreItemSP operator[](int i) const;

	CoreItemSP operator[](const char* key) const
	{
		return (*this)[kstl::string(key)];
	}

	virtual CoreItemSP operator[](const kstl::string& key) const;

	virtual CoreItemSP operator[](const usString& key) const;

	// operator [] needs to be overloaded on vectors and maps

	virtual CoreItemIterator begin()
	{
		CoreItemIteratorBase* iter = new CoreItemIteratorBase(CoreItemSP(this, StealRefTag{}), 0);
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}

	virtual CoreItemIterator end()
	{
		CoreItemIteratorBase* iter = new CoreItemIteratorBase(CoreItemSP(this, StealRefTag{}), 0xFFFFFFFF);
		CoreItemIterator	toReturn(iter);
		return toReturn;
	}

	virtual void*	getContainerStruct() = 0;

protected:

	CoreItem(COREITEM_TYPE _type) : m_eType(_type)
	{
	
	}

	void ProtectedDestroy() override {}

	COREITEM_TYPE	m_eType;
};


class CoreNamedItem : public CoreItem
{
protected:
	CoreNamedItem(COREITEM_TYPE _type,const kstl::string& _name) : CoreItem(_type),m_Name(_name)
	{
		
	}

	CoreNamedItem(COREITEM_TYPE _type) : CoreItem(_type),m_Name("")
	{
		
	}
public:
	virtual kstl::string getName() const
	{
		return m_Name;
	}

	void	setName(const kstl::string& _name)
	{
		m_Name=_name;
	}


	virtual CoreItemSP operator[](const kstl::string& key) const;

	virtual CoreItemSP operator[](const usString& key) const;


protected:
	kstl::string	m_Name;
};


// operator [] needs to be overloaded on vectors and maps
inline CoreItemSP CoreItemSP::operator[](int i) const
{
	return myPointer->operator[](i);
}

inline CoreItemSP CoreItemSP::operator[](const char* key) const
{
	return myPointer->operator[](key);
}

inline CoreItemSP CoreItemSP::operator[](const kstl::string& key) const
{
	return myPointer->operator[](key);
}

inline CoreItemSP CoreItemSP::operator[](const usString& key) const
{
	return myPointer->operator[](key);
}

#endif // _COREITEM_H