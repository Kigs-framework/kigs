#include "CoreTreeNode.h"


//! destructor
CoreTreeNode::~CoreTreeNode()
{
	for (auto i : myChildren)
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
	std::lock_guard<std::recursive_mutex> lk{ myMutex };
	myInstances.push_back(toAdd);
	myInstanceVectorNeedSort = true;
}

void CoreTreeNode::RemoveInstance(CoreModifiable* toRemove)
{
	std::lock_guard<std::recursive_mutex> lk{ myMutex };

	std::vector< CoreModifiable* >::iterator	i;
	std::vector< CoreModifiable* >::iterator	e = myInstances.end();

	for (i = myInstances.begin(); i != e; ++i)
	{
		if ((*i) == toRemove)
		{
			myInstances.erase(i);
			break;
		}
	}
}

size_t CoreTreeNode::GetInstanceCount() const
{
	size_t result = myInstances.size();
	if (myChildren.size())
	{
		auto itchild = myChildren.begin();
		while (itchild != myChildren.end())
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
	std::lock_guard<std::recursive_mutex> lk{ myMutex };
	for (auto i : myInstances)
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
		for (auto it : myChildren)
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
	std::lock_guard<std::recursive_mutex> lk{ myMutex };
	if (myInstances.size())
	{
		KigsID searchedOne = name;
		if (myInstanceVectorNeedSort)
		{
			sortInstanceVector();
		}
		std::vector<CoreModifiable*>::const_iterator i = myInstances.begin();
		std::vector<CoreModifiable*>::const_iterator e = myInstances.end();

		size_t	currentSize = myInstances.size();

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
		for (auto j = myChildren.begin();
			j != myChildren.end();
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
	std::lock_guard<std::recursive_mutex> lk{ myMutex };
	for (auto i : myInstances)
	{
		CoreModifiable* testFather = ((CoreModifiable*)i);
		if (testFather->GetParentCount() == 0)
		{
			if (i->TryGetRef())
			{
				result.push_back(OwningRawPtrToSmartPtr(static_cast<CoreModifiable*>(i)));
			}
		}
	}
	if (recursive)
	{
		for (auto j : myChildren)
		{
			if (j.second)
				j.second->getRootInstances(result, true);
		}
	}
}

void CoreTreeNode::getInstances(std::vector<CMSP>& result, bool recursive, bool only_one) const
{
	std::lock_guard<std::recursive_mutex> lk{ myMutex };
	for (auto i : myInstances)
	{
		if (i->TryGetRef())
		{
			result.push_back(OwningRawPtrToSmartPtr(static_cast<CoreModifiable*>(i)));
			if (only_one) return;
		}
	}
	if (recursive)
	{
		for (auto it : myChildren)
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
	std::lock_guard<std::recursive_mutex> lk{ myMutex };
	if (myInstances.size())
	{
		KigsID searchedOne = name;
		if (myInstanceVectorNeedSort)
		{
			sortInstanceVector();
		}
		std::vector<CoreModifiable*>::const_iterator i = myInstances.begin();
		std::vector<CoreModifiable*>::const_iterator e = myInstances.end();

		size_t	currentSize = myInstances.size();

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
					if((*i)->TryGetRef())
					{
						result.push_back(OwningRawPtrToSmartPtr(static_cast<CoreModifiable*>(*i)));
						if (only_one) return;
					}
				}
			}
		}
	}
	if (recursive)
	{
		for (auto j = myChildren.begin();
			j != myChildren.end();
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
	std::sort(myInstances.begin(), myInstances.end(), [](const CoreModifiable* a1, const CoreModifiable* a2)
		{
			if (a1->getNameID().toUInt() == a2->getNameID().toUInt())
			{
				return ((uptr)a1 < (uptr)a2);
			}

			return a1->getNameID().toUInt() < a2->getNameID().toUInt();
		}
	);
	myInstanceVectorNeedSort = false;
}

