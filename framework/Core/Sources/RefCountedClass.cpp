#include "PrecompiledHeaders.h"

#include "RefCountedClass.h"

#include "Core.h"
#include <iterator>

std::atomic<unsigned int> RefCountedClass::myUIDCounter{ 0 };
unsigned int RefCountedClass::myDefaultNameCounter = 0;

//! create a default name when no name is given
kstl::string RefCountedClass::DefaultName()
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

void	CoreTreeNode::AddInstance(RefCountedClass* toAdd)
{
	std::lock_guard<std::recursive_mutex> lk{ myMutex };
	myInstances.push_back(toAdd);
	myInstanceVectorNeedSort = true;
}

void	CoreTreeNode::RemoveInstance(RefCountedClass* toRemove)
{
	std::lock_guard<std::recursive_mutex> lk{ myMutex };

	kstl::vector< RefCountedClass* >::iterator	i;
	kstl::vector< RefCountedClass* >::iterator	e= myInstances.end();

	for ( i =myInstances.begin();i!=e;++i)
	{
		if ((*i) == toRemove)
		{
			myInstances.erase(i);
			break;
		}
	}
}

void CoreTreeNode::getInstances(kstl::set<CoreModifiable*>& instances, bool recursive, bool only_one,bool getref) const
{
	std::lock_guard<std::recursive_mutex> lk{ myMutex };
	for (auto i : myInstances)
	{
		if(getref) i->GetRef();
		instances.insert(static_cast<CoreModifiable*>(i));
		if (only_one) return;
	}
	if (recursive)
	{
		for (auto it : myChildren)
		{
			if (it.second)
			{
				it.second->getInstances(instances, true, only_one,getref);
				if (only_one&&instances.size())
					break;
			}

		}
	}
}


void CoreTreeNode::getRootInstances(kstl::set<CoreModifiable*>& instances, bool recursive, bool getref) const
{
	std::lock_guard<std::recursive_mutex> lk{ myMutex };
	for (auto i : myInstances)
	{
		CoreModifiable* testFather = ((CoreModifiable*)i);
		if (testFather->GetParentCount() == 0)
		{
			if(getref) (i)->GetRef();
			instances.insert((CoreModifiable*)i);
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

//! utility method for inheritance maganement
/*
CoreClassNameTree* RefCountedClass::EnrichClassNameTree(CoreClassNameTree* classNameTree, KigsID classID, KigsID runtimetype)
{
	KigsCore::Instance()->GetSemaphore();
	if (classID == 0u)
	{
		KigsCore::Instance()->ReleaseSemaphore();
		return classNameTree;
	}
	if (!classNameTree)
	{
		CoreClassNameTree* newone = new CoreClassNameTree(classID, runtimetype);
		KigsCore::Instance()->ReleaseSemaphore();
		return newone;
	}
	classNameTree->addClassName(classID, runtimetype);

	KigsCore::Instance()->ReleaseSemaphore();

	return classNameTree;
}
*/

//! static method : return the set of all instances of the given type
void RefCountedClass::GetInstances(const KigsID& cid, kstl::set<CoreModifiable*>& instances, bool exactTypeOnly, bool only_one, bool getref)
{
	instances.clear();
	CoreTreeNode* node = KigsCore::GetTypeNode(cid);// GetTypeNode(KigsCore::GetRootNode(), cid);
	if (node) node->getInstances(instances, !exactTypeOnly, only_one,getref);
}

CoreModifiable* RefCountedClass::GetFirstInstance(const KigsID& cid, bool exactTypeOnly, bool getref)
{
	kstl::set<CoreModifiable*> insts;
	GetInstances(cid, insts, exactTypeOnly, true, getref);
	if (insts.size()) return *insts.begin();
	return nullptr;
}

//! static method : return the set of all root instances of the given type 
void RefCountedClass::GetRootInstances(const KigsID& cid, kstl::set<CoreModifiable*>& instances, bool exactTypeOnly, bool getref)
{
	instances.clear();
	CoreTreeNode* node = KigsCore::GetTypeNode(cid);// GetTypeNode(KigsCore::GetRootNode(), cid);
	if (node) node->getRootInstances(instances, !exactTypeOnly, getref);
}

// runtime ID is "name:type:pointer:uid"
void RefCountedClass::GetInstanceByRuntimeID(const kstl::string &runtimeID, kstl::set<CoreModifiable*>& instances, bool getref)
{
	instances.clear();

	kstl::string cid = "";
	kstl::string name = "";
	kstl::string searchstring = runtimeID;

	uptr pointer;
	unsigned int  uid;

	size_t lastpos = 0;
	int i;
	for (i = 0; i < 2; i++)
	{
		size_t result = runtimeID.find(':', lastpos);
		if (result == kstl::string::npos)
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

	kstl::set<CoreModifiable*> searchinstances;
	// retreive name and type
	GetInstancesByName(cid, name, searchinstances,false,false,getref);

	// in searched instances search the one we want

	kstl::set<CoreModifiable*>::const_iterator itinst;
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
const kstl::vector<CoreClassNameTree::TwoNames>& RefCountedClass::getClassNameTree() const
{
	assert(false); // Not supported
	// Use GetClassNameTree instead;
	// return myTreeNode->myClassNameTree->classNames();
	return (*(const kstl::vector<CoreClassNameTree::TwoNames>*)nullptr);
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

void CoreTreeNode::getInstancesByName(kstl::set<CoreModifiable*>& instances, bool recursive, const kstl::string& name, bool only_one, bool getref)
{
	std::lock_guard<std::recursive_mutex> lk{ myMutex };
	if (myInstances.size())
	{

		KigsID searchedOne = name;
		if (myInstanceVectorNeedSort)
		{
			sortInstanceVector();
		}
		kstl::vector<RefCountedClass*>::const_iterator i = myInstances.begin();
		kstl::vector<RefCountedClass*>::const_iterator e = myInstances.end();

		size_t	currentSize = myInstances.size();

		// little dichotomie to reduce the test set
		while (currentSize > 8)
		{
			currentSize = currentSize / 2;
			kstl::vector<RefCountedClass*>::const_iterator mid = i;
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
					if (getref) (*i)->GetRef();
					instances.insert((CoreModifiable*)*i);
					if (only_one) return;
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
void RefCountedClass::GetInstancesByName(const KigsID& cid, const kstl::string &name, kstl::set<CoreModifiable*>& instances, bool exactTypeOnly, bool only_one, bool getref)
{
	instances.clear();
	CoreTreeNode* node = KigsCore::GetTypeNode(cid); 
	if (node) node->getInstancesByName(instances, !exactTypeOnly, name, only_one, getref);
}

CoreModifiable* RefCountedClass::GetFirstInstanceByName(const KigsID& cid, const kstl::string &name, bool exactTypeOnly, bool getref)
{
	kstl::set<CoreModifiable*> insts;
	GetInstancesByName(cid, name, insts, exactTypeOnly, true, getref);
	if (insts.size()) return *insts.begin();
	return nullptr;
}

//! constructor manage inheritance tree
RefCountedClass::RefCountedClass(const kstl::string& name, CLASS_NAME_TREE_ARG)
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

void RefCountedClass::setName(const kstl::string &name)
{
	myTypeNode->RemoveInstance(this);
	myName = name;
	myNameID = myName;
	myTypeNode->AddInstance(this);
}

//! destructor

void RefCountedClass::ProtectedDestructor()
{
	KigsCore::Instance()->GetSemaphore();

	if (myTypeNode)
	{
		myTypeNode->RemoveInstance(this);
	}

#ifdef _TRACKREFCOUNT
	char pBuffer[256];
	sprintf(pBuffer, "XXX Destroying %s(%s)\n", myName.c_str(), RefCountedBaseClass::myClassName);
	Log(pBuffer);
#endif

	KigsCore::Instance()->ReleaseSemaphore();
}

void	RefCountedClass::Destroy() 
{
	if (myTypeNode)
	{
		std::lock_guard<std::recursive_mutex> lk{ myTypeNode->myMutex }; // lock corresponding CoreTreeNode mutex to avoid instance search founding an instance already in destroy process
		RefCountedBaseClass::Destroy();	// can't put this outside of the if as it have to be locked
	}
	else
	{
		RefCountedBaseClass::Destroy(); // no locked call to destroy
	}
}

//
void CoreTreeNode::getTreeNodes(bool OnlyAppendNodesWithInstances, kstl::list<const CoreTreeNode *> &nodes) const
{
	if (!OnlyAppendNodesWithInstances || myInstances.size())
		nodes.push_back((const CoreTreeNode *)this);

	for (auto j : myChildren)
	{
		j.second->getTreeNodes(OnlyAppendNodesWithInstances, nodes);
	}

}
