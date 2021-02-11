#pragma once

#include "CoreModifiableAttribute.h"
#include "AttributeModifier.h"

// ****************************************
// * maStringHeritage class
// * --------------------------------------
/**
* \class	maStringHeritage
* \ingroup CoreModifiableAttibute
* \brief	CoreModifiableAttributeData of string with different level of notification
*/
// ****************************************

template<int notificationLevel>
class maStringHeritage : public CoreModifiableAttributeData<kstl::string>
{	
	DECLARE_ATTRIBUTE_HERITAGE(maStringHeritage, maStringHeritage, kstl::string, CoreModifiable::ATTRIBUTE_TYPE::STRING);
	
public:


	//! return a const char* pointer on internal value
	//! Doesn't call modifiers!
	const char* c_str() const { return mValue.c_str(); }

	void* getRawValue() final { return (void*)mValue.data(); }
	size_t MemorySize() const final { return mValue.size(); };

	// getValue overloads
	virtual bool getValue(kstl::string& value) const override
	{
		kstl::string tmpValue = this->mValue;
		CALL_GETMODIFIER(notificationLevel, tmpValue);
		value = tmpValue;
		return true;
	}
	virtual bool getValue(usString&  value) const override
	{
		kstl::string tmpValue = this->mValue;
		CALL_GETMODIFIER(notificationLevel, tmpValue);
		value = tmpValue;
		return true;
	}
	virtual bool getValue(void*& value) const override
	{
		value = (void*)&mValue;
		return true;
	}

	virtual bool getValue(float& value) const override
	{
		value = (float)atof(mValue.c_str());
		return true;
	}

	///

	// setValue overloads
	virtual bool setValue(const char* value) override
	{
		if (this->isReadOnly())
			return false;

		kstl::string tmpValue = value;
		CALL_SETMODIFIER(notificationLevel, tmpValue);
		this->mValue = tmpValue;
		DO_NOTIFICATION(notificationLevel);
		return true;
	}
	virtual bool setValue(const kstl::string& value) override
	{
		if (this->isReadOnly())
			return false;

		kstl::string tmpValue = value;
		CALL_SETMODIFIER(notificationLevel, tmpValue);
		this->mValue = tmpValue;
		DO_NOTIFICATION(notificationLevel);
		return true;
	}

#undef DECLARE_SET_NUMERIC
#define DECLARE_SET_NUMERIC(type)	virtual bool setValue(type value) override { \
	if (this->isReadOnly())\
		return false; \
	kstl::string tmpValue = std::to_string(value); \
	CALL_SETMODIFIER(notificationLevel, tmpValue); \
	this->mValue = tmpValue; \
	DO_NOTIFICATION(notificationLevel); \
	return true; \
}
	
	EXPAND_MACRO_FOR_NUMERIC_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SET_NUMERIC);

	///

	// operators
	auto& operator+=(const kstl::string& attribute)
	{
		kstl::string val;
		getValue(val);
		val += attribute;
		setValue(val);
		return *this;
	}
	auto& operator+=(const char* attribute)
	{
		*this += kstl::string{ attribute };
		return *this;
	}
	///



};


STATIC_ASSERT_NOTIF_LEVEL_SIZES(maStringHeritage);


using maString = maStringHeritage<0>;

