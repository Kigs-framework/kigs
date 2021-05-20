#include "CoreTreeNode.h"


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
	//mInstanceVectorNeedSort = true;
}

void CoreTreeNode::RemoveInstance(CoreModifiable* toRemove)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	for (auto i = mInstances.begin(); i != mInstances.end(); ++i)
	{
		if (i->first == toRemove)
		{
			mInstances.erase(i);
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

/*void CoreTreeNode::getInstances(std::set<CMSP>& instances, bool recursive, bool only_one) const
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	for (auto i : mInstances)
	{
		bool ok = true;
		if (getref) ok = i->TryGetRef();
		if (ok)
		{
			instances.insert(static_cast<CoreModifiable*>(i));
			if (only_one) return;
		}
	}
	if (recursive)
	{
		for (auto it : mChildren)
		{
			if (it.second)
			{
				it.second->getInstances(instances, true, only_one, getref);
				if (only_one && instances.size())
					break;
			}

		}
	}
}

void CoreTreeNode::getInstancesByName(std::set<CoreModifiable*>& instances, bool recursive, const std::string& name, bool only_one, bool getref)
{
	std::lock_guard<std::recursive_mutex> lk{ mMutex };
	if (mInstances.size())
	{
		KigsID searchedOne = name;
		if (mInstanceVectorNeedSort)
		{
			sortInstanceVector();
		}
		std::vector<CoreModifiable*>::const_iterator i = mInstances.begin();
		std::vector<CoreModifiable*>::const_iterator e = mInstances.end();

		size_t	currentSize = mInstances.size();

		// little dichotomie to reduce the test set
		while (currentSize > 8)
		{
			currentSize = currentSize / 2;
			std::vector<CoreModifiable*>::const_iterator mid = i;
			std::advance(mid, currentSize);
			if (searchedOne.toUInt() == (*mid)->getNameID().toUInt())
			{
				break;
			}
			else if ((*mid)->getNameID().toUInt() < searchedOne.toUInt())
			{
				i = mid;
			}
			else
			{
				e = mid;
			}
		}

		for (; i != e; ++i)
		{
			if ((*i)->getNameID().toUInt() == searchedOne.toUInt())
			{
				if ((*i)->getName() == name)
				{
					bool ok = true;
					if (getref) ok = (*i)->TryGetRef();
					if (ok)
					{
						instances.insert((CoreModifiable*)*i);
						if (only_one) return;
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
				(*j).second->getInstancesByName(instances, true, name, only_one, getref);
				if (only_one && instances.size())
					break;
			}
		}
	}
}
*/


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
		if (mInstanceVectorNeedSort)
		{
			// sortInstanceVector();
		}
		auto i = mInstances.begin();
		auto e = mInstances.end();

		//size_t	currentSize = mInstances.size();

		// little dichotomie to reduce the test set
		// NOTE(antoine) with weak_ptr, it's not clear to me how to do the dichitomie
		// Until we redo that, I've diasbled the sorting of mInstances, as it's not necessary

		/*while (currentSize > 8)
		{
			currentSize = currentSize / 2;
			auto mid = i;
			std::advance(mid, currentSize);
			if (searchedOne.toUInt() == (*mid)->getNameID().toUInt())
			{
				break;
			}
			else if ((*mid)->getNameID().toUInt() < searchedOne.toUInt())
			{
				i = mid;
			}
			else
			{
				e = mid;
			}
		}*/

		for (; i != e; ++i)
		{
			if (auto ptr = i->second.lock(); ptr)
			{
				if (ptr->getNameID().toUInt() == searchedOne.toUInt())
				{
					if (ptr->getName() == name)
					{
						bool ok = true;
						result.push_back(ptr);
						if (only_one) return;
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

void CoreTreeNode::sortInstanceVector()
{
	/*
	std::sort(mInstances.begin(), mInstances.end(), [](const CoreModifiable* a1, const CoreModifiable* a2)
		{
			if (a1->getNameID().toUInt() == a2->getNameID().toUInt())
			{
				return ((uptr)a1 < (uptr)a2);
			}

			return a1->getNameID().toUInt() < a2->getNameID().toUInt();
		}
	);
	mInstanceVectorNeedSort = false;
	*/
}

