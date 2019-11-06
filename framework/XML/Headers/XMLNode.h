#ifndef __XML_NODE_H__
#define __XML_NODE_H__

#include "kTypes.h"
#include "kstlstring.h"
#include "kstlvector.h"
#include "PreallocatedClassNew.h"
#include <type_traits>
#include "MinimalXML.h"
#include "XMLAttribute.h"

#ifdef KIGS_TOOLS
  #define USE_PARENT_AND_SIBLING
#endif

//! enum for different kinds of nodes
enum XMLNodeType
{
    XML_NODE_UNKNOWN,
	XML_NODE_DOCUMENT,
	XML_NODE_ELEMENT,
	XML_NODE_COMMENT,
	XML_NODE_TEXT,
    XML_NODE_TEXT_NO_CHECK,
	XML_NODE_DECLARATION
};


class XMLNodeBase
{
public:

	XMLNodeBase() :
		m_type(XML_NODE_UNKNOWN)
#ifdef USE_PARENT_AND_SIBLING
		, m_parent(0)
		, m_nextSibling(0)
#endif // USE_PARENT_AND_SIBLING
	{

	}

	XMLNodeBase(XMLNodeType type) : m_type(type)
#ifdef USE_PARENT_AND_SIBLING
		, m_parent(0)
		, m_nextSibling(0)
#endif // USE_PARENT_AND_SIBLING
	{

	}

	virtual ~XMLNodeBase( )
	{
	}

	//! set node type
	void setType(XMLNodeType type)
	{
		m_type = type;
	}

	//! return node type
	XMLNodeType getType()
	{
		return m_type;
	}
#ifdef USE_PARENT_AND_SIBLING	

	//! return node's parent
	XMLNodeBase* getParent()
	{
		return m_parent;
	}
	//! set current node's parent
	void setParent(XMLNodeBase* parent)
	{
		m_parent = parent;
	}
	//! return next "brother" node
	XMLNodeBase* getNextSibling() 
	{
		return m_nextSibling;
	}

	//! set next "brother" node
	void setNextSibling(XMLNodeBase* nextSibling)
	{
		m_nextSibling = nextSibling;
	}
#endif // USE_PARENT_AND_SIBLING

	virtual void addChild(XMLNodeBase* child)=0;

protected:
	//! node type
	XMLNodeType m_type;

#ifdef USE_PARENT_AND_SIBLING
	//! node parent
	XMLNodeBase* m_parent;
	//! node next "brother" in parent list
	XMLNodeBase* m_nextSibling;
#endif // USE_PARENT_AND_SIBLING
};

// ****************************************
// * XMLNodeTemplate class
// * --------------------------------------
/*!  \class XMLNodeTemplate
      manage a xml node in the xml hierarchy
	  \ingroup XML
*/
// ****************************************
template<typename StringType>
class XMLNodeTemplate : public XMLNodeBase
{
	static_assert(std::is_same<StringType, std::string>::value || std::is_same<StringType, std::string_view>::value,	"XMLNodeTemplate can only use string or string_view");
	DECLARE_PREALLOCATED_NEW(XMLNodeTemplate, 4096)
public:
	//! constructor
    XMLNodeTemplate( ) : XMLNodeBase() {}
	//! constructor for the given type
    XMLNodeTemplate( XMLNodeType type ) : XMLNodeBase(type) {}

	//! constructor for the given type and name
    inline XMLNodeTemplate( XMLNodeType type, const StringType&name );
	inline XMLNodeTemplate(XMLNodeType type, const char *name);
	inline XMLNodeTemplate(XMLNodeType type, const char* name,unsigned int strsize);

	//! destructor ( no need for virtual as XMLNodeTemplate will not have inheritance )
    virtual ~XMLNodeTemplate( );


	//! set node name
	void setName( const StringType& name )
	{
		m_name = name;
	}
	//! set node value with given string
	void setString( const StringType &value )
	{
		m_value = value;
	}
	
	//! set node value with given int
	void setInt(const int value)
	{
		assert(0); // only with std::string nodes
	}
	//! set node value with given kfloat
	void setFloat(const kfloat value)
	{
		assert(0); // only with std::string nodes
	}

	//! return node name
	const StringType& getName( )
	{
		return m_name;
	}
	//! return node value as a string
	const StringType& getString( )
	{
		return m_value;
	}
	//! return node value as an int
	inline int getInt( );
	//! return node value as a kfloat
	inline kfloat getFloat( );

	//! return node's children count
    int getChildCount( )
	{
		return (int)m_childs.size();
	}
	
	//! return child node by index
    inline XMLNodeTemplate *getChildElement( unsigned int index = 0 );
	//! return child node by name
	inline XMLNodeTemplate *getChildElement( const StringType &name );
    
	//! create a new child with given name and return the corresponding node
    inline XMLNodeTemplate *addChildElement( const StringType&childName);
	//! create a new child with given name and text value and return the corresponding node
    inline  XMLNodeTemplate *addChildElementText( const StringType& childName, const StringType& textValue, bool textValueNoCheck = false );
	//! create a new child with given name and int value and return the corresponding node
    inline XMLNodeTemplate *addChildElementInteger( const StringType &childName, int integerValue );

	//! return attribute count for this node
    int getAttributeCount( )
	{
		return (int)m_attributes.size();
	}
	//! return one attribute by index
	inline XMLAttributeTemplate<StringType>* getAttribute( int index );
	//! return one attribute by name

	XMLAttributeTemplate<StringType>* getAttribute( const std::string_view& name );
	XMLAttributeTemplate<StringType>* getAttribute(const std::string_view& a, const std::string_view& b);

	//! return one attribute by name and remove it from list (useful when parsing xml struct and you are sure you will access attribute only once)
	// so next searches are faster (but you need to delete XMLAttributeTemplate<StringType> yourself).
	XMLAttributeTemplate<StringType>* getAndRemoveAttribute(const std::string_view&name);
	XMLAttributeTemplate<StringType>* getAndRemoveAttribute(const std::string_view& a, const std::string_view& b);


	//! add the given attribute to the node
	inline void addAttribute( XMLAttributeTemplate<StringType> *attribute );
	void reserveAttribute(int count)
	{
		m_attributes.reserve(count);
	}
	
	//! remove the given attribute from the node
	inline void removeAttribute(XMLAttributeTemplate<StringType> *attribute);

	//! add the given child to the node
	virtual void addChild( XMLNodeBase *child ) override;
	//! remove the given child to the node
	inline void removeChild(XMLNodeTemplate *child);


	// return the list of all nodes with the given type in hierarchy
	inline kstl::vector<XMLNodeTemplate*>	getNodes(XMLNodeType type);


	inline bool nameOneOf(const char* a, const char* b);

private:

	//! node name
	StringType m_name;
	//! node value
	StringType m_value;

	//! node attributes
	kstl::vector< XMLAttributeTemplate<StringType> * > m_attributes;
	//! node children
	kstl::vector< XMLNodeTemplate * > m_childs;



	// return the list of all nodes with the given type in hierarchy
	void getNodes(XMLNodeType type, kstl::vector<XMLNodeTemplate<StringType>*>& list);
};


typedef XMLNodeTemplate<std::string> XMLNode;
typedef XMLNodeTemplate<std::string_view> XMLNodeStringRef;

#ifndef JAVASCRIPT
template<typename T>
IMPLEMENT_PREALLOCATED_NEW(XMLNodeTemplate<T>, 4096)
#endif

template<typename StringType>
XMLNodeTemplate<StringType>::XMLNodeTemplate(XMLNodeType type, const StringType& name) : XMLNodeBase(type), m_name(name)
{

}

template<typename StringType>
XMLNodeTemplate<StringType>::XMLNodeTemplate(XMLNodeType type, const char* name) : XMLNodeBase(type), m_name(name)
{

}

template<typename StringType>
XMLNodeTemplate<StringType>::XMLNodeTemplate(XMLNodeType type, const char* name, unsigned int strsize) : XMLNodeBase(type), m_name(name, strsize)
{

}

template<typename StringType>
XMLNodeTemplate<StringType>::~XMLNodeTemplate()
{
	unsigned int i;

	for (i = 0; i < m_attributes.size(); ++i)
	{
		delete m_attributes.at(i);
	}
	m_attributes.clear();

	for (i = 0; i < m_childs.size(); ++i)
	{
		delete m_childs.at(i);
	}
	m_childs.clear();
}

template<>
inline void XMLNodeTemplate<std::string>::setInt(const int value)
{
	static char szValue[64];
	snprintf(szValue, 64, "%d", value);
	m_value = szValue;
}

template<>
inline void XMLNodeTemplate<std::string>::setFloat(const kfloat value)
{
	static char szValue[64];
	snprintf(szValue, 64, "%f", CastToFloat(value));
	m_value = szValue;
}


template<typename StringType>
int XMLNodeTemplate<StringType>::getInt()
{
	// when no test is required, use atoi insteed of sscanf 
	/*
	int temp=0;

	sscanf("%d",m_value.c_str( ),temp);*/

	return atoi(m_value.c_str());
}

template<typename StringType>
kfloat XMLNodeTemplate<StringType>::getFloat()
{
	// when no test is required, use atof insteed of sscanf 
	/*float temp=0.0f;

	sscanf(m_value.c_str(),"%f",&temp);*/

	return (kfloat)atof(m_value.c_str());
}

template<typename StringType>
void XMLNodeTemplate<StringType>::addAttribute(XMLAttributeTemplate<StringType>* attribute)
{
	m_attributes.push_back(attribute);
}

//! remove the given attribute from the node (not delete)
template<typename StringType>
void XMLNodeTemplate<StringType>::removeAttribute(XMLAttributeTemplate<StringType>* attribute)
{
	auto itb = m_attributes.begin();
	auto ite = m_attributes.end();
	while (itb != ite)
	{
		if ((*itb) == attribute)
		{
			m_attributes.erase(itb);
			break;
		}
		++itb;
	}
}

template<typename StringType>
void XMLNodeTemplate<StringType>::addChild(XMLNodeBase* child)
{
	m_childs.push_back((XMLNodeTemplate<StringType>*)child);
}

template<typename StringType>
void XMLNodeTemplate<StringType>::removeChild(XMLNodeTemplate* child)
{
	auto it = m_childs.begin();
	for (; it != m_childs.end(); ++it) {
		if (*it == child) {
			m_childs.erase(it);
			break;
		}
	}
}



template<typename StringType>
XMLAttributeTemplate<StringType>* XMLNodeTemplate<StringType>::getAttribute(int index)
{
	return m_attributes.at((unsigned int)index);
}

template<typename StringType>
XMLAttributeTemplate<StringType>* XMLNodeTemplate<StringType>::getAttribute(const std::string_view& name)
{
	for (unsigned int i = 0; i < m_attributes.size(); ++i)
	{
		if (m_attributes.at(i)->getName() == name)
		{
			return m_attributes.at(i);
		}
	}

	return 0;
}

template<typename StringType>
XMLAttributeTemplate<StringType>* XMLNodeTemplate<StringType>::getAttribute(const std::string_view& a, const std::string_view& b)
{
	XMLAttributeTemplate<StringType>* result = getAttribute(a);
	if (!result) result = getAttribute(b);
	return result;
}

template<typename StringType>
XMLAttributeTemplate<StringType>* XMLNodeTemplate<StringType>::getAndRemoveAttribute(const std::string_view& name)
{
	size_t attrl = m_attributes.size();
	for (unsigned int i = 0; i < attrl; ++i)
	{
		if (m_attributes.at(i)->getName() == name)
		{
			XMLAttributeTemplate<StringType>* result = m_attributes.at(i);
			m_attributes[i] = m_attributes[attrl - 1];
			m_attributes.pop_back();
			return result;
		}
	}

	return 0;
}

template<typename StringType>
XMLAttributeTemplate<StringType>* XMLNodeTemplate<StringType>::getAndRemoveAttribute(const std::string_view& a, const std::string_view& b)
{
	XMLAttributeTemplate<StringType>* result = getAndRemoveAttribute(a);
	if (!result) result = getAndRemoveAttribute(b);
	return result;
}

template<typename StringType>
XMLNodeTemplate<StringType>* XMLNodeTemplate<StringType>::addChildElement(const StringType& childName)
{
	XMLNodeTemplate* node = new XMLNodeTemplate(XML_NODE_ELEMENT, childName);

	addChild(node);

	return node;
}


template<typename StringType>
XMLNodeTemplate<StringType>* XMLNodeTemplate<StringType>::addChildElementText(const StringType& childName, const StringType& textValue, bool textValueNoCheck)
{
	XMLNodeTemplate* node = new XMLNodeTemplate();
	node->setType(XML_NODE_ELEMENT);
	node->setName(childName);

	XMLNodeTemplate* nodeChild = new XMLNodeTemplate();
	if (!textValueNoCheck)
	{
		nodeChild->setType(XML_NODE_TEXT);
	}
	else
	{
		nodeChild->setType(XML_NODE_TEXT_NO_CHECK);
	}
	nodeChild->setString(textValue);

	node->addChild(nodeChild);
	addChild(node);

	return node;
}

template<typename StringType>
XMLNodeTemplate<StringType>* XMLNodeTemplate<StringType>::addChildElementInteger(const StringType& childName, int integerValue)
{
	kstl::string textValue("%d", (unsigned int)integerValue);

	XMLNodeTemplate* node = new XMLNodeTemplate();
	node->setType(XML_NODE_ELEMENT);
	node->setName(childName);

	XMLNodeTemplate* nodeChild = new XMLNodeTemplate();
	nodeChild->setType(XML_NODE_TEXT);
	nodeChild->setString(textValue);

	node->addChild(nodeChild);
	addChild(node);

	return node;
}


template<typename StringType>
XMLNodeTemplate<StringType>* XMLNodeTemplate<StringType>::getChildElement(unsigned int index)
{
	if (m_childs.size() <= index)
	{
		return 0;
	}

	return m_childs.at(index);
}

template<typename StringType>
XMLNodeTemplate<StringType>* XMLNodeTemplate<StringType>::getChildElement(const StringType& name)
{
	unsigned int i;

	for (i = 0; i < m_childs.size(); ++i)
	{
		if (m_childs.at(i)->getName() == name)
		{
			return m_childs.at(i);
		}
	}

	for (i = 0; i < m_childs.size(); ++i)
	{
		XMLNodeTemplate* node = m_childs.at(i)->getChildElement(name);
		if (node)
		{
			return node;
		}
	}

	return 0;
}

template<typename StringType>
kstl::vector<XMLNodeTemplate<StringType>*>	XMLNodeTemplate<StringType>::getNodes(XMLNodeType type)
{
	kstl::vector<XMLNodeTemplate<StringType>*> result;
	result.clear();

	getNodes(type, result);

	return result;
}

template<typename StringType>
void XMLNodeTemplate<StringType>::getNodes(XMLNodeType type, kstl::vector<XMLNodeTemplate*>& list)
{
	if (this->getType() == type)
	{
		list.push_back(this);
	}
	unsigned int i;

	for (i = 0; i < m_childs.size(); ++i)
	{
		m_childs.at(i)->getNodes(type, list);
	}
}

template<typename StringType>
bool XMLNodeTemplate<StringType>::nameOneOf(const char* a, const char* b)
{
	return (m_name == a) || (m_name == b);
}


#endif
