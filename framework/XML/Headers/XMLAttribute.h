#ifndef __XML_ATTRIBUTE_H__
#define __XML_ATTRIBUTE_H__

#include "kstlstring.h"
#include "kTypes.h"
#include "PreallocatedClassNew.h"
#include <assert.h> 

#include <assert.h>

class XMLAttributeBase
{};


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
 	//! constructor for kfloat attributes
    inline XMLAttributeTemplate( const StringType&name, kfloat value )
	{
		assert(0); // only available for std::string
	}
	
	//! destructor ( no need for virtual as XMLAttributeTemplate will not have inheritance )
	~XMLAttributeTemplate()
	{

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
	//! set attribute value with the given kfloat parameter
	inline void setFloat( const kfloat value )
	{
		assert(0); // set float only available for std::string
	}

	//! return attribute name
	const StringType& getName( )
	{
		return mName;
	}
	void setName(const StringType& n)
	{
		mName = n;
	}
	//! return value as a string
	const StringType& getString( )
	{
		return mValue;
	}

	//! return value as an int
	inline int getInt( );

	//! return value as a kfloat
	inline kfloat getFloat( );

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
inline void XMLAttributeTemplate<std::string>::setFloat(const kfloat value)
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
inline XMLAttributeTemplate<std::string>::XMLAttributeTemplate(const std::string& name, kfloat value) : mName(name)
{
	setFloat(value);
}

template<typename StringType>
inline int XMLAttributeTemplate<StringType>::getInt()
{
	int result = 0;
	std::from_chars(mValue.data(), mValue.data() + mValue.size(), result);
	return result;
}

template<typename StringType>
inline kfloat XMLAttributeTemplate<StringType>::getFloat()
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
inline XMLAttributeTemplate<std::string>::XMLAttributeTemplate(const std::string& name, kfloat value) : mName(name)
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
inline void XMLAttributeTemplate<std::string>::setFloat(const kfloat value)
{
	static char szValue[64];
	snprintf(szValue, 64, "%f", CastToFloat(value));
	mValue = szValue;
}

template<>
inline int XMLAttributeTemplate<std::string>::getInt()
{
	// when no test is required, use atoi insteed of sscanf 
	/*int temp=0;

	sscanf(mValue.c_str( ), "%d",&temp);*/

	return atoi(mValue.c_str());
}

template<>
inline int XMLAttributeTemplate<std::string_view>::getInt()
{
	// when no test is required, use atoi insteed of sscanf 
	/*int temp=0;

	sscanf(mValue.c_str( ), "%d",&temp);*/
	std::string to_string(mValue);
	return atoi(to_string.c_str());
}

template<>
inline kfloat XMLAttributeTemplate<std::string>::getFloat()
{
	// when no test is required, use atof insteed of sscanf 
	/*float temp=0;

	sscanf(mValue.c_str( ),"%f",&temp);*/

	return atof(mValue.c_str());
}

template<>
inline kfloat XMLAttributeTemplate<std::string_view>::getFloat()
{
	// when no test is required, use atof insteed of sscanf 
	/*float temp=0;

	sscanf(mValue.c_str( ),"%f",&temp);*/
	std::string to_string(mValue);
	return atof(to_string.c_str());
}
#endif



#endif 
