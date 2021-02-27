#ifndef _MAUSSTRING_H
#define _MAUSSTRING_H

#include "CoreModifiableAttribute.h"
#include "usString.h"
#include "AttributeModifier.h"

// ****************************************
// * maUSStringHeritage class
// * --------------------------------------
/**
* \class	maUSStringHeritage
* \ingroup CoreModifiableAttibute
* \brief	CoreModifiableAttributeData of usstring with different level of notification
*/
// ****************************************
template<int notificationLevel>
class maUSStringHeritage : public CoreModifiableAttributeData<usString>
{
	DECLARE_ATTRIBUTE_HERITAGE(maUSStringHeritage, maUSStringHeritage, usString, CoreModifiable::ATTRIBUTE_TYPE::USSTRING);

public:

	//! Extra constructors
	maUSStringHeritage(CoreModifiable& owner, bool isInitAttribute, KigsID ID, const kstl::string& value ) : CoreModifiableAttributeData<usString>(owner, isInitAttribute, ID)
	{
		mValue = usString{ value };
	}

	void* getRawValue() final { return (void*)mValue.us_str(); }
	size_t MemorySize() const final { return mValue.length()*sizeof(u16); };

	/// getValue overloads
	virtual bool getValue(kstl::string& value) const override
	{
		usString copy(mValue);
		value = copy.ToString();
		return true;
	}
	virtual bool getValue(usString& value) const override
	{
		value = mValue;
		return true;
	}
	virtual bool getValue(void*& value) const override
	{
		value = (void*)&mValue;
		return true;
	}
	///

	/// setValue overloads
	virtual bool setValue(const char* value) override
	{
		if (this->isReadOnly())
			return false;
		
		mValue = usString(value);
		
		DO_NOTIFICATION(notificationLevel);
		
		return true;
	}
	virtual bool setValue(const kstl::string& value) override
	{
		if (this->isReadOnly())
			return false;
		
		mValue = value;
		
		DO_NOTIFICATION(notificationLevel);
		
		return false;
	}
	virtual bool setValue(const unsigned short* value) override
	{
		if (this->isReadOnly())
			return false;
		
		mValue = value;
		DO_NOTIFICATION(notificationLevel);
		
		return true;
	}
	virtual bool setValue(const usString& value) override
	{
		if (this->isReadOnly())
			return false;
		
		mValue = value;
		DO_NOTIFICATION(notificationLevel);
		return true;
	}

	virtual bool setValue(const UTF8Char* value) override 
	{
		if (this->isReadOnly())
			return false;

		mValue = value;

		DO_NOTIFICATION(notificationLevel);

		return false;
	}

#undef DECLARE_SET_NUMERIC
#define DECLARE_SET_NUMERIC(type)	virtual bool setValue(type value) override { \
	if (this->isReadOnly())\
		return false; \
	usString tmpValue = std::to_string(value); \
	CALL_SETMODIFIER(notificationLevel, tmpValue); \
	this->mValue = tmpValue; \
	DO_NOTIFICATION(notificationLevel); \
	return true; \
}

	EXPAND_MACRO_FOR_NUMERIC_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SET_NUMERIC);


	///

	/// operators
	// Assign
	auto& operator=(const unsigned short* attribute)
	{
		mValue = attribute;
		return *this;
	}	
	auto& operator=(kstl::string& attribute)
	{
		mValue = attribute;
		return *this;
	}
	/*auto& operator=(const char* attribute)
	{
		mValue = attribute;
		return *this;
	}*/

	// Comparison
	bool operator==(const usString& L_value) const
	{
		return (mValue == L_value);
	}
	bool operator==(unsigned short* L_value) const
	{
		return (mValue == L_value);
	}
	bool operator!=(const usString& L_value) const
	{
		return (mValue != L_value);
	}
	bool operator!=(unsigned short* L_value) const
	{
		return (mValue != L_value);
	}

	// Append
	auto& operator+=(const usString& value)
	{
		mValue += value;
		return *this;
	}
	auto& operator+=(unsigned short* value)
	{
		mValue += value;
		return *this;
	}
	auto& operator+=(const CoreModifiableAttribute& value)
	{
		usString L_tmp = usString("");
		if(value.getValue(L_tmp))
			mValue += L_tmp;
		return *this;
	}
	///

	//! return a const unsigned short* pointer on internal value
	const unsigned short* us_str() const { return mValue.us_str(); }
	kstl::string ToString() { return mValue.ToString(); }
	void strcpywUtoC(char * _Dest, const unsigned short * src) { mValue.strcpywUtoC(_Dest, src); }
	kstl::vector<usString>	SplitByCharacter(unsigned short value) const { return mValue.SplitByCharacter(value); }
	unsigned int strlen() const { return mValue.strlen(); }

};


using maUSString = maUSStringHeritage<0>;



#endif //_MAUSSTRING_H