#include "PrecompiledHeaders.h"
#include "CoreItem.h"
#include "CoreVector.h"
#include "CoreMap.h"
#include "maCoreItem.h"



CoreItemSP CoreItemIteratorBase::operator*() const
{
	if (mPos == 0)
	{
		return mAttachedCoreItem;
	}
	return CoreItemSP(nullptr);
}

CoreItemIteratorBase& CoreItemIteratorBase::operator=(const CoreItemIteratorBase& other)
{
	mAttachedCoreItem = other.mAttachedCoreItem;
	mPos = other.mPos;
	return *this;
}

CoreItemSP CoreItemIterator::operator*() const
{
	return get()->operator*();
}

CoreItemIterator& CoreItemIterator::operator=(const CoreItemIterator& other)
{
	SmartPointer<CoreItemIteratorBase>::operator=(other);
	return *this;
}

CoreItemIterator& CoreItemIterator::operator+(const int decal)
{
	get()->operator+(decal);
	return *this;
}

CoreItemIterator& CoreItemIterator::operator++()
{
	get()->operator++();
	return *this;
}

CoreItemIterator CoreItemIterator::operator++(int)
{
	CoreItemIterator	tmp(get()->clone());
	get()->operator++();
	return tmp;
}

bool CoreItemIterator::operator==(const CoreItemIterator& other) const
{
	return (*get()) == *(other.get());
}

bool CoreItemIterator::operator!=(const CoreItemIterator& other) const
{
	return (*get()) != *(other.get());
}

bool CoreItemIterator::getKey(kstl::string& returnedkey)
{
	return get()->getKey(returnedkey);
}

bool CoreItemIterator::getKey(usString& returnedkey)
{
	return get()->getKey(returnedkey);
}


// operator [] needs to be overloaded on vectors and maps
CoreItemSP CoreItem::operator[](int i)
{
	if (i == 0)
	{
		return SharedFromThis();
	}
	return CoreItemSP(nullptr);
}

CoreItemSP CoreItem::operator[](const kstl::string& key)
{
	return CoreItemSP(nullptr);
}

CoreItemSP CoreItem::operator[](const usString& key)
{
	return CoreItemSP(nullptr);
}

CoreItemSP CoreNamedItem::operator[](const kstl::string& key)
{
	if (key == mName)
	{
		return SharedFromThis();
	}
	return CoreItemSP(nullptr);
}

CoreItemSP CoreNamedItem::operator[](const usString& key)
{
	if (key.ToString() == mName)
	{
		return SharedFromThis();
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

CoreItem::operator double() const
{
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return 0.0;
}

CoreItem::operator int() const
{
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return 0;
}

CoreItem::operator s64() const
{
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return 0;
}

CoreItem::operator unsigned int() const
{
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return 0;
}

CoreItem::operator u64() const
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

CoreItem::operator Vector4D() const
{
	Vector4D result;
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return result;
}

CoreItem::operator Point3D() const
{
	Point3D result;
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return result;
}

CoreItemSP MakeCoreNamedMap(const std::string& name)
{
	return std::make_shared<CoreNamedMap<std::string>>(name);
}
CoreItemSP MakeCoreMap()
{
	return std::make_shared<CoreMap<std::string>>();
}
CoreItemSP MakeCoreMapUS()
{
	return std::make_shared<CoreMap<usString>>();
}
CoreItemSP MakeCoreNamedMapUS(const std::string& name)
{
	return std::make_shared<CoreNamedMap<usString>>(name);
}
CoreItemSP MakeCoreVector()
{
	return std::make_shared<CoreVector>();
}
CoreItemSP MakeCoreNamedVector(const std::string& name)
{
	return std::make_shared<CoreNamedVector>(name);
}
#define IMPLEMENT_MAKE_COREVALUE(type) CoreItemSP MakeCoreValue(const type& value)\
{\
	return std::make_shared<CoreValue<type>>(value);\
}\
CoreItemSP MakeCoreNamedValue(const type& value, const std::string& name)\
{\
	return std::make_shared<CoreNamedValue<type>>(value, name);\
}

IMPLEMENT_MAKE_COREVALUE(bool)
IMPLEMENT_MAKE_COREVALUE(int)
IMPLEMENT_MAKE_COREVALUE(unsigned int)
IMPLEMENT_MAKE_COREVALUE(s64)
IMPLEMENT_MAKE_COREVALUE(u64)
IMPLEMENT_MAKE_COREVALUE(float)
IMPLEMENT_MAKE_COREVALUE(double)
IMPLEMENT_MAKE_COREVALUE(std::string)
IMPLEMENT_MAKE_COREVALUE(usString)

CoreItemSP MakeCoreValue(const std::string& value, CoreModifiable* owner)
{
	// check eval
	if (AttributeNeedEval(value))
	{
		maCoreItemValue	tmpVal;
		tmpVal.InitWithJSON(value, owner);
		return tmpVal.item;
	}
	else
		return std::make_shared<CoreValue<std::string>>(value);
}
CoreItemSP MakeCoreValue(const usString& value, CoreModifiable* owner)
{
	if (AttributeNeedEval(value.ToString()))
	{
		maCoreItemValue	tmpVal;
		tmpVal.InitWithJSON(value.ToString(), owner);
		return tmpVal.item;
	}
	else
		return std::make_shared<CoreValue<usString>>(value);
}

CoreItemSP MakeCoreValue(const v2f& value)
{
	auto vector = MakeCoreVector();
	vector->set("", MakeCoreValue(value.x));
	vector->set("", MakeCoreValue(value.y));
	return vector;
}

CoreItemSP MakeCoreValue(const v3f& value)
{
	auto vector = MakeCoreVector();
	vector->set("", MakeCoreValue(value.x));
	vector->set("", MakeCoreValue(value.y));
	vector->set("", MakeCoreValue(value.z));
	return vector;
}

CoreItemSP MakeCoreValue(const v4f& value)
{
	auto vector = MakeCoreVector();
	vector->set("", MakeCoreValue(value.x));
	vector->set("", MakeCoreValue(value.y));
	vector->set("", MakeCoreValue(value.z));
	vector->set("", MakeCoreValue(value.w));
	return vector;
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
CoreItem& CoreItem::operator=(const s64& other)
{
	KIGS_WARNING("trying to assign base CoreItem with value", 2);
	return *this;
}
CoreItem& CoreItem::operator=(const u64& other)
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
CoreItem& CoreItem::operator=(const Vector4D& other)
{
	KIGS_WARNING("trying to assign base CoreItem with value", 2);
	return *this;
}
