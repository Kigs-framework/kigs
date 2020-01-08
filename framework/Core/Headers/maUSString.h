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
	DECLARE_ATTRIBUTE_HERITAGE(maUSStringHeritage, maUSStringHeritage, usString, CoreModifiable::USSTRING);

public:

	//! Extra constructors
	maUSStringHeritage(CoreModifiable& owner, bool isInitAttribute, KigsID ID, const kstl::string& value ) : CoreModifiableAttributeData<usString>(owner, isInitAttribute, ID)
	{
		_value = usString{ value };
	}



	/// getValue overloads
	virtual bool getValue(kstl::string& value) const override
	{
		usString copy(_value);
		value = copy.ToString();
		return true;
	}
	virtual bool getValue(usString& value) const override
	{
		value = _value;
		return true;
	}
	virtual bool getValue(void*& value) const override
	{
		value = (void*)&_value;
		return true;
	}
	///

	/// setValue overloads
	virtual bool setValue(const char* value) override
	{
		if (this->isReadOnly())
			return false;
		
		_value = usString(value);
		
		DO_NOTIFICATION(notificationLevel);
		
		return true;
	}
	virtual bool setValue(const kstl::string& value) override
	{
		if (this->isReadOnly())
			return false;
		
		_value = value;
		
		DO_NOTIFICATION(notificationLevel);
		
		return false;
	}
	virtual bool setValue(const unsigned short* value) override
	{
		if (this->isReadOnly())
			return false;
		
		_value = value;
		DO_NOTIFICATION(notificationLevel);
		
		return true;
	}
	virtual bool setValue(const usString& value) override
	{
		if (this->isReadOnly())
			return false;
		
		_value = value;
		DO_NOTIFICATION(notificationLevel);
		return true;
	}

	virtual bool setValue(const UTF8Char* value) override 
	{
		if (this->isReadOnly())
			return false;

		_value = value;

		DO_NOTIFICATION(notificationLevel);

		return false;
	}

	///

	/// operators
	// Assign
	auto& operator=(const unsigned short* attribute)
	{
		_value = attribute;
		return *this;
	}	
	auto& operator=(kstl::string& attribute)
	{
		_value = attribute;
		return *this;
	}
	auto& operator=(const char* attribute)
	{
		_value = attribute;
		return *this;
	}

	// Comparison
	bool operator==(const usString& L_value) const
	{
		return (_value == L_value);
	}
	bool operator==(unsigned short* L_value) const
	{
		return (_value == L_value);
	}
	bool operator!=(const usString& L_value) const
	{
		return (_value != L_value);
	}
	bool operator!=(unsigned short* L_value) const
	{
		return (_value != L_value);
	}

	// Append
	auto& operator+=(const usString& value)
	{
		_value += value;
		return *this;
	}
	auto& operator+=(unsigned short* value)
	{
		_value += value;
		return *this;
	}
	auto& operator+=(const CoreModifiableAttribute& value)
	{
		usString L_tmp = usString("");
		if(value.getValue(L_tmp))
			_value += L_tmp;
		return *this;
	}
	///

	//! return a const unsigned short* pointer on internal value
	const unsigned short* us_str() const { return _value.us_str(); }
	kstl::string ToString() { return _value.ToString(); }
	void strcpywUtoC(char * _Dest, const unsigned short * src) { _value.strcpywUtoC(_Dest, src); }
	kstl::vector<usString>	SplitByCharacter(unsigned short value) const { return _value.SplitByCharacter(value); }
	unsigned int strlen() const { return _value.strlen(); }

};


using maUSString = maUSStringHeritage<0>;



#endif //_MAUSSTRING_H