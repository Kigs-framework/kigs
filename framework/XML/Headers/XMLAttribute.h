#ifndef __XML_ATTRIBUTE_H__
#define __XML_ATTRIBUTE_H__

#include "kstlstring.h"
#include "kTypes.h"
#include "PreallocatedClassNew.h"

class XMLAttributeBase
{};

// ****************************************
// * XMLAttributeTemplate class
// * --------------------------------------
/*!  \class XMLAttributeTemplate
      manage one xml attribute
	  \ingroup XML
*/
// ****************************************

template<typename StringType>
class XMLAttributeTemplate : public XMLAttributeBase
{
	DECLARE_PREALLOCATED_NEW(XMLAttributeTemplate,4096)

public:
	//! constructor for string attributes
    XMLAttributeTemplate( const StringType& name, const StringType& value ) : m_name(name), m_value(value)
	{

	}
	//! constructor for string attributes
	XMLAttributeTemplate(const char* name, const char* value) : m_name(name), m_value(value)
	{

	}
	//! constructor for string attributes
	XMLAttributeTemplate(const char* name,unsigned int namelen, const char* value, unsigned int valuelen) : m_name(name, namelen), m_value(value, valuelen)
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
		m_value = value;
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
		return m_name;
	}
	void setName(const StringType& n)
	{
		m_name = n;
	}
	//! return value as a string
	const StringType& getString( )
	{
		return m_value;
	}

	//! return value as an int
	int getInt( );

	//! return value as a kfloat
	kfloat getFloat( );

private:
	//! name
	StringType m_name;
	//! value
	StringType m_value;
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
	m_value = std::string(szValue,sz.ptr- szValue);
}

template<>
inline void XMLAttributeTemplate<std::string>::setFloat(const kfloat value)
{
	static char szValue[64];
	auto sz = std::to_chars(szValue, szValue + 64, value);
	m_value = std::string(szValue, sz.ptr - szValue);
}

template<>
inline XMLAttributeTemplate<std::string>::XMLAttributeTemplate(const std::string& name, int value) : m_name(name)
{
	setInt(value);
}

template<>
inline XMLAttributeTemplate<std::string>::XMLAttributeTemplate(const std::string& name, kfloat value) : m_name(name)
{
	setFloat(value);
}

template<typename StringType>
int XMLAttributeTemplate<StringType>::getInt()
{
	int result = 0;
	std::from_chars(m_value.data(), m_value.data() + m_value.size(), result);
	return result;
}

template<typename StringType>
kfloat XMLAttributeTemplate<StringType>::getFloat()
{
	float result = 0.0f;
	std::from_chars(m_value.data(), m_value.data() + m_value.size(), result);
	return result;
}

#else

template<>
inline XMLAttributeTemplate<std::string>::XMLAttributeTemplate(const std::string& name, int value) : m_name(name)
{
	static char szValue[64];
	snprintf(szValue, 64, "%d", value);
	m_value = szValue;
}

template<>
inline XMLAttributeTemplate<std::string>::XMLAttributeTemplate(const std::string& name, kfloat value) : m_name(name)
{
	static char szValue[64];
	snprintf(szValue, 64, "%f", CastToFloat(value));
	m_value = szValue;
}

template<>
inline void XMLAttributeTemplate<std::string>::setInt(const int value)
{
	static char szValue[64];
	snprintf(szValue, 64, "%d", value);
	m_value = szValue;
}

template<>
inline void XMLAttributeTemplate<std::string>::setFloat(const kfloat value)
{
	static char szValue[64];
	snprintf(szValue, 64, "%f", CastToFloat(value));
	m_value = szValue;
}

template<typename StringType>
int XMLAttributeTemplate<StringType>::getInt()
{
	// when no test is required, use atoi insteed of sscanf 
	/*int temp=0;

	sscanf(m_value.c_str( ), "%d",&temp);*/

	return atoi(((std::string)m_value).c_str());
}

template<typename StringType>
kfloat XMLAttributeTemplate<StringType>::getFloat()
{
	// when no test is required, use atof insteed of sscanf 
	/*float temp=0;

	sscanf(m_value.c_str( ),"%f",&temp);*/

	return (kfloat)atof(((std::string)m_value).c_str());
}
#endif



#endif 
