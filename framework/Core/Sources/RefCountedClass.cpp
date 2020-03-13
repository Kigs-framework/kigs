#include "PrecompiledHeaders.h"

#include "RefCountedClass.h"

#include "Core.h"
#include <iterator>

std::atomic<unsigned int> RefCountedClass::myUIDCounter{ 0 };
unsigned int RefCountedClass::myDefaultNameCounter = 0;

//! create a default name when no name is given
std::string RefCountedClass::DefaultName()
{
	char name[64];
	snprintf(name, 64, "nobody%d", ++myDefaultNameCounter);
	return name;
}

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

RefCountedClass::ModifiableMethod CoreTreeNode::GetMethod(KigsID id) const
{
	auto it = myMethods.find(id);
	if (it != myMethods.end())
		return it->second.m_Method;
	
	return nullptr;
}

void CoreTreeNode::AddInstance(RefCountedClass* toAdd)
{
	std::lock_guard<std::recursive_mutex> lk{ myMutex };
	myInstances.push_back(toAdd);
	myInstanceVectorNeedSort = true;
}

void CoreTreeNode::RemoveInstance(RefCountedClass* toRemove)
{
	std::lock_guard<std::recursive_mutex> lk{ myMutex };

	std::vector< RefCountedClass* >::iterator	i;
	std::vector< RefCountedClass* >::iterator	e= myInstances.end();

	for ( i =myInstances.begin();i!=e;++i)
	{
		if ((*i) == toRemove)
		{
			myInstances.erase(i);
			break;
		}
	}
}

void CoreTreeNode::getInstances(std::set<CoreModifiable*>& instances, bool recursive, bool only_one, bool getref) const
{
	std::lock_guard<std::recursive_mutex> lk{ myMutex };
	for (auto i : myInstances)
	{
		bool ok = true;
		if(getref) ok = i->TryGetRef();
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
				it.second->getInstances(instances, true, only_one,getref);
				if (only_one && instances.size())
					break;
			}

		}
	}
}

void CoreTreeNode::getRootInstances(std::set<CoreModifiable*>& instances, bool recursive, bool getref) const
{
	std::lock_guard<std::recursive_mutex> lk{ myMutex };
	for (auto i : myInstances)
	{
		CoreModifiable* testFather = ((CoreModifiable*)i);
		if (testFather->GetParentCount() == 0)
		{
			bool ok = true;
			if(getref) ok = (i)->TryGetRef();
			if(ok) instances.insert((CoreModifiable*)i);
		}
	}
	if (recursive)
	{
		for (auto j : myChildren)
		{
			if (j.second)
				j.second->getRootInstances(instances, true,getref);
		}
	}
}

IMPLEMENT_CLASS_INFO(RefCountedClass)

//! static method : return the set of all instances of the given type
void RefCountedClass::GetInstances(const KigsID& cid, std::set<CoreModifiable*>& instances, bool exactTypeOnly, bool only_one, bool getref)
{
	instances.clear();
	CoreTreeNode* node = KigsCore::GetTypeNode(cid);
	if (node) node->getInstances(instances, !exactTypeOnly, only_one,getref);
}

CoreModifiable* RefCountedClass::GetFirstInstance(const KigsID& cid, bool exactTypeOnly, bool getref)
{
	std::set<CoreModifiable*> insts;
	GetInstances(cid, insts, exactTypeOnly, true, getref);
	if (insts.size()) return *insts.begin();
	return nullptr;
}

//! static method : return the set of all root instances of the given type 
void RefCountedClass::GetRootInstances(const KigsID& cid, std::set<CoreModifiable*>& instances, bool exactTypeOnly, bool getref)
{
	instances.clear();
	CoreTreeNode* node = KigsCore::GetTypeNode(cid);
	if (node) node->getRootInstances(instances, !exactTypeOnly, getref);
}

// runtime ID is "name:type:pointer:uid"
void RefCountedClass::GetInstanceByRuntimeID(const std::string &runtimeID, std::set<CoreModifiable*>& instances, bool getref)
{
	instances.clear();

	std::string cid = "";
	std::string name = "";
	std::string searchstring = runtimeID;

	uptr pointer;
	unsigned int  uid;

	size_t lastpos = 0;
	int i;
	for (i = 0; i < 2; i++)
	{
		size_t result = runtimeID.find(':', lastpos);
		if (result == std::string::npos)
		{
			return;
		}
		searchstring = runtimeID.substr(lastpos, result - lastpos);
		switch (i)
		{
		case 0:
			name = runtimeID.substr(0, result);
			//sscanf(searchstring.c_str(),"%s",name);
			break;
		case 1:
			if (searchstring == "")
			{
				searchstring = "CoreModifiable";
			}

			cid = searchstring;
			//	sscanf(searchstring.c_str(),"%s",cid);
			break;
		}
		lastpos = result + 1;
	}
	searchstring = runtimeID.substr(lastpos, runtimeID.size() - lastpos);
	sscanf(searchstring.c_str(), "%lu:%u", &pointer, &uid);

	std::set<CoreModifiable*> searchinstances;
	// retreive name and type
	GetInstancesByName(cid, name, searchinstances,false,false,getref);

	// in searched instances search the one we want

	std::set<CoreModifiable*>::const_iterator itinst;
	for (itinst = searchinstances.begin(); itinst != searchinstances.end(); ++itinst)
	{
		if ((*itinst)->getUID() == uid)
		{
			uptr localpointer = (uptr)(*itinst);
			if (localpointer == pointer)
			{
				instances.insert((CoreModifiable*)*itinst);
				break;
			}
			else if(getref)
			{
				(*itinst)->Destroy(); // release previously set ref
			}
		}
	}

	if (getref)
	{
		++itinst;
		for (; itinst != searchinstances.end(); ++itinst)
		{
			(*itinst)->Destroy(); // release previously set ref
		}
	}
}

//! utility method for inheritance maganement
const std::vector<CoreClassNameTree::TwoNames>& RefCountedClass::getClassNameTree() const
{
	assert(false); // Not supported
	// Use GetClassNameTree instead;
	// return myTreeNode->myClassNameTree->classNames();
	return (*(const std::vector<CoreClassNameTree::TwoNames>*)nullptr);
}

void	CoreTreeNode::sortInstanceVector()
{
	std::sort(myInstances.begin(), myInstances.end(), [](const RefCountedClass* a1, const RefCountedClass* a2)
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
		std::vector<RefCountedClass*>::const_iterator i = myInstances.begin();
		std::vector<RefCountedClass*>::const_iterator e = myInstances.end();

		size_t	currentSize = myInstances.size();

		// little dichotomie to reduce the test set
		while (currentSize > 8)
		{
			currentSize = currentSize / 2;
			std::vector<RefCountedClass*>::const_iterator mid = i;
			std::advance(mid, currentSize);
			if (searchedOne.toUInt() == (*mid)->getNameID().toUInt())
			{
				break;
			}
			else if ((*mid)->getNameID().toUInt()<searchedOne.toUInt())
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


//! static method : return the set of all instances of the given type matching the correct name (use NameComparator)
void RefCountedClass::GetInstancesByName(const KigsID& cid, const std::string &name, std::set<CoreModifiable*>& instances, bool exactTypeOnly, bool only_one, bool getref)
{
	instances.clear();
	CoreTreeNode* node = KigsCore::GetTypeNode(cid); 
	if (node) node->getInstancesByName(instances, !exactTypeOnly, name, only_one, getref);
}

CoreModifiable* RefCountedClass::GetFirstInstanceByName(const KigsID& cid, const std::string &name, bool exactTypeOnly, bool getref)
{
	std::set<CoreModifiable*> insts;
	GetInstancesByName(cid, name, insts, exactTypeOnly, true, getref);
	if (insts.size()) return *insts.begin();
	return nullptr;
}

//! constructor manage inheritance tree
RefCountedClass::RefCountedClass(const std::string& name, CLASS_NAME_TREE_ARG)
	: RefCountedBaseClass()
	, myName(name)
{
	myNameID = myName;
	myUID = myUIDCounter.fetch_add(1);

#ifdef _TRACKREFCOUNT
	char pBuffer[256];
	sprintf(pBuffer, "*** Creating %s with refcount=%d\n", name.c_str(), myRefCounter);
	Log(pBuffer);
#endif

#if 0
	PrintTreeNode(KigsCore::getRootNode());
	std::cout << std::endl;
#endif
}


void RefCountedClass::RegisterToCore()
{
	KigsID cid = GetRuntimeType();
	if (cid == "")
	{
		cid = getExactType();
	}
	CoreTreeNode* type_node = KigsCore::GetTypeNode(cid);
	KIGS_ASSERT(type_node != nullptr);
	type_node->AddInstance(this);
	myTypeNode = type_node;
}

void RefCountedClass::setName(const std::string &name)
{
	if(myTypeNode) myTypeNode->RemoveInstance(this);
	myName = name;
	myNameID = myName;
	if(myTypeNode) myTypeNode->AddInstance(this);
}

void RefCountedClass::ProtectedDestructor()
{
	if (myTypeNode)
	{
		myTypeNode->RemoveInstance(this);
	}

#ifdef _TRACKREFCOUNT
	char pBuffer[256];
	sprintf(pBuffer, "XXX Destroying %s(%s)\n", myName.c_str(), RefCountedBaseClass::myClassName);
	Log(pBuffer);
#endif
}

void CoreTreeNode::getTreeNodes(bool OnlyAppendNodesWithInstances, std::list<const CoreTreeNode *> &nodes) const
{
	if (!OnlyAppendNodesWithInstances || myInstances.size())
		nodes.push_back(this);

	for (auto j : myChildren)
	{
		j.second->getTreeNodes(OnlyAppendNodesWithInstances, nodes);
	}

}

/*
void CoreTreeNode::getInstances(std::vector<CMSP>& result, bool recursive, bool only_one = false) const
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

void CoreTreeNode::getInstancesByName(std::vector<CMSP>& result, bool recursive, const std::string& name, bool only_one = false)
{
	std::lock_guard<std::recursive_mutex> lk{ myMutex };
	if (myInstances.size())
	{
		KigsID searchedOne = name;
		if (myInstanceVectorNeedSort)
		{
			sortInstanceVector();
		}
		std::vector<RefCountedClass*>::const_iterator i = myInstances.begin();
		std::vector<RefCountedClass*>::const_iterator e = myInstances.end();

		size_t	currentSize = myInstances.size();

		// little dichotomie to reduce the test set
		while (currentSize > 8)
		{
			currentSize = currentSize / 2;
			std::vector<RefCountedClass*>::const_iterator mid = i;
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

std::vector<CMSP> RefCountedClass::FindInstances(const KigsID& id, bool exact_type_only)
{
	std::vector<CMSP> result;
	CoreTreeNode* node = KigsCore::GetTypeNode(id);
	if (node) node->getInstances(result, !exact_type_only);
	return result;
}

std::vector<CMSP> RefCountedClass::FindInstancesByName(const KigsID& id, const std::string& name, bool exact_type_only)
{
	std::vector<CMSP> result;
	CoreTreeNode* node = KigsCore::GetTypeNode(id);
	if (node) node->getInstancesByName(result, !exact_type_only, name);
	return result;
}

CMSP RefCountedClass::FindFirstInstance(const KigsID& id, const std::string& name, bool exact_type_only)
{
	std::vector<CMSP> result;
	CoreTreeNode* node = KigsCore::GetTypeNode(id);
	if (node) node->getInstances(result, !exact_type_only, true);
	if (result.size()) return result.front();
	return nullptr;
}

CMSP RefCountedClass::FindFirstInstanceByName(const KigsID& id, const std::string& name, bool exact_type_only)
{
	std::vector<CMSP> result;
	CoreTreeNode* node = KigsCore::GetTypeNode(id);
	if (node) node->getInstancesByName(result, !exact_type_only, name, true);
	if (result.size()) return result.front();
	return nullptr;
}*/