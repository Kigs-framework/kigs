#include "PrecompiledHeaders.h"
#include "CoreItem.h"


CoreItem& CoreItemIterator::operator*() const
{
	return *(*(*myPointer));
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





CoreItem* CoreItemIteratorBase::operator*() const
{
	if (myPos == 0)
	{
		return myAttachedCoreItem;
	}
	return KigsCore::Instance()->NotFoundCoreItem();
}

CoreItemIteratorBase& CoreItemIteratorBase::operator=(const CoreItemIteratorBase & other)
{
	myAttachedCoreItem = other.myAttachedCoreItem;
	myPos = other.myPos;
	return *this;
}



// operator [] needs to be overloaded on vectors and maps
CoreItem& CoreItem::operator[](int i) const
{
	if (i == 0)
	{
		return *(CoreItem*)this; // hack
	}
	return *KigsCore::Instance()->NotFoundCoreItem();
}

CoreItem& CoreItem::operator[](const kstl::string& key) const
{
	return *KigsCore::Instance()->NotFoundCoreItem();
}

CoreItem& CoreItem::operator[](const usString& key) const
{
	return *KigsCore::Instance()->NotFoundCoreItem();
}






CoreItem& CoreNamedItem::operator[](const kstl::string& key) const
{
	if (key == m_Name)
	{
		return *(CoreItem*)this; // hack
	}
	return *KigsCore::Instance()->NotFoundCoreItem();
}

CoreItem& CoreNamedItem::operator[](const usString& key) const
{
	if (key.ToString() == m_Name)
	{
		return  *(CoreItem*)this; // hack
	}
	return *KigsCore::Instance()->NotFoundCoreItem();
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
	return "";
}

CoreItem::operator Point2D() const
{
	Point2D result;
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return result;
}

CoreItem::operator Point3D() const
{
	Point3D result;
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return result;
}