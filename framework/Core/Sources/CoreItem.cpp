#include "PrecompiledHeaders.h"
#include "CoreItem.h"
#include "CoreVector.h"
#include "CoreMap.h"
#include "maCoreItem.h"

#include <type_traits>

using namespace Kigs::Core;

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

bool CoreItemIterator::getKey(std::string& returnedkey)
{
	return get()->getKey(returnedkey);
}

bool CoreItemIterator::getKey(usString& returnedkey)
{
	return get()->getKey(returnedkey);
}


CoreItemSP::CoreItemSP(const bool& value) { *this = MakeCoreValue(value); }
CoreItemSP::CoreItemSP(const float& value) { *this = MakeCoreValue(value); }
CoreItemSP::CoreItemSP(const double& value) { *this = MakeCoreValue(value); }
CoreItemSP::CoreItemSP(const s32& value) { *this = MakeCoreValue(value); }
CoreItemSP::CoreItemSP(const u32& value) { *this = MakeCoreValue(value); }
CoreItemSP::CoreItemSP(const s64& value) { *this = MakeCoreValue(value); }
CoreItemSP::CoreItemSP(const u64& value) { *this = MakeCoreValue(value); }
CoreItemSP::CoreItemSP(const v2f& value) { *this = MakeCoreValue(value); }
CoreItemSP::CoreItemSP(const v3f& value) { *this = MakeCoreValue(value); }
CoreItemSP::CoreItemSP(const v4f& value) { *this = MakeCoreValue(value); }
CoreItemSP::CoreItemSP(const std::string& value, CoreModifiable* owner) { *this = MakeCoreValue(value, owner); }
CoreItemSP::CoreItemSP(const char* value, CoreModifiable* owner) { *this = MakeCoreValue(value, owner); }
CoreItemSP::CoreItemSP(const usString& value, CoreModifiable* owner) { *this = MakeCoreValue(value, owner); }

// operator [] needs to be overloaded on vectors and maps
CoreItemSP CoreItem::operator[](int i)
{
	if (i == 0)
	{
		return SharedFromThis();
	}
	return CoreItemSP(nullptr);
}

CoreItemSP CoreItem::operator[](const std::string& key)
{
	return CoreItemSP(nullptr);
}

CoreItemSP CoreItem::operator[](const usString& key)
{
	return CoreItemSP(nullptr);
}


CoreItem::operator bool() const
{
	KIGS_ERROR("cast operator called on base CoreItem", 2);
	return false;
}

CoreItem::operator float() const
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

CoreItem::operator std::string() const
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

CoreItemSP Kigs::Core::MakeCoreMap()
{
	return std::make_shared<CoreMap<std::string>>();
}
CoreItemSP Kigs::Core::MakeCoreMapUS()
{
	return std::make_shared<CoreMap<usString>>();
}

CoreItemSP Kigs::Core::MakeCoreVector()
{
	return std::make_shared<CoreVector>();
}

#define IMPLEMENT_MAKE_COREVALUE(type) CoreItemSP Kigs::Core::MakeCoreValue(type value)\
{\
	return std::make_shared<CoreValue<std::decay_t<type>>>(value);\
}

IMPLEMENT_MAKE_COREVALUE(const bool&)
IMPLEMENT_MAKE_COREVALUE(const s32&)
IMPLEMENT_MAKE_COREVALUE(const u32&)
IMPLEMENT_MAKE_COREVALUE(const s64&)
IMPLEMENT_MAKE_COREVALUE(const u64&)
IMPLEMENT_MAKE_COREVALUE(const float&)
IMPLEMENT_MAKE_COREVALUE(const double&)

CoreItemSP Kigs::Core::MakeCoreValue(const char* value, CoreModifiable* owner)
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

CoreItemSP Kigs::Core::MakeCoreValue(const std::string& value, CoreModifiable* owner)
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

CoreItemSP Kigs::Core::MakeCoreValue(const usString& value, CoreModifiable* owner)
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

CoreItemSP Kigs::Core::MakeCoreValue(const v2f& value)
{
	auto vector = MakeCoreVector();
	vector->set("", MakeCoreValue(value.x));
	vector->set("", MakeCoreValue(value.y));
	return vector;
}

CoreItemSP Kigs::Core::MakeCoreValue(const v3f& value)
{
	auto vector = MakeCoreVector();
	vector->set("", MakeCoreValue(value.x));
	vector->set("", MakeCoreValue(value.y));
	vector->set("", MakeCoreValue(value.z));
	return vector;
}

CoreItemSP Kigs::Core::MakeCoreValue(const v4f& value)
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
CoreItem& CoreItem::operator=(const float& other)
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
CoreItem& CoreItem::operator=(const std::string& other)
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
