#include "PrecompiledHeaders.h"
#include "CoreItem.h"
#include "CoreVector.h"
#include "CoreMap.h"



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