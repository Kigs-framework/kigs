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


// ****************************************
// * XMLNodeBase class
// * --------------------------------------
/**
 * \file	XMLNode.h
 * \class	XMLNodeBase
 * \ingroup ModuleXML
 * \brief	Base class for templated XML Node class.
 */
 // ****************************************
class XMLNodeBase 
{
public:

	XMLNodeBase() :
		mType(XML_NODE_UNKNOWN)
#ifdef USE_PARENT_AND_SIBLING
		, mParent(0)
		, mNextSibling(0)
#endif // USE_PARENT_AND_SIBLING
	{

	}

	XMLNodeBase(XMLNodeType type) : mType(type)
#ifdef USE_PARENT_AND_SIBLING
		, mParent(0)
		, mNextSibling(0)
#endif // USE_PARENT_AND_SIBLING
	{

	}

	virtual ~XMLNodeBase( )
	{
	}

	//! set node type
	void setType(XMLNodeType type)
	{
		mType = type;
	}

	//! return node type
	XMLNodeType getType()
	{
		return mType;
	}
#ifdef USE_PARENT_AND_SIBLING	

	//! return node's parent
	XMLNodeBase* getParent()
	{
		return mParent;
	}
	//! set current node's parent
	void setParent(XMLNodeBase* parent)
	{
		mParent = parent;
	}
	//! return next "brother" node
	XMLNodeBase* getNextSibling() 
	{
		return mNextSibling;
	}

	//! set next "brother" node
	void setNextSibling(XMLNodeBase* nextSibling)
	{
		mNextSibling = nextSibling;
	}
#endif // USE_PARENT_AND_SIBLING

	virtual void addChild(XMLNodeBase* child)=0;

protected:
	//! node type
	XMLNodeType mType;

#ifdef USE_PARENT_AND_SIBLING
	//! node parent
	XMLNodeBase* mParent;
	//! node next "brother" in parent list
	XMLNodeBase* mNextSibling;
#endif // USE_PARENT_AND_SIBLING
};


// ****************************************
// * XMLNodeTemplate class
// * --------------------------------------
/**
 * \file	XMLNode.h
 * \class	XMLNodeTemplate
 * \ingroup ModuleXML
 * \brief	Manage XML node structure using string or string_view.
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
		mName = name;
	}
	//! set node value with given string
	void setString( const StringType &value )
	{
		mValue = value;
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
	const StringType& getName( ) const
	{
		return mName;
	}
	//! return node value as a string
	const StringType& getString( )
	{
		return mValue;
	}
	//! return node value as an int
	inline int getInt( );
	//! return node value as a kfloat
	inline kfloat getFloat( );

	//! return node's children count
    int getChildCount( )
	{
		return (int)mChildren.size();
	}

	//! return child node by index
    inline XMLNodeTemplate *getChildElement( unsigned int index = 0 );
	//! return child node by name
	inline XMLNodeTemplate *getChildElement( const std::string &name );
    
	//! create a new child with given name and return the corresponding node
    inline XMLNodeTemplate *addChildElement( const StringType&childName);
	//! create a new child with given name and text value and return the corresponding node
    inline  XMLNodeTemplate *addChildElementText( const StringType& childName, const StringType& textValue, bool textValueNoCheck = false );
	//! create a new child with given name and int value and return the corresponding node
    inline XMLNodeTemplate *addChildElementInteger( const StringType &childName, int integerValue );

	//! return attribute count for this node
    int getAttributeCount( )
	{
		return (int)mAttributes.size();
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
		mAttributes.reserve(count);
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
	StringType mName;
	//! node value
	StringType mValue;

	//! node attributes
	kstl::vector< XMLAttributeTemplate<StringType> * > mAttributes;
	//! node children
	kstl::vector< XMLNodeTemplate * > mChildren;



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
XMLNodeTemplate<StringType>::XMLNodeTemplate(XMLNodeType type, const StringType& name) : XMLNodeBase(type), mName(name)
{

}

template<typename StringType>
XMLNodeTemplate<StringType>::XMLNodeTemplate(XMLNodeType type, const char* name) : XMLNodeBase(type), mName(name)
{

}

template<typename StringType>
XMLNodeTemplate<StringType>::XMLNodeTemplate(XMLNodeType type, const char* name, unsigned int strsize) : XMLNodeBase(type), mName(name, strsize)
{

}

template<typename StringType>
XMLNodeTemplate<StringType>::~XMLNodeTemplate()
{
	unsigned int i;

	for (i = 0; i < mAttributes.size(); ++i)
	{
		delete mAttributes.at(i);
	}
	mAttributes.clear();

	for (i = 0; i < mChildren.size(); ++i)
	{
		delete mChildren.at(i);
	}
	mChildren.clear();
}

template<>
inline void XMLNodeTemplate<std::string>::setInt(const int value)
{
	static char szValue[64];
	snprintf(szValue, 64, "%d", value);
	mValue = szValue;
}

template<>
inline void XMLNodeTemplate<std::string>::setFloat(const kfloat value)
{
	static char szValue[64];
	snprintf(szValue, 64, "%f", CastToFloat(value));
	mValue = szValue;
}

#if (__cplusplus >= 201703L || _MSVC_LANG  >= 201703L) && !defined(JAVASCRIPT) && !defined(__clang__)
#include <charconv>
template<typename StringType>
inline int XMLNodeTemplate<StringType>::getInt()
{
	int result = 0;
	std::from_chars(mValue.data(), mValue.data() + mValue.size(), result);
	return result;
}

template<typename StringType>
inline kfloat XMLNodeTemplate<StringType>::getFloat()
{
	float result = 0.0f;
	std::from_chars(mValue.data(), mValue.data() + mValue.size(), result);
	return result;
}
#else
template<>
inline int XMLNodeTemplate<std::string>::getInt()
{
	int result = atoi(mValue.c_str());
	return result;
}

template<>
inline kfloat XMLNodeTemplate<std::string>::getFloat()
{
	float result = atof(mValue.c_str());
	return result;
}
template<>
inline int XMLNodeTemplate<std::string_view>::getInt()
{
	std::string to_string(mValue);
	int result = atoi(to_string.c_str());
	return result;
}

template<>
inline kfloat XMLNodeTemplate<std::string_view>::getFloat()
{
	std::string to_string(mValue);
	float result = atof(to_string.c_str());
	return result;
}
#endif


template<typename StringType>
void XMLNodeTemplate<StringType>::addAttribute(XMLAttributeTemplate<StringType>* attribute)
{
	mAttributes.push_back(attribute);
}

//! remove the given attribute from the node (not delete)
template<typename StringType>
void XMLNodeTemplate<StringType>::removeAttribute(XMLAttributeTemplate<StringType>* attribute)
{
	auto itb = mAttributes.begin();
	auto ite = mAttributes.end();
	while (itb != ite)
	{
		if ((*itb) == attribute)
		{
			mAttributes.erase(itb);
			break;
		}
		++itb;
	}
}

template<typename StringType>
void XMLNodeTemplate<StringType>::addChild(XMLNodeBase* child)
{
	mChildren.push_back((XMLNodeTemplate<StringType>*)child);
}

template<typename StringType>
void XMLNodeTemplate<StringType>::removeChild(XMLNodeTemplate* child)
{
	auto it = mChildren.begin();
	for (; it != mChildren.end(); ++it) {
		if (*it == child) {
			mChildren.erase(it);
			break;
		}
	}
}



template<typename StringType>
XMLAttributeTemplate<StringType>* XMLNodeTemplate<StringType>::getAttribute(int index)
{
	return mAttributes.at((unsigned int)index);
}

template<typename StringType>
XMLAttributeTemplate<StringType>* XMLNodeTemplate<StringType>::getAttribute(const std::string_view& name)
{
	for (unsigned int i = 0; i < mAttributes.size(); ++i)
	{
		if (mAttributes.at(i)->getName() == name)
		{
			return mAttributes.at(i);
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
	size_t attrl = mAttributes.size();
	for (unsigned int i = 0; i < attrl; ++i)
	{
		if (mAttributes.at(i)->getName() == name)
		{
			XMLAttributeTemplate<StringType>* result = mAttributes.at(i);
			mAttributes[i] = mAttributes[attrl - 1];
			mAttributes.pop_back();
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
	if (mChildren.size() <= index)
	{
		return 0;
	}

	return mChildren.at(index);
}

template<typename StringType>
XMLNodeTemplate<StringType>* XMLNodeTemplate<StringType>::getChildElement(const std::string& name)
{
	unsigned int i;

	for (i = 0; i < mChildren.size(); ++i)
	{
		if (mChildren.at(i)->getName() == name)
		{
			return mChildren.at(i);
		}
	}

	for (i = 0; i < mChildren.size(); ++i)
	{
		XMLNodeTemplate* node = mChildren.at(i)->getChildElement(name);
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

	for (i = 0; i < mChildren.size(); ++i)
	{
		mChildren.at(i)->getNodes(type, list);
	}
}

template<typename StringType>
bool XMLNodeTemplate<StringType>::nameOneOf(const char* a, const char* b)
{
	return (mName == a) || (mName == b);
}


#endif
