#pragma once

#include "CoreModifiableAttribute.h"
#include "AttributeModifier.h"

// ****************************************
// * maBoolHeritage class
// * --------------------------------------
/**
* \class	maBoolHeritage
* \ingroup CoreModifiableAttibute
* \brief	CoreModifiableAttributeData for bool with different level of notification
*/
// ****************************************
template<int notificationLevel>
class maBoolHeritage : public CoreModifiableAttributeData<bool>
{
	DECLARE_ATTRIBUTE_HERITAGE(maBoolHeritage, maBoolHeritage, bool, CoreModifiable::ATTRIBUTE_TYPE::BOOL);

public:

	// getValue overloads
#define IMPLEMENT_GET_VALUE_BOOL(type)\
	virtual bool getValue(type value) const override \
	{\
		bool tmpValue = mValue;\
		CALL_GETMODIFIER(notificationLevel, tmpValue);\
		value = (type)tmpValue;\
		return true;\
	}
	EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, IMPLEMENT_GET_VALUE_BOOL);

	virtual bool getValue(kstl::string& value) const override
	{
		bool tmpValue = mValue;
		CALL_GETMODIFIER(notificationLevel, tmpValue);
		value = tmpValue ? "true" : "false";
		return true;
	}
	///

	// setValue overloads
#define IMPLEMENT_SET_VALUE_BOOL(type)\
	virtual bool setValue(type value) override\
	{\
		if (isReadOnly()) { return false; }\
		bool tmpValue = (value != (type)0);\
		CALL_SETMODIFIER(notificationLevel, tmpValue); \
		mValue = tmpValue; \
		DO_NOTIFICATION(notificationLevel);\
		return true;\
	}

	EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, IMPLEMENT_SET_VALUE_BOOL);


	virtual bool setValue(const kstl::string& value) override
	{
		if (this->isReadOnly()) { return false; }
		bool tmpValue = (value == "true" || value == "TRUE");
		CALL_SETMODIFIER(notificationLevel, tmpValue);
		mValue = tmpValue;
		DO_NOTIFICATION(notificationLevel);  
		return true;
	}
	virtual bool setValue(const char* value) override { if (this->isReadOnly()) { return false; } if (value) { kstl::string localstr(value); return setValue(localstr); } return  setValue(false);}
	///

};

using maBool = maBoolHeritage<0>;





#undef IMPLEMENT_SET_VALUE_BOOL
#undef IMPLEMENT_GET_VALUE_BOOL

