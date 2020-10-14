#include "PrecompiledHeaders.h"

#include "XMLNode.h"
#include "XMLAttribute.h"

#include <algorithm>


XMLNodeBase* XMLNodeBase::getChildElement(unsigned int index)
{
	if (mChildren.size() <= index)
	{
		return 0;
	}

	return mChildren.at(index);
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


XMLAttributeBase* XMLNodeBase::getAttribute(int index)
{
	return mAttributes.at((unsigned int)index);
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
		if (mAttributes.at(i)->getName() == name)
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

kstl::vector<XMLNodeBase*>	XMLNodeBase::getNodes(XMLNodeType type)
{
	kstl::vector<XMLNodeBase*> result;
	result.clear();

	getNodes(type, result);

	return result;
}

void XMLNodeBase::getNodes(XMLNodeType type, kstl::vector<XMLNodeBase*>& list)
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
