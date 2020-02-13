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
	const char* c_str() const { return _value.c_str(); }


	// getValue overloads
	virtual bool getValue(kstl::string& value) const override
	{
		kstl::string tmpValue = this->_value;
		CALL_GETMODIFIER(notificationLevel, tmpValue);
		value = tmpValue;
		return true;
	}
	virtual bool getValue(usString&  value) const override
	{
		kstl::string tmpValue = this->_value;
		CALL_GETMODIFIER(notificationLevel, tmpValue);
		value = tmpValue;
		return true;
	}
	virtual bool getValue(void*& value) const override
	{
		value = (void*)&_value;
		return true;
	}

	virtual bool getValue(float& value) const override
	{
		value = (float)atof(_value.c_str());
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
		this->_value = tmpValue;
		DO_NOTIFICATION(notificationLevel);
		return true;
	}
	virtual bool setValue(const kstl::string& value) override
	{
		if (this->isReadOnly())
			return false;

		kstl::string tmpValue = value;
		CALL_SETMODIFIER(notificationLevel, tmpValue);
		this->_value = tmpValue;
		DO_NOTIFICATION(notificationLevel);
		return true;
	}
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

