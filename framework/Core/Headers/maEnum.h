#pragma once

#include "CoreModifiableAttribute.h"
#include "AttributeModifier.h"
#include <array>

// ****************************************
// * maEnumBase class
// * --------------------------------------
/**
* \class	maEnumBase
* \ingroup CoreModifiableAttibute
* \brief	CoreModifiableAttributeData for an enum
*/
// ****************************************

template<unsigned int N>
struct maEnumValue
{
	int current_value = 0;
	std::array<kstl::string, N> value_list;
};


// ****************************************
// * maEnumHeritage class
// * --------------------------------------
/**
* \class	maEnumHeritage
* \ingroup CoreModifiableAttibute
* \brief	CoreModifiableAttributeData for an enum with different level of notification
*/
// ****************************************
template<int notificationLevel, unsigned int nbElements>
class maEnumHeritage : public CoreModifiableAttributeData<maEnumValue<nbElements>>
{
	template<int notiflevel>
	using TemplateForPlacementNew = maEnumHeritage<notiflevel, nbElements>;

	DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maEnumHeritage, TemplateForPlacementNew, maEnumValue<nbElements>, CoreModifiable::ENUM);

public:
	
	

	maEnumHeritage(CoreModifiable& owner, bool isInitAttribute, KigsID ID, kstl::string val0, kstl::string val1, kstl::string val2 = "", kstl::string val3 = "", kstl::string val4 = "", kstl::string val5 = "", kstl::string val6 = "", kstl::string val7 = "", kstl::string val8 = "", kstl::string val9 = "")
		: CoreModifiableAttributeData<maEnumValue<nbElements>>(owner, isInitAttribute, ID)
	{
		//@TODO variadic template constructor?
		// Copy and move idiom
		if (nbElements>0) _value.value_list[0] = std::move(val0);
		if (nbElements>1) _value.value_list[1] = std::move(val1);
		if (nbElements>2) _value.value_list[2] = std::move(val2);
		if (nbElements>3) _value.value_list[3] = std::move(val3);
		if (nbElements>4) _value.value_list[4] = std::move(val4);
		if (nbElements>5) _value.value_list[5] = std::move(val5);
		if (nbElements>6) _value.value_list[6] = std::move(val6);
		if (nbElements>7) _value.value_list[7] = std::move(val7);
		if (nbElements>8) _value.value_list[8] = std::move(val8);
		if (nbElements>9) _value.value_list[9] = std::move(val9);
		_value.current_value = 0;
	}

	kstl::vector<kstl::string> getEnumElements() const override { return {_value.value_list.data(), _value.value_list.data() + nbElements }; }

	virtual bool CopyAttribute(const CoreModifiableAttribute& attribute) override
	{
		if (CoreModifiableAttributeData<maEnumValue<nbElements>>::CopyAttribute(attribute)) return true;
		int val;
		if (attribute.getValue(val))
		{
			if (val >= 0 && val < nbElements)
			{
				_value.current_value = val;
				return true;
			}
		}
		return false;
	}


	kstl::string& operator[](unsigned int index) { KIGS_ASSERT(index < nbElements); return _value.value_list[index]; }
	const kstl::string& operator[](unsigned int index) const { KIGS_ASSERT(index < nbElements); return _value.value_list[index]; }



	operator int() { int val = 0; getValue(val); return val; }
	operator const kstl::string&() const
	{
		int val; getValue(val);
		return _value.value_list[val];
	}


#define IMPLEMENT_SET_VALUE_ENUM(type)\
	virtual bool setValue(type value) override { if (this->isReadOnly()) { return false; }  unsigned int tmpValue = (unsigned int)value; CALL_SETMODIFIER(notificationLevel, tmpValue); if (tmpValue < nbElements) { _value.current_value = tmpValue;  DO_NOTIFICATION(notificationLevel);  return true; } return false; }

	EXPAND_MACRO_FOR_NUMERIC_TYPES(NOQUALIFIER, NOQUALIFIER, IMPLEMENT_SET_VALUE_ENUM);

#define IMPLEMENT_GET_VALUE_ENUM(type)\
	virtual bool getValue(type value) const override {  unsigned int tmpValue=_value.current_value; CALL_GETMODIFIER(notificationLevel, tmpValue); if(tmpValue<nbElements){ value = (type)tmpValue; return true;} return false;  }

	EXPAND_MACRO_FOR_NUMERIC_TYPES(NOQUALIFIER, &, IMPLEMENT_GET_VALUE_ENUM);


#undef IMPLEMENT_SET_VALUE_ENUM
#undef IMPLEMENT_GET_VALUE_ENUM

	virtual bool setValue(const kstl::string& value) override
	{
		if (this->isReadOnly()) { return false; }
		unsigned int i;
		for (i = 0; i<nbElements; i++)
		{
			if (_value.value_list[i] == value)
			{
				CALL_SETMODIFIER(notificationLevel, i);
				if (i < nbElements)
				{
					_value.current_value = i;
					DO_NOTIFICATION(notificationLevel);
					return true;
				}
				else return false;
			}
		}
		return false;
	}
	
	virtual bool getValue(kstl::string& value) const override
	{
		unsigned int tmpValue = _value.current_value;
		CALL_GETMODIFIER(notificationLevel, tmpValue);
		if (tmpValue < nbElements)
		{
			value = _value.value_list[tmpValue];
		}
		return false;
	}

	virtual bool setValue(const char* value) override
	{
		kstl::string localstr(value);
		return setValue(localstr);
	}

};



template<unsigned int nbElements>
using maEnum = maEnumHeritage<0, nbElements>;

