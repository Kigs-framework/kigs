#include "PrecompiledHeaders.h"
#include "CoreItem.h"
#include "CoreVector.h"
#include "CoreMap.h"
#include "maCoreItem.h"



CoreItemSP CoreItemIterator::operator*() const
{
	return myPointer->operator*();
}

CoreItemIterator& CoreItemIterator::operator=(const CoreItemIterator & other)
{
	SmartPointer<CoreItemIteratorBase>::operator=(other);
	return *this;
}

CoreItemIterator& CoreItemIterator::operator+(const int decal)
{
	myPointer->operator+(decal);
	return *this;
}

CoreItemIterator& CoreItemIterator::operator++()
{
	myPointer->operator++();
	return *this;
}

CoreItemIterator CoreItemIterator::operator++(int)
{
	CoreItemIterator	tmp(myPointer->clone());
	myPointer->operator++();
	return tmp;
}

bool CoreItemIterator::operator==(const CoreItemIterator & other) const
{
	return (*myPointer) == *(other.myPointer);
}

bool CoreItemIterator::operator!=(const CoreItemIterator & other) const
{
	return (*myPointer) != *(other.myPointer);
}

bool CoreItemIterator::getKey(kstl::string& returnedkey)
{
	return myPointer->getKey(returnedkey);
}

bool CoreItemIterator::getKey(usString& returnedkey)
{
	return myPointer->getKey(returnedkey);
}


CoreItemSP CoreItemIteratorBase::operator*() const
{
	if (myPos == 0)
	{
		return myAttachedCoreItem;
	}
	return CoreItemSP(nullptr);
}

CoreItemIteratorBase& CoreItemIteratorBase::operator=(const CoreItemIteratorBase & other)
{
	myAttachedCoreItem = other.myAttachedCoreItem;
	myPos = other.myPos;
	return *this;
}



// operator [] needs to be overloaded on vectors and maps
CoreItemSP CoreItem::operator[](int i) const
{
	if (i == 0)
	{
		return CoreItemSP((CoreItem*)this, StealRefTag{}); // hack
	}
	return CoreItemSP(nullptr);
}

CoreItemSP CoreItem::operator[](const kstl::string& key) const
{
	return CoreItemSP(nullptr);
}

CoreItemSP CoreItem::operator[](const usString& key) const
{
	return CoreItemSP(nullptr);
}

CoreItemSP CoreNamedItem::operator[](const kstl::string& key) const
{
	if (key == m_Name)
	{
		return CoreItemSP((CoreItem*)this, StealRefTag{}); // hack
	}
	return CoreItemSP(nullptr);
}

CoreItemSP CoreNamedItem::operator[](const usString& key) const
{
	if (key.ToString() == m_Name)
	{
		return   CoreItemSP((CoreItem*)this, StealRefTag{}); // hack
	}
	return CoreItemSP(nullptr);
}

CoreItem::operator bool() const
{
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return false;
}

CoreItem::operator kfloat() const
{
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return 0.0f;
}

CoreItem::operator int() const
{
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return 0;
}

CoreItem::operator unsigned int() const
{
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return 0;
}

CoreItem::operator kstl::string() const
{
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return "";
}

CoreItem::operator usString() const
{
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return usString("");
}

CoreItem::operator Point2D() const
{
	Point2D result;
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return result;
}

CoreItem::operator Quaternion() const
{
	Quaternion result;
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return result;
}


CoreItem::operator Point3D() const
{
	Point3D result;
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return result;
}

CoreItemSP	CoreItemSP::getCoreMap()
{
	return CoreItemSP(new CoreMap<kstl::string>(), StealRefTag{});
}
CoreItemSP	CoreItemSP::getCoreVector()
{
	return CoreItemSP(new CoreVector(), StealRefTag{});
}
CoreItemSP	CoreItemSP::getCoreValue(int i)
{
	return CoreItemSP(new CoreValue<int>(i), StealRefTag{});
}
CoreItemSP	CoreItemSP::getCoreValue(float f)
{
	return CoreItemSP(new CoreValue<float>(f), StealRefTag{});
}
CoreItemSP	CoreItemSP::getCoreValue(const kstl::string& s)
{
	return CoreItemSP(new CoreValue<kstl::string>(s), StealRefTag{});
}
CoreItemSP	CoreItemSP::getCoreValue(const usString& s)
{
	return CoreItemSP(new CoreValue<usString>(s), StealRefTag{});
}

CoreItemIterator CoreItemSP::begin()
{
	return myPointer->begin();
}

CoreItemIterator CoreItemSP::end()
{
	return myPointer->end();
}


CoreItemSP::CoreItemSP(const bool& value) 
{
	myPointer = new CoreValue<bool>(value);
}

CoreItemSP::CoreItemSP(const kfloat& value)
{
	myPointer = new CoreValue<kfloat>(value);
}
CoreItemSP::CoreItemSP(const int& value)
{
	myPointer = new CoreValue<int>(value);
}
CoreItemSP::CoreItemSP(const unsigned int& value)
{
	myPointer = new CoreValue<unsigned int>(value);
}
CoreItemSP::CoreItemSP(const kstl::string& value, CoreModifiable* owner)
{
	// check eval
	if (CoreModifiable::AttributeNeedEval(value))
	{
		maCoreItemValue	tmpVal;
		tmpVal.InitWithJSON(value,owner);
		*this = tmpVal.item;
	}
	else
	{
		myPointer = new CoreValue<kstl::string>(value);
	}
}
CoreItemSP::CoreItemSP(const usString& value, CoreModifiable* owner)
{
	// check eval
	if (CoreModifiable::AttributeNeedEval(value.ToString()))
	{
		maCoreItemValue	tmpVal;
		tmpVal.InitWithJSON(value.ToString(), owner);
		*this = tmpVal.item;
	}
	else
	{
		myPointer = new CoreValue<usString>(value);
	}
}
CoreItemSP::CoreItemSP(const Point2D& value)
{
	myPointer = new CoreVector();

	(*myPointer).set("", CoreItemSP(value.x));
	(*myPointer).set("", CoreItemSP(value.y));
}
CoreItemSP::CoreItemSP(const Point3D& value)
{
	myPointer = new CoreVector();
	(*myPointer).set("", CoreItemSP(value.x));
	(*myPointer).set("", CoreItemSP(value.y));
	(*myPointer).set("", CoreItemSP(value.z));
}

// empty assignement with value
CoreItem& CoreItem::operator=(const bool& other)
{
	KIGS_WARNING("trying to assign base CoreItem with value", 2);
	return *this;
}
CoreItem& CoreItem::operator=(const kfloat& other)
{
	KIGS_WARNING("trying to assign base CoreItem with value", 2);
	return *this;
}
CoreItem& CoreItem::operator=(const int& other)
{
	KIGS_WARNING("trying to assign base CoreItem with value", 2);
	return *this;
}
CoreItem& CoreItem::operator=(const unsigned int& other)
{
	KIGS_WARNING("trying to assign base CoreItem with value", 2);
	return *this;
}
CoreItem& CoreItem::operator=(const kstl::string& other)
{
	KIGS_WARNING("trying to assign base CoreItem with value", 2);
	return *this;
}
CoreItem& CoreItem::operator=(const usString& other)
{
	KIGS_WARNING("trying to assign base CoreItem with value", 2);
	return *this;
}
CoreItem& CoreItem::operator=(const Point2D& other)
{
	KIGS_WARNING("trying to assign base CoreItem with value", 2);
	return *this;
}
CoreItem& CoreItem::operator=(const Point3D& other)
{
	KIGS_WARNING("trying to assign base CoreItem with value", 2);
	return *this;
}