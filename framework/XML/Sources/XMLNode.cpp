#include "PrecompiledHeaders.h"

#include "XMLNode.h"
#include "XMLAttribute.h"

#include <algorithm>

using namespace Kigs::Xml;
using namespace Kigs::Core;


XMLNodeBase* XMLNodeBase::getChildElement(unsigned int index)
{
	if (mChildren.size() <= index)
	{
		return 0;
	}

	return mChildren.at(index);
}

void			XMLNodeBase::deleteChildElement(size_t pos)
{
	if (mChildren.size() <= pos)
	{
		return;
	}

	std::vector< XMLNodeBase*>::iterator it = mChildren.begin() + pos;

	delete (*it);
	mChildren.erase(it);
}

XMLNodeBase* XMLNodeBase::getChildElementWithAttribute(const std::string& name, const std::string& attrname, const std::string& attrval)
{
	for (size_t i = 0; i < mChildren.size(); ++i)
	{
		if (mChildren[i]->getName() == name)
		{
			auto attr=mChildren[i]->getAttribute(attrname);
			if (attr && attr->getString() == attrval)
			{
				return mChildren[i];
			}
		}
	}
	return nullptr;
}

void			XMLNodeBase::deleteChildElementWithAttribute(const std::string& name, const std::string& attrname, const std::string& attrval)
{
	std::vector< XMLNodeBase*>::iterator it;
	for (it = mChildren.begin(); it != mChildren.end(); ++it)
	{
		if ((*it)->getName() == name)
		{
			auto attr = (*it)->getAttribute(attrname);
			if (attr && attr->getString() == attrval)
			{
				delete (*it);
				mChildren.erase(it);
				return;
			}
		}
	}
}

XMLNodeBase* XMLNodeBase::getChildElement(const std::string& name)
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
		XMLNodeBase* node = mChildren.at(i)->getChildElement(name);
		if (node)
		{
			return node;
		}
	}

	return 0;
}

void			XMLNodeBase::deleteChildElement(const std::string& name)
{
	std::vector< XMLNodeBase*>::iterator it;
	for (it = mChildren.begin(); it != mChildren.end(); ++it)
	{
		if ((*it)->getName() == name)
		{
			delete (*it);
			mChildren.erase(it);
			return;
		}
	}
}



XMLAttributeBase* XMLNodeBase::getAttribute(const std::string_view& name)
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

XMLAttributeBase* XMLNodeBase::getAttribute(const std::string_view& a, const std::string_view& b)
{
	XMLAttributeBase* result = getAttribute(a);
	if (!result) result = getAttribute(b);
	return result;
}

XMLAttributeBase* XMLNodeBase::getAndRemoveAttribute(const std::string_view& name)
{
	size_t attrl = mAttributes.size();
	for (unsigned int i = 0; i < attrl; ++i)
	{
		if (mAttributes.at(i)->compareName(name))
		{
			XMLAttributeBase* result = mAttributes.at(i);
			mAttributes[i] = mAttributes[attrl - 1];
			mAttributes.pop_back();
			return result;
		}
	}

	return 0;
}

XMLAttributeBase* XMLNodeBase::getAndRemoveAttribute(const std::string_view& a, const std::string_view& b)
{
	XMLAttributeBase* result = getAndRemoveAttribute(a);
	if (!result) result = getAndRemoveAttribute(b);
	return result;
}

std::vector<XMLNodeBase*>	XMLNodeBase::getNodes(XMLNodeType type)
{
	std::vector<XMLNodeBase*> result;
	result.clear();

	getNodes(type, result);

	return result;
}

void XMLNodeBase::getNodes(XMLNodeType type, std::vector<XMLNodeBase*>& list)
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


//IMPLEMENT_TEMPLATED_PREALLOCATED_NEW(XMLNodeTemplate<std::string>, 4096)
//IMPLEMENT_TEMPLATED_PREALLOCATED_NEW(XMLNodeTemplate<std::string_view>, 4096)
