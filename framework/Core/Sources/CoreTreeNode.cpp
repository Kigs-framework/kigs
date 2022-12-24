#include "CoreTreeNode.h"

using namespace Kigs::Core;

//! destructor
CoreTreeNode::~CoreTreeNode()
{
	for (auto i : mChildren)
	{
		CoreTreeNode* sonNode = i.second;
		if (sonNode)
		{
			delete sonNode;
		}
	}
}

void CoreTreeNode::AddInstance(CoreModifiable* toAdd)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	mInstances.push_back({ toAdd, toAdd->SharedFromThis() });
	if(mHasNameLookup)
		mNameLookup[toAdd->getName()] = mInstances.size() - 1;
}

void CoreTreeNode::RemoveInstance(CoreModifiable* toRemove)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	for (auto i = mInstances.begin(); i != mInstances.end(); ++i)
	{
		if (i->first == toRemove)
		{
			auto index = std::distance(mInstances.begin(), i);
			if (mHasNameLookup)
				mNameLookup.erase(toRemove->getName());
			if (index != mInstances.size() - 1)
			{
				auto& swap_with = mInstances.back();
				if (mHasNameLookup)
				{
					if (auto ptr = swap_with.second.lock(); ptr)
					{
						mNameLookup[ptr->getName()] = index;
					}
				}
				std::swap(*i, swap_with);
			}
			mInstances.pop_back();
			break;
		}
	}
}

size_t CoreTreeNode::GetInstanceCount() const
{
	size_t result = mInstances.size();
	if (mChildren.size())
	{
		auto itchild = mChildren.begin();
		while (itchild != mChildren.end())
		{
			if ((*itchild).second)
			{
				result += (*itchild).second->GetInstanceCount();
			}
			++itchild;
		}
	}
	return result;
}

void CoreTreeNode::getRootInstances(std::vector<CMSP>& result, bool recursive) const
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	for (auto i : mInstances)
	{
		if (auto ptr = i.second.lock(); ptr)
		{
			if (ptr->GetParentCount() == 0)
			{
				result.push_back(ptr);
			}
		}
	}
	if (recursive)
	{
		for (auto j : mChildren)
		{
			if (j.second)
				j.second->getRootInstances(result, true);
		}
	}
}

void CoreTreeNode::getInstances(std::vector<CMSP>& result, bool recursive, bool only_one) const
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	for (auto i : mInstances)
	{
		if (auto ptr = i.second.lock(); ptr)
		{
			result.push_back(ptr);
			if (only_one) return;
		}
	}
	if (recursive)
	{
		for (auto it : mChildren)
		{
			if (it.second)
			{
				it.second->getInstances(result, true, only_one);
				if (only_one && result.size())
					break;
			}
		}
	}
}

void CoreTreeNode::getInstancesByName(std::vector<CMSP>& result, bool recursive, const std::string& name, bool only_one)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	if (mInstances.size())
	{
		KigsID searchedOne = name;
		if (!mHasNameLookup && only_one)
		{
			size_t i = 0;
			for (auto& el : mInstances)
			{
				if (auto ptr = el.second.lock(); ptr)
				{
					mNameLookup[ptr->getName()] = i;
				}
				++i;
			}
			mHasNameLookup = true;
		}
		if (only_one)
		{
			auto itfind = mNameLookup.find(name);
			if (itfind != mNameLookup.end())
			{
				if (auto ptr = mInstances[itfind->second].second.lock(); ptr)
				{
					result.push_back(ptr);
					return;
				}
			}
		}
		else
		{
			auto i = mInstances.begin();
			auto e = mInstances.end();
			for (; i != e; ++i)
			{
				if (auto ptr = i->second.lock(); ptr)
				{
					if (ptr->getNameID().toUInt() == searchedOne.toUInt())
					{
						if (ptr->getName() == name)
						{
							result.push_back(ptr);
						}
					}
				}
			}
		}
	}
	if (recursive)
	{
		for (auto j = mChildren.begin();
			j != mChildren.end();
			++j)
		{
			if ((*j).second)
			{
				(*j).second->getInstancesByName(result, true, name, only_one);
				if (only_one && result.size())
					break;
			}
		}
	}
}

