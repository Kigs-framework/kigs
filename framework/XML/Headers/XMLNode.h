#pragma once

#include "kTypes.h"
#include "kstlstring.h"
#include <vector>
#include "PreallocatedClassNew.h"
#include <type_traits>
#include "MinimalXML.h"
#include "XMLAttribute.h"

#ifdef KIGS_TOOLS
  #define USE_PARENT_AND_SIBLING
#endif

namespace Kigs
{

	namespace Xml
	{
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

			virtual XMLNodeBase* Copy() = 0;

			virtual ~XMLNodeBase()
			{
				unsigned int i;

				for (i = 0; i < mAttributes.size(); ++i)
				{
					delete mAttributes.at(i);
				}
				mAttributes.clear();

				clearAllChildren();
			}

			void	clearAllChildren()
			{
				size_t i;
				for (i = 0; i < mChildren.size(); ++i)
				{
					delete mChildren.at(i);
				}
				mChildren.clear();
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

			virtual void addChild(XMLNodeBase* child) = 0;

			//! return child node by index
			XMLNodeBase* getChildElement(unsigned int index = 0);
			void			deleteChildElement(size_t pos);

			//! return child node by name
			XMLNodeBase* getChildElement(const std::string& name);
			void			deleteChildElement(const std::string& name);

			XMLNodeBase* getChildElementWithAttribute(const std::string& name, const std::string& attrname, const std::string& attrval);
			void			deleteChildElementWithAttribute(const std::string& name, const std::string& attrname, const std::string& attrval);


			//! return node's children count
			int getChildCount()
			{
				return (int)mChildren.size();
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

			inline bool	compareName(const char* n) const
			{
				return compareName((std::string_view)n);
			}


			virtual bool	compareName(const std::string& n) const = 0;
			virtual bool	compareName(const std::string_view& n) const = 0;

			//! return attribute count for this node
			int getAttributeCount()
			{
				return (int)mAttributes.size();
			}
			//! return one attribute by index
			inline XMLAttributeBase* getAttribute(int index)
			{
				return mAttributes.at((unsigned int)index);
			}
			//! return one attribute by name

			XMLAttributeBase* getAttribute(const std::string_view& name);
			XMLAttributeBase* getAttribute(const std::string_view& a, const std::string_view& b);

			//! return one attribute by name and remove it from list (useful when parsing xml struct and you are sure you will access attribute only once)
			// so next searches are faster (but you need to delete XMLAttributeTemplate<StringType> yourself).
			XMLAttributeBase* getAndRemoveAttribute(const std::string_view& name);
			XMLAttributeBase* getAndRemoveAttribute(const std::string_view& a, const std::string_view& b);

			//! return value as an int
			virtual int getInt() const = 0;

			//! return value as a float
			virtual float getFloat() const = 0;

			// return the list of all nodes with the given type in hierarchy
			std::vector<XMLNodeBase*>	getNodes(XMLNodeType type);

			virtual bool nameOneOf(const char* a, const char* b) = 0;

		protected:

			void getNodes(XMLNodeType type, std::vector<XMLNodeBase*>& list);

			virtual void getName(std::string& n) const = 0;
			virtual void getName(std::string_view& n) const = 0;
			virtual void getString(std::string& v) const = 0;
			virtual void getString(std::string_view& v) const = 0;


			//! node type
			XMLNodeType mType;

#ifdef USE_PARENT_AND_SIBLING
			//! node parent
			XMLNodeBase* mParent;
			//! node next "brother" in parent list
			XMLNodeBase* mNextSibling;
#endif // USE_PARENT_AND_SIBLING

			//! node children
			std::vector< XMLNodeBase* > mChildren;

			//! node attributes
			std::vector< XMLAttributeBase* > mAttributes;

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
			static_assert(std::is_same<StringType, std::string>::value || std::is_same<StringType, std::string_view>::value, "XMLNodeTemplate can only use string or string_view");
			DECLARE_PREALLOCATED_NEW(XMLNodeTemplate, 4096)
		public:
			//! constructor
			XMLNodeTemplate() : XMLNodeBase() {}

			//! constructor for the given type
			XMLNodeTemplate(XMLNodeType type) : XMLNodeBase(type) {}

			//! constructor for the given type and name
			inline XMLNodeTemplate(XMLNodeType type, const StringType& name);
			inline XMLNodeTemplate(XMLNodeType type, const char* name);
			inline XMLNodeTemplate(XMLNodeType type, const char* name, unsigned int strsize);


			XMLNodeBase* Copy() override
			{
				XMLNodeTemplate<StringType>* newone = new XMLNodeTemplate<StringType>(mType);
				newone->setName(mName);
				newone->setString(mValue);

				size_t i;

				for (i = 0; i < mAttributes.size(); ++i)
				{
					newone->addAttribute(static_cast<XMLAttributeTemplate<StringType>*>(mAttributes[i]->Copy()));
				}
				for (i = 0; i < mChildren.size(); ++i)
				{
					newone->addChild(static_cast<XMLNodeTemplate<StringType>*>(mChildren[i]->Copy()));
				}
				return newone;
			}


			//! set node name
			void setName(const StringType& name)
			{
				mName = name;
			}
			//! set node value with given string
			void setString(const StringType& value)
			{
				mValue = value;
			}

			//! set node value with given int
			void setInt(const int value)
			{
				assert(0); // only with std::string nodes
			}
			//! set node value with given float
			void setFloat(const float value)
			{
				assert(0); // only with std::string nodes
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

			//! return node value as an int
			inline int getInt() const override;
			//! return node value as a float
			inline float getFloat() const override;


			//! create a new child with given name and return the corresponding node
			inline XMLNodeTemplate* addChildElement(const StringType& childName);
			//! create a new child with given name and text value and return the corresponding node
			inline  XMLNodeTemplate* addChildElementText(const StringType& childName, const StringType& textValue, bool textValueNoCheck = false);
			//! create a new child with given name and int value and return the corresponding node
			inline XMLNodeTemplate* addChildElementInteger(const StringType& childName, int integerValue);

			//! add the given attribute to the node
			inline void addAttribute(XMLAttributeTemplate<StringType>* attribute);
			void reserveAttribute(int count)
			{
				mAttributes.reserve(count);
			}

			//! remove the given attribute from the node
			inline void removeAttribute(XMLAttributeTemplate<StringType>* attribute);

			//! add the given child to the node
			virtual void addChild(XMLNodeBase* child) override;
			//! remove the given child to the node
			inline void removeChild(XMLNodeTemplate* child);

			inline bool nameOneOf(const char* a, const char* b) override;

			inline bool	compareName(const std::string& n) const override
			{
				return (n == mName);
			}
			inline bool	compareName(const std::string_view& n) const override
			{
				return (n == mName);
			}


		private:

			//! node name
			StringType mName;
			//! node value
			StringType mValue;

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

		template<>
		inline void XMLNodeTemplate<std::string>::setInt(const int value)
		{
			static char szValue[64];
			snprintf(szValue, 64, "%d", value);
			mValue = szValue;
		}

		template<>
		inline void XMLNodeTemplate<std::string>::setFloat(const float value)
		{
			static char szValue[64];
			snprintf(szValue, 64, "%f", CastToFloat(value));
			mValue = szValue;
		}
	}
}

#if (__cplusplus >= 201703L || _MSVC_LANG  >= 201703L) && !defined(JAVASCRIPT) && !defined(__clang__)
#include <charconv>
namespace Kigs
{

	namespace Xml
	{
		template<typename StringType>
		inline int XMLNodeTemplate<StringType>::getInt() const
		{
			int result = 0;
			std::from_chars(mValue.data(), mValue.data() + mValue.size(), result);
			return result;
		}

		template<typename StringType>
		inline float XMLNodeTemplate<StringType>::getFloat() const
		{
			float result = 0.0f;
			std::from_chars(mValue.data(), mValue.data() + mValue.size(), result);
			return result;
		}
	}
}
#else
namespace Kigs
{

	namespace Xml
	{
		template<>
		inline int XMLNodeTemplate<std::string>::getInt() const
		{
			int result = atoi(mValue.c_str());
			return result;
		}

		template<>
		inline float XMLNodeTemplate<std::string>::getFloat() const
		{
			float result = atof(mValue.c_str());
			return result;
		}
		template<>
		inline int XMLNodeTemplate<std::string_view>::getInt() const
		{
			std::string to_string(mValue);
			int result = atoi(to_string.c_str());
			return result;
		}

		template<>
		inline float XMLNodeTemplate<std::string_view>::getFloat() const
		{
			std::string to_string(mValue);
			float result = atof(to_string.c_str());
			return result;
		}
	}
}
#endif

namespace Kigs
{

	namespace Xml
	{
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
			std::string textValue("%d", (unsigned int)integerValue);

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
		bool XMLNodeTemplate<StringType>::nameOneOf(const char* a, const char* b)
		{
			return (mName == a) || (mName == b);
		}

	}
}
