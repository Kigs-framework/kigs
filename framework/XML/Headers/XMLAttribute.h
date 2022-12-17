#ifndef __XML_ATTRIBUTE_H__
#define __XML_ATTRIBUTE_H__

#include "kstlstring.h"
#include "kTypes.h"
#include "PreallocatedClassNew.h"
#include <assert.h> 

#include <assert.h>

class XMLAttributeBase
{
public:

	virtual ~XMLAttributeBase()
	{

	}

	std::string	getName()
	{
		std::string n;
		getName(n);
		return n;
	}

	std::string_view	getRefName()
	{
		std::string_view n;
		getName(n);
		return n;
	}
	std::string	getString()
	{
		std::string n;
		getString(n);
		return n;
	}

	std::string_view	getRefString()
	{
		std::string_view n;
		getString(n);
		return n;
	}
	virtual bool	compareName(const std::string& n) const = 0;
	virtual bool	compareName(const std::string_view& n) const = 0;
	virtual bool	compareValue(const std::string_view& n) const = 0;

	//! return value as an int
	virtual int getInt() const =0;

	//! return value as a float
	virtual float getFloat() const =0;

	virtual XMLAttributeBase* Copy() = 0;

protected:
	virtual void getName(std::string& n) const = 0;
	virtual void getName(std::string_view& n) const = 0;
	virtual void getString(std::string& v) const = 0;
	virtual void getString(std::string_view& v) const = 0;

};


// ****************************************
// * XMLAttributeTemplate class
// * --------------------------------------
/**
 * \file	XMLAttribute.h
 * \class	XMLAttributeTemplate
 * \ingroup ModuleXML
 * \brief	Manage XML attributes structure using string or string_view.
 */
 // ****************************************
template<typename StringType>
class XMLAttributeTemplate : public XMLAttributeBase
{
	DECLARE_PREALLOCATED_NEW(XMLAttributeTemplate,4096)

public:
	//! constructor for string attributes
    XMLAttributeTemplate( const StringType& name, const StringType& value ) : mName(name), mValue(value)
	{

	}
	//! constructor for string attributes
	XMLAttributeTemplate(const char* name, const char* value) : mName(name), mValue(value)
	{

	}
	//! constructor for string attributes
	XMLAttributeTemplate(const char* name,unsigned int namelen, const char* value, unsigned int valuelen) : mName(name, namelen), mValue(value, valuelen)
	{

	}

	//! constructor for int attributes
	inline XMLAttributeTemplate(const StringType& name, int value)
	{
		assert(0); // only available for std::string
	}
 	//! constructor for float attributes
    inline XMLAttributeTemplate( const StringType&name, float value )
	{
		assert(0); // only available for std::string
	}
	
	//! destructor ( no need for virtual as XMLAttributeTemplate will not have inheritance )
	~XMLAttributeTemplate()
	{

	}

	XMLAttributeBase* Copy() override
	{
		XMLAttributeTemplate<StringType>* newone = new XMLAttributeTemplate<StringType>(mName,mValue);
		return newone;
	}

	//! set attribute value with the given string parameter
	void setString( const StringType &value )
	{
		mValue = value;
	}
	//! set attribute value with the given int parameter
	inline void setInt(const int value)
	{
		assert(0); // set int only available for std::string
	}
	//! set attribute value with the given float parameter
	inline void setFloat( const float value )
	{
		assert(0); // set float only available for std::string
	}

	void	getName(std::string& n) const override
	{
		n = mName;
	}
	void	getName(std::string_view& n) const override
	{
		n = mName;
	}

	void	getString(std::string& v) const override
	{
		v = mValue;
	}
	void	getString(std::string_view& v) const override
	{
		v = mValue;
	}

	void setName(const StringType& n)
	{
		mName = n;
	}
	//! return value as an int
	inline int getInt( ) const override;

	//! return value as a float
	inline float getFloat( ) const override;

	bool	compareName(const std::string& n) const override
	{
		return  (n == mName);
	}
	bool	compareName(const std::string_view& n) const override
	{
		return (n == mName);
	}

	inline bool	compareValue(const std::string_view& n) const override
	{
		return (n == mValue);
	}

private:
	//! name
	StringType mName;
	//! value
	StringType mValue;
};

typedef XMLAttributeTemplate<std::string> XMLAttribute;
typedef XMLAttributeTemplate<std::string_view> XMLAttributeStringRef;

#ifndef JAVASCRIPT
template<typename T>
IMPLEMENT_PREALLOCATED_NEW(XMLAttributeTemplate<T>, 4096)
#endif

#if (__cplusplus >= 201703L || _MSVC_LANG  >= 201703L) && !defined(JAVASCRIPT) && !defined(__clang__)
#include <charconv>

template<>
inline void XMLAttributeTemplate<std::string>::setInt(const int value)
{
	static char szValue[64];
	auto sz=std::to_chars(szValue, szValue + 64, value);
	mValue = std::string(szValue,sz.ptr- szValue);
}

template<>
inline void XMLAttributeTemplate<std::string>::setFloat(const float value)
{
	static char szValue[64];
	auto sz = std::to_chars(szValue, szValue + 64, value);
	mValue = std::string(szValue, sz.ptr - szValue);
}

template<>
inline XMLAttributeTemplate<std::string>::XMLAttributeTemplate(const std::string& name, int value) : mName(name)
{
	setInt(value);
}

template<>
inline XMLAttributeTemplate<std::string>::XMLAttributeTemplate(const std::string& name, float value) : mName(name)
{
	setFloat(value);
}

template<typename StringType>
inline int XMLAttributeTemplate<StringType>::getInt() const
{
	int result = 0;
	std::from_chars(mValue.data(), mValue.data() + mValue.size(), result);
	return result;
}

template<typename StringType>
inline float XMLAttributeTemplate<StringType>::getFloat() const
{
	float result = 0.0f;
	std::from_chars(mValue.data(), mValue.data() + mValue.size(), result);
	return result;
}

#else

template<>
inline XMLAttributeTemplate<std::string>::XMLAttributeTemplate(const std::string& name, int value) : mName(name)
{
	static char szValue[64];
	snprintf(szValue, 64, "%d", value);
	mValue = szValue;
}

template<>
inline XMLAttributeTemplate<std::string>::XMLAttributeTemplate(const std::string& name, float value) : mName(name)
{
	static char szValue[64];
	snprintf(szValue, 64, "%f", CastToFloat(value));
	mValue = szValue;
}

template<>
inline void XMLAttributeTemplate<std::string>::setInt(const int value)
{
	static char szValue[64];
	snprintf(szValue, 64, "%d", value);
	mValue = szValue;
}

template<>
inline void XMLAttributeTemplate<std::string>::setFloat(const float value)
{
	static char szValue[64];
	snprintf(szValue, 64, "%f", CastToFloat(value));
	mValue = szValue;
}

template<>
inline int XMLAttributeTemplate<std::string>::getInt() const
{
	// when no test is required, use atoi insteed of sscanf 
	/*int temp=0;

	sscanf(mValue.c_str( ), "%d",&temp);*/

	return atoi(mValue.c_str());
}

template<>
inline int XMLAttributeTemplate<std::string_view>::getInt() const
{
	// when no test is required, use atoi insteed of sscanf 
	/*int temp=0;

	sscanf(mValue.c_str( ), "%d",&temp);*/
	std::string to_string(mValue);
	return atoi(to_string.c_str());
}

template<>
inline float XMLAttributeTemplate<std::string>::getFloat() const
{
	// when no test is required, use atof insteed of sscanf 
	/*float temp=0;

	sscanf(mValue.c_str( ),"%f",&temp);*/

	return atof(mValue.c_str());
}

template<>
inline float XMLAttributeTemplate<std::string_view>::getFloat() const
{
	// when no test is required, use atof insteed of sscanf 
	/*float temp=0;

	sscanf(mValue.c_str( ),"%f",&temp);*/
	std::string to_string(mValue);
	return atof(to_string.c_str());
}
#endif



#endif 
