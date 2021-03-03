#include "PrecompiledHeaders.h"

#include "CoreModifiable.h"

#include "CoreTreeNode.h"

#include "NotificationCenter.h"
#include "CoreBaseApplication.h"

#include "maCoreItem.h"
#include "maReference.h"
#include "maUSString.h"
#include "maBuffer.h"

#include "ModuleFileManager.h"
#include "XMLIncludes.h"
#include "XMLReaderFile.h"
#include "XMLWriterFile.h"
#include "XMLNode.h"
#include "XMLAttribute.h"
#include "CoreModifiableTemplateImport.h"

#include "AttributePacking.h"
#include "AttributeModifier.h"
#include "JSonFileParser.h"

#include "Upgrador.h"

#include <algorithm>

#include <stdio.h>

#ifdef _KIGS_ONLY_STATIC_LIB_
#include "Platform/Core/PlatformCore.h"
#endif

#define XML_MULTIPLE_EXPORT_ROOT "Root_node"

#include "CorePackage.h"

#ifdef KIGS_TOOLS
#define TRACEREF_RETAIN  kigsprintf("+++ REF ON %p (%s) (%03d>%03d) \n",this, getExactTypeID()._id_name.c_str(), (int)mRefCounter-1, (int)mRefCounter);
#define TRACEREF_RELEASE kigsprintf("--- REF ON %p (%s) (%03d>%03d) \n",this, getExactTypeID()._id_name.c_str(), (int)mRefCounter, (int)mRefCounter-1);
#define TRACEREF_DELETE  kigsprintf("### REF ON %p (%s)\n",this, getExactTypeID()._id_name.c_str());
#else
#define TRACEREF_RETAIN
#define TRACEREF_RELEASE
#define TRACEREF_DELETE
#endif

std::atomic<unsigned int> CoreModifiable::mUIDCounter{ 0 };

//! auto implement static members
IMPLEMENT_CLASS_INFO(CoreModifiable);

CoreModifiable::~CoreModifiable()
{
	if (isFlagAsNotificationCenterRegistered())
	{
		unregisterFromNotificationCenter();
	}

	if (isFlagAsReferenceRegistered())
	{
		unregisterFromReferenceMap();
	}

	if (isFlagAsAutoUpdateRegistered())
	{
		unregisterFromAutoUpdate();
	}

	// delete dynamics
	DeleteDynamicAttributes();

	mItems.clear();
	//_itemsLinkType.clear();

	if (mLazyContent)
		delete mLazyContent;
	
	mAttributes.clear();
	mUsers.clear();



#ifdef KEEP_XML_DOCUMENT
	DeleteXMLFiles();
#endif
}

LazyContent* CoreModifiable::GetLazyContent() const
{
	if (mLazyContent) return mLazyContent;
	std::lock_guard<std::recursive_mutex> lk{ GetMutex() };
	if (mLazyContent) return mLazyContent;
	mLazyContent = new LazyContent;
	return mLazyContent;
}

void CoreModifiable::RegisterToCore()
{
	KigsID cid = GetRuntimeType();
	if (cid == "")
	{
		cid = getExactType();
	}
	CoreTreeNode* type_node = KigsCore::GetTypeNode(cid);
	KIGS_ASSERT(type_node != nullptr);
	type_node->AddInstance(this);
	mTypeNode = type_node;
}

void CoreModifiable::setName(const std::string& name)
{
	if (mTypeNode) mTypeNode->RemoveInstance(this);
	mName = name;
	mNameID = mName;
	if (mTypeNode) mTypeNode->AddInstance(this);
}

CoreModifiable*	CoreModifiable::getAggregateByType(KigsID id)
{
	if (this->isSubType(id))
	{
		return this;
	}
	if (isFlagAsAggregateParent())
	{	
		std::vector<ModifiableItemStruct>::const_iterator	itc = mItems.begin();
		std::vector<ModifiableItemStruct>::const_iterator	ite = mItems.end();

		while (itc != ite)
		{
			if ((*itc).isAggregate()) // if aggregate is same type as asked
			{
				if ((*itc).mItem->isSubType(id))
				{
					return (CoreModifiable*)(*itc).mItem.get();
				}
				CoreModifiable* found = (*itc).mItem->getAggregateByType(id);
				if (found)
				{
					return found;
				}
			}
			++itc;
		}
	}
	return 0;
}

CoreModifiable*	CoreModifiable::getAggregateRoot() const
{
	const CoreModifiable*  found = this;
	const CoreModifiable*  returnedVal = this;

	while (found)
	{
		found = 0;
		if (returnedVal->isFlagAsAggregateSon()) // if not flag as aggregate son, don't go to parent
		{

			std::vector<CoreModifiable*>::const_iterator itpc = returnedVal->mUsers.begin();
			std::vector<CoreModifiable*>::const_iterator itpe = returnedVal->mUsers.end();

			while (itpc != itpe)
			{
				if ((*itpc)->isFlagAsAggregateParent()) // this parent is an aggregate root
				{
					CoreModifiable* currentAggregate = (*itpc);

					// just check that returnedVal is the searched aggregate
					std::vector<ModifiableItemStruct>::const_iterator	itc = currentAggregate->mItems.begin();
					std::vector<ModifiableItemStruct>::const_iterator	ite = currentAggregate->mItems.end();

					while (itc != ite)
					{
						if ((*itc).isAggregate()) // if aggregate is same type as asked
						{
							if ((*itc).mItem == returnedVal)
							{
								found = currentAggregate;
								returnedVal = currentAggregate;
								break;
							}
						}
						++itc;
					}

					if (found)
					{
						break;
					}
				}
				++itpc;
			}
		}
	}
	return (CoreModifiable*)returnedVal;
}

const kigs::unordered_map<KigsID, ModifiableMethodStruct>* CoreModifiable::GetMethods() 
{ 
	if (!mLazyContent) return nullptr; return &GetLazyContent()->mMethods; 
}

bool CoreModifiable::HasMethod(const KigsID& methodNameID) const
{
	const CoreModifiable* lthis;
	if (findMethod(methodNameID, lthis) != 0)
	{
		return true;
	}
	return false;
}

void CoreModifiable::InsertMethod(KigsID labelID, CoreModifiable::ModifiableMethod method, const std::string& methodName CONNECT_PARAM)
{
	ModifiableMethodStruct toAdd(methodName, method);
#ifdef KIGS_TOOLS
	toAdd.xmlattr = xmlattr;
#endif
	GetLazyContent()->mMethods.insert({ labelID, toAdd });
}

void CoreModifiable::InsertUpgradeMethod(KigsID labelID, CoreModifiable::ModifiableMethod method, UpgradorBase* up)
{
	ModifiableMethodStruct toAdd("", method, up);
	GetLazyContent()->mMethods.insert({ labelID, toAdd });
}

void CoreModifiable::RemoveMethod(KigsID labelID)
{
	if (!mLazyContent) return;
	auto& methods = GetLazyContent()->mMethods;
	auto foundmethod = methods.find(labelID);
	if (foundmethod != methods.end())
	{
		methods.erase(foundmethod);
	}
}

void CoreModifiable::debugPrintfClassList(const std::string& className, s32 maxindent, bool onlyOrphan)
{
	std::vector<CMSP> instances=CoreModifiable::GetInstances(className);
	
	std::vector<CMSP>::const_iterator it=instances.begin();
	std::vector<CMSP>::const_iterator end=instances.end();
	for(; it!=end; ++it)
		if(onlyOrphan)
	{
		if((*it)->GetParents().size()==0)
			(*it)->debugPrintfTree(maxindent);
	}
	else
		(*it)->debugPrintfTree(maxindent);
	
	kigsprintf("%i items\n", (s32)instances.size());
}

void CoreModifiable::Upgrade(UpgradorBase* toAdd)
{
	LazyContent* c = GetLazyContent();

	toAdd->mNextItem = c->mLinkedListItem;
	c->mLinkedListItem = LazyContentLinkedListItemStruct::FromAddressAndType(toAdd, LazyContentLinkedListItemStruct::ItemType::UpgradorType);
	toAdd->UpgradeInstance(this);
}

UpgradorBase* CoreModifiable::GetUpgrador(const KigsID& ID)
{
	if (mLazyContent)
	{ 
		UpgradorBase* current= (UpgradorBase*)mLazyContent->GetLinkedListItem(LazyContentLinkedListItemStruct::ItemType::UpgradorType);
		while (current)
		{
			if( (current->getID() == ID) || (ID == ""))
			{
				return current;
			}
			current = (UpgradorBase*)current->getNext(LazyContentLinkedListItemStruct::ItemType::UpgradorType);
		}
	}
	return nullptr;
}

void CoreModifiable::debugPrintfFullTree(s32 maxindent)
{
	CoreModifiable::debugPrintfClassList("CoreModifiable", maxindent, true);
}

void CoreModifiable::debugPrintfTree(s32 indent, s32 maxindent)
{
	// kigsprintf indent
	for (s32 i = 0; i < indent; i++)
		kigsprintf("|\t");
	
#ifdef KEEP_NAME_AS_STRING
	kigsprintf("%s : %s(%p) %d refs %s items:%d\n", getExactType().c_str(), getName().c_str(), this, (int)mRefCounter, IsInit() ? "" : "Not init!!", (s32)getItems().size());
#else
	kigsprintf("%d : %s(%p) %d refs %s items:%d\n", (u32)getExactType().toUInt(), getName().c_str(), this, (int)mRefCounter, IsInit()?"": "Not init!!", (s32)getItems().size());
#endif
	
	if(indent+1>maxindent)
		return;
	
	std::vector<ModifiableItemStruct>::iterator it=mItems.begin();
	std::vector<ModifiableItemStruct>::iterator end=mItems.end();
	for(; it!=end; ++it)
	{
		((*it).mItem)->debugPrintfTree(indent+1, maxindent);
	}
}

void CoreModifiable::Init() 
{ 
	EmitSignal(Signals::PreInit, this);
	InitModifiable(); 
	EmitSignal(Signals::PostInit, this);
}

//! default modifiable init : set all initparams to readonly and set initflag to true
void CoreModifiable::InitModifiable()
{
	//! already done ? then return
	if(_isInit)
	{
		return;
	}
	
#ifdef KIGS_TOOLS
	if (getAttribute("TRACE"))
		mTraceRef = true;
	
#endif
	
	
	//	std::map<std::string,CoreModifiableAttribute*>::iterator it;
	auto it = mAttributes.begin();
	if(!getAttribute("NoRecursiveUpdateFromParent"))
		mModifiableFlag |= (u32)RecursiveUpdateFlag;
	
	for(;it!=mAttributes.end();++it)
	{
		CoreModifiableAttribute* attribute=(*it).second;
		
		if(attribute->isInitParam())
		{
			attribute->setReadOnly(true);
		}
	}
	
	mModifiableFlag|=(u32)InitFlag;
}

//! called when InitModifiable has failled : reset read/write flag on all init params
void CoreModifiable::UninitModifiable()
{
	//! not init ? return
	if(!_isInit)
	{
		return;
	}
	
	EmitSignal(Signals::Uninit, this);
	
	auto it = mAttributes.begin();
	for(;it!=mAttributes.end();++it)
	{
		CoreModifiableAttribute* attribute=(*it).second;
		
		if(attribute->isInitParam())
		{
			attribute->setReadOnly(false);
		}
	}
	
	mModifiableFlag&=0xFFFFFFFF^(u32)InitFlag;
}

void CoreModifiable::RecursiveInit(bool a_childInFirst)
{
	if (a_childInFirst)
	{
		std::vector<ModifiableItemStruct> L_Child = getItems();
		
		if (!L_Child.empty())
		{
			std::vector<ModifiableItemStruct>::iterator L_ItStart = L_Child.begin();
			std::vector<ModifiableItemStruct>::iterator L_ItEnd = L_Child.end();
			
			while (L_ItStart != L_ItEnd)
			{
				(*L_ItStart).mItem->RecursiveInit(a_childInFirst);
				L_ItStart++;
			}
		}
		EmitSignal(Signals::PreInit, this);
		InitModifiable();
		EmitSignal(Signals::PostInit, this);
	}
	else
	{
		EmitSignal(Signals::PreInit, this);
		InitModifiable();
		EmitSignal(Signals::PostInit, this);
		
		std::vector<ModifiableItemStruct> L_Child = getItems();
		if (!L_Child.empty())
		{
			std::vector<ModifiableItemStruct>::iterator L_ItStart = L_Child.begin();
			std::vector<ModifiableItemStruct>::iterator L_ItEnd = L_Child.end();
			
			while (L_ItStart != L_ItEnd)
			{
				(*L_ItStart).mItem->RecursiveInit(a_childInFirst);
				L_ItStart++;
			}
		}
	}
	
}
const ModifiableMethodStruct* CoreModifiable::findMethodOnThisOnly(const KigsID& id, const CoreModifiable*& localthis) const
{
	
	// first check on this
	if (mLazyContent)
	{
		auto& methods = GetLazyContent()->mMethods;
		auto it = methods.find(id);
		if (it != methods.end())
		{
			localthis = this;
			return &(*it).second;
		}
	}

	if (mTypeNode)
	{
		auto it = mTypeNode->mMethods.find(id);
		if (it != mTypeNode->mMethods.end())
		{
			localthis = this;
			return &it->second;
		}
	}
	// if not found, check on forward ptr
	if (mLazyContent)
	{
		StructLinkedListBase* found = mLazyContent->GetLinkedListItem(LazyContentLinkedListItemStruct::ItemType::ForwardSmartPtrType);
		while (found)
		{
			ForwardSP<CoreModifiable> f = *(static_cast<ForwardSP<CoreModifiable>*>(found));
			const ModifiableMethodStruct* search = f->findMethodOnThisOnly(id, localthis);
			if (search)
			{
				return search;
			}
			found = found->getNext(LazyContentLinkedListItemStruct::ItemType::ForwardSmartPtrType);
		}
	}
	return nullptr;
}
const ModifiableMethodStruct* CoreModifiable::findMethod(const KigsID& id, const CoreModifiable*& localthis) const
{
	const ModifiableMethodStruct* found = findMethodOnThisOnly(id, localthis);
	if (found)
	{
		return found;
	}

	const CoreModifiable* search = this;
	// if not found, search aggregate
	if (isFlagAsAggregateSon())
	{
		search = getAggregateRoot();
	}
	return search->recursivefindMethod(id, localthis);
}

const ModifiableMethodStruct* CoreModifiable::recursivefindMethod(const KigsID& id, const CoreModifiable*& localthis) const
{
	const ModifiableMethodStruct* foundonthis = findMethodOnThisOnly(id, localthis);

	if (foundonthis)
	{
		return foundonthis;
	}

	if (isFlagAsAggregateParent()) // search if other aggregates
	{
		std::vector<ModifiableItemStruct>::const_iterator	itc = mItems.begin();
		std::vector<ModifiableItemStruct>::const_iterator	ite = mItems.end();

		while (itc != ite)
		{
			if ((*itc).isAggregate())
			{
				auto found = (*itc).mItem->recursivefindMethod(id, localthis);
				if (found != 0)
				{
					return found;
				}
			}
			++itc;
		}
	}
	return nullptr;
}

bool CoreModifiable::aggregateWith(CMSP& item, ItemPosition pos)
{
	if (addItem(item, pos PASS_LINK_NAME(linkName)))
	{
		if (pos == First)
		{
			mItems[0].setAggregate();
			mItems[0].mItem->flagAsAggregateSon();
		}
		else
		{
			mItems[mItems.size() - 1].setAggregate();
			mItems[mItems.size() - 1].mItem->flagAsAggregateSon();
		}

		flagAsAggregateParent();

		return true;
	}
	return false;
}

bool CoreModifiable::removeAggregateWith(CMSP& item)
{
	bool itemIsAlive = (item->getRefCount() > 1);

	if (removeItem(item PASS_LINK_NAME(linkName)))
	{

		if (itemIsAlive)
		{
			if (!item->checkIfAggregateSon()) // item is no more an aggregate son ?
			{
				item->unflagAsAggregateSon();
			}
		}
		if (isFlagAsAggregateParent()) // search if other aggregates
		{
			std::vector<ModifiableItemStruct>::const_iterator	itc = mItems.begin();
			std::vector<ModifiableItemStruct>::const_iterator	ite = mItems.end();

			while (itc != ite)
			{
				if ((*itc).isAggregate()) // remaining aggregates, return true 
				{
					return true;
				}
				++itc;
			}

			// no remaining aggregates

			unflagAsAggregateParent();
		}
		return true;
	}
	return false;
}

void CoreModifiable::UpdateAggregates(const Timer&  timer, void* addParam)
{
	std::vector<ModifiableItemStruct>::const_iterator	itc = mItems.begin();
	std::vector<ModifiableItemStruct>::const_iterator	ite = mItems.end();

	while (itc != ite)
	{
		if ((*itc).isAggregate())
		{
			(*itc).mItem->CallUpdate(timer, addParam);
		}
		++itc;
	}
}

CoreModifiableAttribute* CoreModifiable::findAttributeOnThisOnly(const KigsID& id) const
{
	// first check on this
	auto i = mAttributes.find(id);
	if (i != mAttributes.end())
	{
		return (*i).second;
	}
	// if not found, check on forward ptr
	if (mLazyContent)
	{
		StructLinkedListBase* found = mLazyContent->GetLinkedListItem(LazyContentLinkedListItemStruct::ItemType::ForwardSmartPtrType);
		while (found)
		{
			ForwardSP<CoreModifiable> f = *(static_cast<ForwardSP<CoreModifiable>*>(found));
			if (!f.isNil())
			{
				CoreModifiableAttribute* search = f->findAttributeOnThisOnly(id);
				if (search)
				{
					return search;
				}
			}
			found = found->getNext(LazyContentLinkedListItemStruct::ItemType::ForwardSmartPtrType);
		}
	}
	return nullptr;
}

CoreModifiableAttribute* CoreModifiable::findAttribute(const KigsID& id) const
{
	auto foundOnThis = findAttributeOnThisOnly(id);
	if (foundOnThis)
	{
		return foundOnThis;
	}

	const CoreModifiable* search = this;
	// if not found, check on aggregate
	if (isFlagAsAggregateSon())
	{
		search = getAggregateRoot();
	}
	return search->recursivefindAttribute(id);
}

CoreModifiableAttribute* CoreModifiable::recursivefindAttribute(const KigsID& id) const
{
	auto foundOnThis = findAttributeOnThisOnly(id);
	if (foundOnThis)
	{
		return foundOnThis;
	}

	if (isFlagAsAggregateParent()) // search if other aggregates
	{
		std::vector<ModifiableItemStruct>::const_iterator	itc = mItems.begin();
		std::vector<ModifiableItemStruct>::const_iterator	ite = mItems.end();

		while (itc != ite)
		{
			if ((*itc).isAggregate())
			{
				CoreModifiableAttribute* found = (*itc).mItem->recursivefindAttribute(id);
				if (found)
				{
					return found;
				}
			}
			++itc;
		}
	}
	return 0;
	
}

bool CoreModifiable::CallMethod(KigsID methodNameID,std::vector<CoreModifiableAttribute*>& params,void* privateParams,CoreModifiable* sender)
{
	const CoreModifiable* getlocalthis;
	const ModifiableMethodStruct* methodFound = findMethod(methodNameID, getlocalthis);

	if(methodFound == 0) // method was not found ? try to find maCoreItem with the same name to evaluate
	{
		CoreModifiableAttribute* foundattrib = findAttribute(methodNameID);
		if (foundattrib)
		{
			if (foundattrib->getType() == ATTRIBUTE_TYPE::COREITEM)
			{
				CoreItem& ItemToEval = (*(maCoreItem*)foundattrib);
				
				bool result = ItemToEval;

				return result;

			}
		}
#ifdef KIGS_TOOLS
		if(methodNameID != "SequenceStart" && methodNameID != "SequenceEnd")
			kigsprintf("Trying to call %s but the method doesn't exists\n", methodNameID._id_name.c_str());
#endif
		return false;
	}
	CoreModifiable* localthis=(CoreModifiable*)getlocalthis;
	bool result = false;


	if (!methodFound->IsMethod())
	{
		result = methodFound->GetFunction()(this, sender, params, privateParams);
	}
	else
	{
		auto& method = methodFound->GetMethod();
		if (method.mName != "")
		{
			maString methodName{ "methodName" , method.mName };
			params.insert(params.begin(), &methodName);
			result = (localthis->*method.mMethod)(sender, params, privateParams);
			params.erase(params.begin());
		}
		else
		{
			// cache upgrador 
			LazyContentLinkedListItemStruct cachedUpgrador = 0;
			// set this upgrador at first pos
			if (method.mUpgrador)
			{
				cachedUpgrador = localthis->mLazyContent->mLinkedListItem;
				localthis->mLazyContent->mLinkedListItem = LazyContentLinkedListItemStruct::FromAddressAndType(method.mUpgrador,LazyContentLinkedListItemStruct::ItemType::UpgradorType);
			}
			result = (localthis->*method.mMethod)(sender, params, privateParams);
			// reset cached 
			if (cachedUpgrador)
			{
				localthis->mLazyContent->mLinkedListItem = cachedUpgrador;
			}
		}
		
	}
	return result;
}

// used in import
bool CoreModifiable::SimpleCallWithCoreItemParams(KigsID methodNameID, const CoreItemSP& params)
{
	PackCoreModifiableAttributes	attr(this);

	for (const auto& p : params)
	{
		if (p->GetType() & CoreItem::COREITEM_TYPE::COREVALUE)
		{
			attr.AddAttribute(p->createAttribute(nullptr));
		}
		else if (p->GetType() & CoreItem::COREITEM_TYPE::COREVECTOR)
		{
			switch (p->size())
			{
			case 2:
				attr << (Point2D)p;
				break;
			case 3:
				attr << (Point3D)p;
				break;
			case 4:
				attr << (Vector4D)p;
				break;
			default:
				KIGS_WARNING("bad params for calls from xml", 2);
			}
		}
		else
		{
			KIGS_WARNING("bad params for calls from xml", 2);
		}
	}

	return CallMethod(methodNameID, ((std::vector<CoreModifiableAttribute*>&)(attr)));
}


void CoreModifiable::ManageToCall(CoreModifiable::ImportState::ToCall& c)
{
	JSonFileParser L_JsonParser;
	std::string plist = "[" + c.paramList + "]";
	CoreItemSP L_Dictionary = L_JsonParser.Get_JsonDictionaryFromString(plist);

	if (!L_Dictionary.isNil())
	{
		c.currentNode->SimpleCallWithCoreItemParams(c.methodName, L_Dictionary);
	}
}

bool CoreModifiable::SimpleCall(KigsID methodNameID)
{
	PackCoreModifiableAttributes	attr(this);
	return CallMethod(methodNameID, ((std::vector<CoreModifiableAttribute*>&)(attr)));
}

bool CoreModifiable::EmitSignal(const KigsID& signalID)
{
	std::vector<CoreModifiableAttribute*>	empty;
	return EmitSignal(signalID, empty);
}

bool CoreModifiable::EmitSignal(const KigsID& signalID, std::vector<CoreModifiableAttribute*>& params, void* privateParams)
{
	if (!isFlagAllowChanges()) return false;
	std::unique_lock<std::recursive_mutex> lk{ GetMutex() };
	if (!isFlagAllowChanges()) return false;
	if (!mLazyContent) return false;
	auto& connected_to = GetLazyContent()->mConnectedTo;
	auto it = connected_to.find(signalID);
	if(it != connected_to.end())
	{
		std::vector<std::pair<KigsID, CoreModifiable*>> copy = it->second;
		lk.unlock();
		for(auto p : copy)
		{
			if (!p.second->isFlagAllowChanges()) continue;
			std::unique_lock<std::recursive_mutex> lk{ p.second->GetMutex() };
			if (!p.second->isFlagAllowChanges()) continue;
			p.second->CallMethod(p.first, params, privateParams);
		}
		return true;
	}
	return false;
}

void CoreModifiable::Connect(KigsID signal, CoreModifiable* other, KigsID slot CONNECT_PARAM)
{
	if(!isFlagAllowChanges() || !other->isFlagAllowChanges())
		return;

	std::unique_lock<std::recursive_mutex> lock_this{ GetMutex(), std::defer_lock};
	std::unique_lock<std::recursive_mutex> lock_other{other->GetMutex(), std::defer_lock};
	std::lock(lock_this, lock_other);

	if (!isFlagAllowChanges() || !other->isFlagAllowChanges())
		return;
	
	auto& vec = GetLazyContent()->mConnectedTo[signal];
	for(auto p : vec)
	{
		if(p.first == slot && p.second == other)
		return;
	}
	vec.push_back({slot, other});
	other->GetLazyContent()->mConnectedToMe[this].insert(std::make_pair(signal, slot));
}

void CoreModifiable::Disconnect(KigsID signal, CoreModifiable* other, KigsID slot)
{
	if (!isFlagAllowChanges() || !other->isFlagAllowChanges()) return;
	std::unique_lock<std::recursive_mutex> lock_this{ GetMutex(), std::defer_lock};
	std::unique_lock<std::recursive_mutex> lock_other{other->GetMutex(), std::defer_lock};
	std::lock(lock_this, lock_other);
	if (!isFlagAllowChanges() || !other->isFlagAllowChanges()) return;

	if (!mLazyContent || !other->mLazyContent) return;

	auto& vec = GetLazyContent()->mConnectedTo[signal];
	for(auto it = vec.begin(); it != vec.end(); ++it)
	{
		if(it->first == slot && it->second == other)
		{
			vec.erase(it);
			other->GetLazyContent()->mConnectedToMe[this].erase(std::make_pair(signal, slot));
			return;
		}	
	}

}

std::string	CoreModifiable::GetRuntimeID() const
{
	std::string result="";

	char buffer[512];
	
#ifdef KEEP_NAME_AS_STRING
	snprintf(buffer, 512, "%s:%s:%p:%u",getName().c_str(), GetRuntimeType().c_str(), this, mUID);
#else
	snprintf(buffer, 512, "%s:%u:%p:%u", getName().c_str(), GetRuntimeType()._id, this, mUID);
#endif
	
	result=buffer;
	return result;
}

CoreModifiable::operator std::vector<CoreModifiableAttribute*> ()
{
	std::vector<CoreModifiableAttribute*> _attributeList;
	for(auto it = mAttributes.begin();it!=mAttributes.end();++it)
	{
		_attributeList.push_back((*it).second);
	}
	return _attributeList;
}

//! return element count for an array type attribute
u32 CoreModifiable::getNbArrayElements(KigsID attributeID) const
{
	CoreModifiableAttribute* attribute = findAttribute(attributeID);
	if (attribute) return attribute->getNbArrayElements();
	return 0;
}

//! macro fast implementing access to an attribute
#define IMPLEMENT_ACCESS_VALUE_BODY_BY_ID(accessValue) \
{ \
	CoreModifiableAttribute* attribute=findAttribute(attributeID);  \
	if(attribute) return attribute->accessValue(value); \
	return false; \
}

//! implement a get
#define IMPLEMENT_GET_VALUE(T) bool CoreModifiable::getValue(const KigsID attributeID, T value) const \
IMPLEMENT_ACCESS_VALUE_BODY_BY_ID(getValue)

//! implement a set
#define IMPLEMENT_SET_VALUE(T) bool CoreModifiable::setValue(KigsID attributeID, T value) \
IMPLEMENT_ACCESS_VALUE_BODY_BY_ID(setValue)


//! implement get/set methods for all types
EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, IMPLEMENT_GET_VALUE);
EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, IMPLEMENT_SET_VALUE);

EXPAND_MACRO_FOR_STRING_TYPES(NOQUALIFIER, &, IMPLEMENT_GET_VALUE);
EXPAND_MACRO_FOR_STRING_TYPES(const, &, IMPLEMENT_SET_VALUE);

IMPLEMENT_SET_VALUE(const char*);
IMPLEMENT_SET_VALUE(const u16*);
IMPLEMENT_SET_VALUE(const UTF8Char*);

//IMPLEMENT_GET_VALUE(CoreModifiable*&); 
//IMPLEMENT_SET_VALUE(CoreModifiable*);

EXPAND_MACRO_FOR_EXTRA_TYPES(NOQUALIFIER, &, IMPLEMENT_GET_VALUE);
EXPAND_MACRO_FOR_EXTRA_TYPES(NOQUALIFIER, NOQUALIFIER, IMPLEMENT_SET_VALUE);



////////////////////////////////////////////////////////////////////////////

//! macro implementing body of get/set values for array
#define IMPLEMENT_ACCESS_ARRAY_VALUE_BODY_BY_ID(accessValue) \
{ \
	CoreModifiableAttribute* attribute=findAttribute(attributeID); \
	if(attribute) return attribute->accessValue(value, nbElements); \
	return false; \
}

//! macro implementing body of get/set values for array by element
#define IMPLEMENT_ACCESS_ARRAY_ELEMENT_VALUE_BODY_BY_ID(accessValue) \
{ \
	CoreModifiableAttribute* attribute=findAttribute(attributeID); \
	if(attribute) return attribute->accessValue(value, line, column); \
	return false; \
}

//! macro implementing get for array

#define IMPLEMENT_GET_ARRAY_VALUE(T) bool CoreModifiable::getArrayValue(KigsID attributeID, T value, u32 nbElements) const \
IMPLEMENT_ACCESS_ARRAY_VALUE_BODY_BY_ID(getArrayValue)

#define IMPLEMENT_GET_ARRAY_ELEMENT(T) bool CoreModifiable::getArrayElementValue(KigsID attributeID, T value, u32 line, u32 column) const \
IMPLEMENT_ACCESS_ARRAY_ELEMENT_VALUE_BODY_BY_ID(getArrayElementValue)

//! macro implementing set for array
#define IMPLEMENT_SET_ARRAY_VALUE(T) bool CoreModifiable::setArrayValue(KigsID attributeID, T value, u32 nbElements) \
IMPLEMENT_ACCESS_ARRAY_VALUE_BODY_BY_ID(setArrayValue) 

#define IMPLEMENT_SET_ARRAY_ELEMENT(T) bool CoreModifiable::setArrayElementValue(KigsID attributeID, T value, u32 line, u32 column) \
IMPLEMENT_ACCESS_ARRAY_ELEMENT_VALUE_BODY_BY_ID(setArrayElementValue)

//! macro implementing all set and get for all array element type

EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, *, IMPLEMENT_GET_ARRAY_VALUE);
EXPAND_MACRO_FOR_BASE_TYPES(const, *, IMPLEMENT_SET_ARRAY_VALUE);

EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, IMPLEMENT_GET_ARRAY_ELEMENT);
EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, IMPLEMENT_SET_ARRAY_ELEMENT);


IMPLEMENT_GET_ARRAY_ELEMENT(std::string&);
IMPLEMENT_SET_ARRAY_ELEMENT(const std::string&);



#define IMPLEMENT_SETARRAY_VALUE2(valuetype) bool CoreModifiable::setArrayValue(KigsID attributeID, valuetype value1,valuetype value2) \
{\
	CoreModifiableAttribute* attribute = findAttribute(attributeID); \
	if (attribute) \
	{ \
		bool result=true; \
		result |= attribute->setArrayElementValue(value1, 0, 0); \
		result |= attribute->setArrayElementValue(value2, 0, 1); \
		return result;\
	} \
	return false; \
}

EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, IMPLEMENT_SETARRAY_VALUE2);

#undef IMPLEMENT_SETARRAY_VALUE2

#define IMPLEMENT_SETARRAY_VALUE3(valuetype) bool CoreModifiable::setArrayValue(KigsID attributeID, valuetype value1,valuetype value2,valuetype value3) \
{\
	CoreModifiableAttribute* attribute = findAttribute(attributeID); \
	if (attribute) \
	{ \
		bool result=true; \
		result |= attribute->setArrayElementValue(value1, 0, 0); \
		result |= attribute->setArrayElementValue(value2, 0, 1); \
		result |= attribute->setArrayElementValue(value3, 0, 2); \
		return result;\
	} \
	return false; \
}
EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, IMPLEMENT_SETARRAY_VALUE3);


#undef IMPLEMENT_SETARRAY_VALUE3

#define IMPLEMENT_SETARRAY_VALUE4(valuetype) bool CoreModifiable::setArrayValue(KigsID attributeID, valuetype value1,valuetype value2,valuetype value3,valuetype value4) \
{\
	CoreModifiableAttribute* attribute = findAttribute(attributeID); \
	if (attribute) \
	{ \
		bool result=true; \
		result |= attribute->setArrayElementValue(value1, 0, 0); \
		result |= attribute->setArrayElementValue(value2, 0, 1); \
		result |= attribute->setArrayElementValue(value3, 0, 2); \
		result |= attribute->setArrayElementValue(value4, 0, 3); \
		return result;\
	} \
	return false; \
}

EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, IMPLEMENT_SETARRAY_VALUE4);

#undef IMPLEMENT_SETARRAY_VALUE4


void CoreModifiable::UpdateAttributes(const CoreModifiable& tocopy)
{
	for (auto it = tocopy.mAttributes.begin(); it != tocopy.mAttributes.end(); ++it)
	{
		auto foundattr = mAttributes.find(it->first);
		if (foundattr != mAttributes.end())
		{
			*foundattr->second = *it->second;
		}
	}
}


void CoreModifiable::unregisterFromNotificationCenter()
{
	KigsCore::GetNotificationCenter()->removeObserver(this,"",0,true);
}

void CoreModifiable::unregisterFromAutoUpdate()
{
	CoreBaseApplication*	currentApp = KigsCore::GetCoreApplication();
	if (currentApp)
	{
		currentApp->RemoveAutoUpdate(this);
	}
}

void CoreModifiable::unregisterFromReferenceMap()
{
	// check if this coremodifiable is referenced
	
	auto& coremodigiablemap = KigsCore::Instance()->getReferenceMap();
	auto found = coremodigiablemap.find(this);
	
	// ok, the CoreModifiable is here
	if (found != coremodigiablemap.end())
	{
		std::vector<CoreModifiableAttribute*>& referencevector = (*found).second;
		
		std::vector<CoreModifiableAttribute*>::iterator	itcurrent = referencevector.begin();
		std::vector<CoreModifiableAttribute*>::iterator	itend = referencevector.end();
		
		while (itcurrent != itend)
		{
			// clear each reference
			
			maReference* currentRef = (maReference*)(*itcurrent);
			currentRef->ResetFoundModifiable();
			
			++itcurrent;
		}
		
		coremodigiablemap.erase(found);
	}
	
}


void	CoreModifiable::DeleteDynamicAttributes()
{
	std::vector<CoreModifiableAttribute*> todelete;
	for (auto it : mAttributes) 
	{
		if (it.second->isDynamic())
		{
			todelete.push_back(it.second);	
		}
	}
	// CoreModifiableAttribute destructor erases itself from mAttributes
	for (auto attr : todelete)
		delete attr;
}

#ifdef KEEP_XML_DOCUMENT
void CoreModifiable::DeleteXMLFiles()
{
	mXMLFiles.clear();
	mXMLNodes.clear();
}
#endif


//! remove all items (sons)
void		CoreModifiable::EmptyItemList()
{
	while(!mItems.empty())
	{
		ModifiableItemStruct& item=(*mItems.begin());
		{
			removeItem(item.mItem);
		}
	}
}


static s32 getOccurenceCount(const char* str, char tofind)
{
	s32 count = 0;
	const char* read = str;

	while (*read)
	{
		if ((*read) == tofind)
		{
			++count;
		}
		++read;
	}
	return count;
}


CoreModifiableAttribute*	CoreModifiable::AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID, const char* defaultval)
{
	CoreModifiableAttribute*	toadd=0;
	
	
	toadd = getAttribute(ID);
	if (toadd!=nullptr)
	{
		toadd->setValue(defaultval);
		return toadd;
	}
	
	
	if ((type == CoreModifiable::ATTRIBUTE_TYPE::ARRAY) && defaultval)
	{
		// suppose it's a vector
		s32 countComma = getOccurenceCount(defaultval, ',');
		countComma++;
		switch (countComma)
		{
			case 1:
			{
				toadd = new maFloat(*this, false, ID);
			}
			break;
			case 2:
			{
				toadd = new maVect2DF(*this, false, ID);
			}
			break;
			case 3:
			{
				toadd = new maVect3DF(*this, false, ID);
			}
			break;
			case 4:
			{
				toadd = new maVect4DF(*this, false, ID);
			}
			break;
			case 5:
			{
				toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 5>(*this, false, ID);
			}
			break;
			case 6:
			{
				toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 6>(*this, false, ID);
			}
			break;
			case 7:
			{
				toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 7>(*this, false, ID);
			}
			break;
			case 8:
			{
				toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 8>(*this, false, ID);
			}
			break;
			case 9:
			{
				toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 9>(*this, false, ID);
			}
			break;
			case 10:
			{
				toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 10>(*this, false, ID);
			}
			break;
			case 12:
			{
				toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 12>(*this, false, ID);
			}
			break;
			case 16:
			{
				toadd = new maVect16DF(*this, false, ID);
			}
			break;
		}
		
		if (toadd)
			toadd->setDynamic(true);
	}
	else
	{
		toadd = GenericCreateDynamicAttribute(type, ID);
	}
	
	if (toadd && defaultval)
	{
		toadd->setValue(defaultval);
	}
	
	return toadd;
}

CoreModifiableAttribute*	CoreModifiable::AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID, s32 defaultval)
{
	CoreModifiableAttribute*	toadd = GenericCreateDynamicAttribute(type, ID);
	
	if (toadd && defaultval)
	{
		toadd->setValue(defaultval);
	}
	
	return toadd;
}

CoreModifiableAttribute*	CoreModifiable::AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID, u64 defaultval)
{
	CoreModifiableAttribute*	toadd = GenericCreateDynamicAttribute(type, ID);

	if (toadd && defaultval)
	{
		toadd->setValue((u64)defaultval);
	}

	return toadd;
}


CoreModifiableAttribute*	CoreModifiable::AddDynamicVectorAttribute(KigsID ID, const s32* defaultval, u32 valcount)
{
	CoreModifiableAttribute*	toadd = 0;
	
	switch (valcount)
	{
		case 1:
		{
			toadd = new maInt(*this, false, ID);
		}
		break;
		case 2:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::INT, 2>(*this, false, ID);
		}
		break;
		case 3:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::INT, 3>(*this, false, ID);
		}
		break;
		case 4:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::INT, 4>(*this, false, ID);
		}
		break;
		case 5:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::INT, 5>(*this, false, ID);
		}
		break;
		case 6:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::INT, 6>(*this, false, ID);
		}
		break;
		case 7:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::INT, 7>(*this, false, ID);
		}
		break;
		case 8:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::INT, 8>(*this, false, ID);
		}
		break;
		case 9:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::INT, 9>(*this, false, ID);
		}
		break;
		case 10:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::INT, 10>(*this, false, ID);
		}
		break;
		case 12:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::INT, 12>(*this, false, ID);
		}
		break;
		case 16:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::INT, 16>(*this, false, ID);
		}
		break;
	}
	
	if (toadd)
	{
		toadd->setDynamic(true);
		toadd->setArrayValue(defaultval, valcount);
	}
	
	return toadd;
}

CoreModifiableAttribute*	CoreModifiable::AddDynamicVectorAttribute(KigsID ID, const kfloat* defaultval, u32 valcount)
{
	CoreModifiableAttribute*	toadd = 0;
	
	switch (valcount)
	{
		case 1:
		{
			toadd = new maFloat(*this, false, ID);
		}
		break;
		case 2:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 2>(*this, false, ID);
		}
		break;
		case 3:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 3>(*this, false, ID);
		}
		break;
		case 4:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 4>(*this, false, ID);
		}
		break;
		case 5:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 5>(*this, false, ID);
		}
		break;
		case 6:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 6>(*this, false, ID);
		}
		break;
		case 7:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 7>(*this, false, ID);
		}
		break;
		case 8:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 8>(*this, false, ID);
		}
		break;
		case 9:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 9>(*this, false, ID);
		}
		break;
		case 10:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 10>(*this, false, ID);
		}
		break;
		case 12:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 12>(*this, false, ID);
		}
		break;
		case 16:
		{
			toadd = new maVector<kfloat, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 16>(*this, false, ID);
		}
		break;
	}
	
	if (toadd)
	{
		toadd->setDynamic(true);
		toadd->setArrayValue(defaultval, valcount);
	}
	
	return toadd;
}

CoreModifiableAttribute*	CoreModifiable::AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID, kfloat defaultval)
{
	CoreModifiableAttribute*	toadd = GenericCreateDynamicAttribute(type, ID);
	
	if (toadd && defaultval)
	{
		toadd->setValue(defaultval);
	}
	
	return toadd;
}

CoreModifiableAttribute*	CoreModifiable::AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID, bool defaultval)
{
	CoreModifiableAttribute*	toadd = GenericCreateDynamicAttribute(type, ID);
	
	if (toadd && defaultval)
	{
		toadd->setValue(defaultval);
	}
	
	return toadd;
}

CoreModifiableAttribute*	CoreModifiable::GenericCreateDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID)
{
	CoreModifiableAttribute*	toadd = 0;
	switch (type)
	{
		case CoreModifiable::ATTRIBUTE_TYPE::BOOL:
		{
			toadd = new maBool(*this, false, ID);
		}
		break;
		case CoreModifiable::ATTRIBUTE_TYPE::CHAR:
		{
			toadd = new maChar(*this, false, ID);
		}
		break;
		case CoreModifiable::ATTRIBUTE_TYPE::SHORT:
		{
			toadd = new maShort(*this, false, ID);
		}
		break;
		case CoreModifiable::ATTRIBUTE_TYPE::INT:
		{
			toadd = new maInt(*this, false, ID);
		}
		break;
		case CoreModifiable::ATTRIBUTE_TYPE::LONG:
		{
			toadd = new maLong(*this, false, ID);
		}
		break;
		case CoreModifiable::ATTRIBUTE_TYPE::UCHAR:
		{
			toadd = new maUChar(*this, false, ID);
		}
		break;
		case CoreModifiable::ATTRIBUTE_TYPE::USHORT:
		{
			toadd = new maUShort(*this, false, ID);
		}
		break;
		case CoreModifiable::ATTRIBUTE_TYPE::UINT:
		{
			toadd = new maUInt(*this, false, ID);
		}
		break;
		case CoreModifiable::ATTRIBUTE_TYPE::ULONG:
		{
			toadd = new maULong(*this, false, ID);
		}
		break;
		case CoreModifiable::ATTRIBUTE_TYPE::FLOAT:
		{
			toadd = new maFloat(*this, false, ID);
		}
		break;
		case CoreModifiable::ATTRIBUTE_TYPE::DOUBLE:
		{
			toadd = new maDouble(*this, false, ID);
		}
		break;
		case CoreModifiable::ATTRIBUTE_TYPE::STRING:
		{
			toadd = new maString(*this, false, ID);
		}
		break;
		case CoreModifiable::ATTRIBUTE_TYPE::REFERENCE:
		{
			toadd = new maReference(*this, false, ID);
		}
		break;
		case CoreModifiable::ATTRIBUTE_TYPE::COREITEM:
		{
			toadd = new maCoreItem(*this, false, ID);
		}
		break;
		case CoreModifiable::ATTRIBUTE_TYPE::USSTRING:
		{
			toadd = new maUSString(*this, false, ID);
		}
		break;
		case  CoreModifiable::ATTRIBUTE_TYPE::COREBUFFER:
		{
			toadd = new maBuffer(*this, false, ID);
		}
		break;
		case  CoreModifiable::ATTRIBUTE_TYPE::RAWPTR:
		{
			toadd = new maRawPtr(*this, false, ID);
		}
		break;
		case CoreModifiable::ATTRIBUTE_TYPE::ARRAY:
		{
			
			KIGS_ERROR("Can not create a dynamic array with no default value", 1);
		}
		break;
		default:
		break;
	}
	
	if (toadd)
	{
		toadd->setDynamic(true);
	}
	return toadd;
}


void CoreModifiable::RemoveDynamicAttribute(KigsID id)
{
	auto it=mAttributes.find(id);
	if(it != mAttributes.end() && it->second->isDynamic())
	{
		delete ((*it).second);
	}
}

//! add item at first or last position
bool CoreModifiable::addItem(const CMSP& item, ItemPosition pos)
{
	std::unique_lock<std::recursive_mutex> lk{ GetMutex() };
	if(!item.isNil())
	{

		if (pos == First)
		{
			mItems.insert(mItems.begin(), ModifiableItemStruct(item));
		}
		else if (pos == Last)
		{
			mItems.push_back(ModifiableItemStruct(item));
		}
		else // try to insert at given pos
		{
			mItems.insert(mItems.begin()+pos, ModifiableItemStruct(item));
		}

		
		item->addUser(this);
		EmitSignal(Signals::AddItem, this, item);
		return true;
	}
	return false;
}

//! add the given parent to list
void CoreModifiable::addUser(CoreModifiable* user)
{
	std::unique_lock<std::recursive_mutex> lk{ GetMutex() };
	mUsers.push_back(user);
}

//! remove user (parent)
void CoreModifiable::removeUser(CoreModifiable* user)
{
	std::unique_lock<std::recursive_mutex> lk{ GetMutex() };
	bool found=false;
	do
	{
		found=false;
		std::vector<CoreModifiable*>::iterator i=mUsers.begin();
		std::vector<CoreModifiable*>::iterator e=mUsers.end();
		for(; i!=e ; ++i)
		{
			if(user==(*i))
			{
				found=true;
				break;
			}
		}
		if(found)
		{
			mUsers.erase(i);
		}
	} while(found);
}

bool CoreModifiable::checkDestroy()
{
	if (isFlagAsPostDestroy())
	{
		unflagAsPostDestroy(); // remove flag
		KigsCore::Instance()->AddToPostDestroyList(this);
		return true;
	}
#ifdef KIGS_TOOLS
	if (mTraceRef)
		TRACEREF_DELETE
#endif
	ProtectedDestroy();
	return GenericRefCountedBaseClass::checkDestroy();
}

//! Destroy method decrement refcounter and delete instance if no more used
void CoreModifiable::ProtectedDestroy()
{
	if (mTypeNode)
	{
		mTypeNode->RemoveInstance(this);
	}

	EmitSignal(Signals::Destroy, this);

	if (mLazyContent)
	{
		// first downgrade if needed
		UpgradorBase* found = (UpgradorBase * )mLazyContent->GetLinkedListItem(LazyContentLinkedListItemStruct::ItemType::UpgradorType);
		while(found)
		{
			// set this upgrador at first pos
			LazyContentLinkedListItemStruct cachedUpgrador = mLazyContent->mLinkedListItem;
			mLazyContent->mLinkedListItem = LazyContentLinkedListItemStruct::FromAddressAndType(found, LazyContentLinkedListItemStruct::ItemType::UpgradorType);
			found->DowngradeInstance(this);
			mLazyContent->mLinkedListItem = cachedUpgrador;

			UpgradorBase* nextfound = (UpgradorBase * )found->getNext(LazyContentLinkedListItemStruct::ItemType::UpgradorType);
			delete found;
			found = nextfound;
		}
		mLazyContent->mLinkedListItem = 0;
	}

	//! remove all items
	EmptyItemList();
	
	std::lock_guard<std::recursive_mutex> lock_this{ GetMutex() };
	unflagAllowChanges();

	// delete dynamic attributes
	DeleteDynamicAttributes();
	if (mLazyContent)
	{
		for (auto ref : GetLazyContent()->mWeakRefs)
		{
			ref->ItemIsBeingDeleted();
		}

		// Notify connected items
		for (auto& signal : GetLazyContent()->mConnectedTo)
		{
			for (auto& p : signal.second)
			{
				std::lock_guard<std::recursive_mutex> lock_other{ p.second->GetMutex() };
				p.second->GetLazyContent()->mConnectedToMe[this].erase(std::make_pair(signal.first, p.first));
			}
		}

		for (auto& to : GetLazyContent()->mConnectedToMe)
		{
			for (auto id : to.second)
			{
				std::lock_guard<std::recursive_mutex> lock_other{ to.first->GetMutex() };
				auto& vec = to.first->GetLazyContent()->mConnectedTo[id.first];
				for (auto it = vec.begin(); it != vec.end();)
				{
					if (it->second == this && it->first == id.second)
					{
						it = vec.erase(it);
					}
					else
						++it;
				}
			}
		}

		
	}
}

//!	utility method : init CoreModifiable parameters from given parameter list
void CoreModifiable::InitParametersFromList(const std::vector<CoreModifiableAttribute*>* params)
{
	if(params)	// check if params is not null
	{
		const std::vector<CoreModifiableAttribute*>& paramlist=(*params);
		std::vector<CoreModifiableAttribute*>::const_iterator it;
		for(it=paramlist.begin();it!=paramlist.end();++it)
		{
			const CoreModifiableAttribute*	current=(*it);
			std::string	value;
			if(current->getValue(value))
			{
				setValue(current->getLabel(), value);
			}
		}
	}
}

void CoreModifiable::CallUpdate(const Timer& timer, void* addParam)
{
	// this class has aggregated sons ? call their update
	if (isFlagAsAggregateParent())
	{
		UpdateAggregates(timer, addParam);
	}

	EmitSignal(Signals::Update, this, (CoreModifiable*)&timer);
	Update(timer, addParam);
	// Upgrador updage
	if (mLazyContent)
	{
		UpgradorBase* found = (UpgradorBase * )mLazyContent->GetLinkedListItem(LazyContentLinkedListItemStruct::ItemType::UpgradorType);
		while (found)
		{
			// set current at first place in the list so cache first one
			LazyContentLinkedListItemStruct cachedUpgrador = mLazyContent->mLinkedListItem;
			// set this upgrador at first pos
			mLazyContent->mLinkedListItem = LazyContentLinkedListItemStruct::FromAddressAndType(found, LazyContentLinkedListItemStruct::ItemType::UpgradorType);
			found->UpgradorUpdate(this,timer,addParam);
			// reset cached
			mLazyContent->mLinkedListItem = cachedUpgrador;

			found = (UpgradorBase * )found->getNext(LazyContentLinkedListItemStruct::ItemType::UpgradorType);

		}
	}
}

//! remove item (son)
bool CoreModifiable::removeItem(const CMSP& item)
{
	std::unique_lock<std::recursive_mutex> lk{ GetMutex() };

	auto item_ptr = item.get();

	bool found=false, res=false;
	do
	{
		found=false;
		std::vector<ModifiableItemStruct>::iterator it=mItems.begin();
		std::vector<ModifiableItemStruct>::iterator end=mItems.end();
		for(; it!=end ; ++it)
		{
			if(item_ptr == (*it).mItem.get())
			{
				found = true;
				break;
			}
			//itl++;
		}
		if(found)
		{
			res=true;
			item_ptr->removeUser(this);
			EmitSignal(Signals::RemoveItem, this, item_ptr);
			mItems.erase(it);
		}
	} while(found);
	
	return res;
}

void CoreModifiable::NotifyUpdate(const u32 labelid)
{
	EmitSignal(Signals::NotifyUpdate, this, labelid); 
}

void CoreModifiable::Upgrade(const std::string& toAdd)
{
	UpgradorBase* newone = (UpgradorBase*)KigsCore::Instance()->GetUpgradorFactory()->CreateClassInstance(toAdd);
	if(newone)
		Upgrade(newone);
}

void CoreModifiable::Downgrade(const std::string& toRemove)
{
	if (!mLazyContent)
		return;

	UpgradorBase* previous = nullptr;
	UpgradorBase* found = (UpgradorBase * )mLazyContent->GetLinkedListItem(LazyContentLinkedListItemStruct::ItemType::UpgradorType);

	while (found)
	{
		if (found->getID() == toRemove)
		{
			if (previous)
			{
				previous->mNextItem = found->mNextItem;
			}
			else
			{
				mLazyContent->mLinkedListItem = found->mNextItem;
			}
			found->DowngradeInstance(this);
			break;
		}
		previous = found;
		found = (UpgradorBase*)found->getNext(LazyContentLinkedListItemStruct::ItemType::UpgradorType);
	}
}

CoreModifiable* CoreModifiable::getFirstParent(KigsID ParentClassID) const
{
	for(const auto i : mUsers)
	{
		if(i->isSubType(ParentClassID))
		{
			return i;
		}
	}
	return nullptr;
}

CoreModifiableAttribute* CoreModifiable::getAttribute(KigsID attributeID) const
{
	return findAttribute(attributeID);
}

bool CoreModifiable::setReadOnly(KigsID id,bool val)
{
	auto it =mAttributes.find(id);
	if(it ==mAttributes.end()) return false;
	CoreModifiableAttribute* attribute=(*it).second;
	if(attribute)
	{
		attribute->setReadOnly(val);
		return true;
	}
	return false;
}

bool	CoreModifiable::Equal(const CoreModifiable& other)
{
	// must be same type
	if(getExactType()!=other.getExactType())
	{
		return false;
	}
	
	// must have same sons count
	if(mItems.size() != other.mItems.size())
	{
		return false;
	}
	
	// must have same attribute count
	if(mAttributes.size() != other.mAttributes.size())
	{
		return false;
	}
	
	// parameters check
	auto	itparams=mAttributes.begin();
	auto	endparams=mAttributes.end();
	auto	otheritparams = other.mAttributes.begin();

	std::string	param1str;
	std::string	param2str;
	for(;itparams!=endparams;++itparams)
	{
		((*itparams).second)->getValue(param1str);
		((*otheritparams).second)->getValue(param2str);
		
		if(param1str != param2str)
		{
			return false;
		}
		++otheritparams;
	}
	
	// check sons
	std::vector<ModifiableItemStruct>::const_iterator itsons1=mItems.begin();
	std::vector<ModifiableItemStruct>::const_iterator endsons1=mItems.end();
	std::vector<ModifiableItemStruct>::const_iterator itsons2;
	itsons2=other.mItems.begin();
	for(;itsons1!=endsons1;++itsons1)
	{
		if(!(*itsons1).mItem->Equal(*(*itsons2).mItem.get()))
		{
			return false;
		}
		itsons2++;
	}
	
	return true;
}

//! static method to export the several CoreModifiable to an XML file. if recursive flag is set to true, export also all sons tree
void CoreModifiable::Append(std::string &XMLString,const std::list<CoreModifiable*> &toexport,bool recursive, ExportSettings* settings)
{
	std::vector<CoreModifiable*>	savedList;
	
	XML * xmlfile=XML::createXML();
	
	XMLNode *currentNode =new XMLNode();
	
	ExportSettings default_settings;
	
#ifdef KEEP_XML_DOCUMENT
	default_settings.current_xml_file_node = xmlfile;
	auto old_xml = settings ? settings->current_xml_file_node : nullptr;
	if (settings) settings->current_xml_file_node = xmlfile;
#endif
	//! call recursive, non static method
	std::list<CoreModifiable*>::const_iterator it = toexport.begin();
	std::list<CoreModifiable*>::const_iterator end = toexport.end();
	for (;it != end;++it)
	{
		XMLNode* modifiableNode = new XMLNode();
		(*it)->Export(savedList, modifiableNode, recursive, settings ? settings : &default_settings);
		currentNode->addChild(modifiableNode);
	}
#ifdef KEEP_XML_DOCUMENT
	if (settings) settings->current_xml_file_node = old_xml;
#endif

	xmlfile->setRoot(currentNode);
	
	XMLWriterFile::WriteString(*xmlfile, XMLString, false);
	
	delete xmlfile;
}

//! static method to export the given CoreModifiable to an XML file. if recursive flag is set to true, export also all sons tree
void	CoreModifiable::Export(const std::string &filename,CoreModifiable* toexport,bool recursive, ExportSettings* settings)
{
	std::vector<CoreModifiable*>	savedList;
	
	auto xmlfile = std::shared_ptr<XML>(XML::createXML());
	

	xmlfile->setEncoding("utf-8");

	XMLNode*	modifiableNode=new XMLNode();
	
	ExportSettings default_settings;
#ifdef KEEP_XML_DOCUMENT
	default_settings.current_xml_file_node = xmlfile.get();
	auto old_xml = settings ? settings->current_xml_file_node : nullptr;
	if (settings) settings->current_xml_file_node = xmlfile.get();
#endif

	//! call recursive, non static method
	toexport->Export(savedList, modifiableNode, recursive, settings ? settings : &default_settings);
	
	xmlfile->setRoot(modifiableNode);
	
	xmlfile->WriteFile(filename);

	if (settings)
	{
#ifdef KIGS_TOOLS
		if (settings->current_package)
		{
			settings->current_package->AddFile(filename, filename);
		}
#endif
	}
#ifdef KEEP_XML_DOCUMENT
	toexport->mXMLFiles.push_back(xmlfile);
	toexport->mXMLNodes[xmlfile.get()] = modifiableNode;
#endif

#ifdef KEEP_XML_DOCUMENT
	if (settings) settings->current_xml_file_node = old_xml;
#endif
}


//! static method to export the given CoreModifiable to an XML Node. if recursive flag is set to true, export also all sons tree
XMLNode* CoreModifiable::ExportToXMLNode(CoreModifiable* toexport, XML* owner_xml_file, bool recursive, ExportSettings* settings)
{
	std::vector<CoreModifiable*> savedList;
	ExportSettings default_settings;

#ifdef KEEP_XML_DOCUMENT
	default_settings.current_xml_file_node = owner_xml_file;
	auto old_xml = settings ? settings->current_xml_file_node : nullptr;
	if (settings) settings->current_xml_file_node = owner_xml_file;
#endif

	XMLNode*	modifiableNode = new XMLNode();

	//! call recursive, non static method
	toexport->Export(savedList, modifiableNode, recursive
		, settings ? settings : &default_settings
	);

#ifdef KEEP_XML_DOCUMENT
	if(owner_xml_file)
		toexport->mXMLNodes[owner_xml_file] = modifiableNode;
	if (settings) settings->current_xml_file_node = old_xml;
#endif

	return modifiableNode;
}

//! static method to export the given CoreModifiable to an XML file. if recursive flag is set to true, export also all sons tree
std::string	CoreModifiable::ExportToXMLString(CoreModifiable* toexport, bool recursive, ExportSettings* settings)
{
	std::vector<CoreModifiable*>	savedList;
	ExportSettings default_settings;
	auto xmlfile = std::shared_ptr<XML>(XML::createXML());
	xmlfile->setEncoding("utf-8");

#ifdef KEEP_XML_DOCUMENT
	default_settings.current_xml_file_node = xmlfile.get();
	auto old_xml = settings ? settings->current_xml_file_node : nullptr;
	if (settings) settings->current_xml_file_node = xmlfile.get();
#endif

	XMLNode*	modifiableNode = new XMLNode();

	//! call recursive, non static method
	toexport->Export(savedList, modifiableNode, recursive
		, settings ? settings : &default_settings
	);

	xmlfile->setRoot(modifiableNode);

	std::string result;
	XMLWriterFile::WriteString(*xmlfile, result);
	
#ifdef KEEP_XML_DOCUMENT
	toexport->mXMLFiles.push_back(xmlfile);
	toexport->mXMLNodes[xmlfile.get()] = modifiableNode;
	if (settings) settings->current_xml_file_node = old_xml;
#endif

	return result;
}

//! static method to export the several CoreModifiable to an XML file. if recursive flag is set to true, export also all sons tree
void CoreModifiable::Export(std::string &XMLString,const std::list<CoreModifiable*> &toexport,bool recursive, ExportSettings* settings)
{
	std::vector<CoreModifiable*>	savedList;
	
	auto xmlfile = std::shared_ptr<XML>(XML::createXML());
	
	XMLNode *RootNode =new XMLNode(XML_NODE_ELEMENT, XML_MULTIPLE_EXPORT_ROOT);
	
	ExportSettings default_settings;

#ifdef KEEP_XML_DOCUMENT
	default_settings.current_xml_file_node = xmlfile.get();
	auto old_xml = settings ? settings->current_xml_file_node : nullptr;
	if(settings) settings->current_xml_file_node = xmlfile.get();
#endif

	//! call recursive, non static method
	std::list<CoreModifiable*>::const_iterator it = toexport.begin();
	std::list<CoreModifiable*>::const_iterator end = toexport.end();
	for (;it != end;++it)
	{
		XMLNode* modifiableNode=new XMLNode();
		(*it)->Export(savedList, modifiableNode, recursive
			, settings ? settings : &default_settings
			);
		RootNode->addChild(modifiableNode);

#ifdef KEEP_XML_DOCUMENT
		(*it)->mXMLFiles.push_back(xmlfile);
		(*it)->mXMLNodes[xmlfile.get()] = modifiableNode;
#endif
	}
#ifdef KEEP_XML_DOCUMENT
	if (settings) settings->current_xml_file_node = old_xml;
#endif
	xmlfile->setRoot(RootNode);
	XMLWriterFile::WriteString(*xmlfile, XMLString);
}

#ifdef KEEP_NAME_AS_STRING
void CoreModifiable::RegisterDecorator(const std::string& name)
{
	if(!mDecorators)
	{
		mDecorators=new std::vector<std::string>();
	}
	mDecorators->push_back(name);
}
void CoreModifiable::UnRegisterDecorator(const std::string& name)
{
	if(!mDecorators)
	{
		return;
	}
	std::vector<std::string>::iterator itDecorStart=mDecorators->begin();
	std::vector<std::string>::iterator itDecorEnd=mDecorators->end();
	
	while(itDecorStart != itDecorEnd)
	{
		if((*itDecorStart) == name)
		{
			mDecorators->erase(itDecorStart);
			break;
		}
		
		itDecorStart++;
	}
	
	if(mDecorators->size() == 0)
	{
		delete mDecorators;
		mDecorators=0;
	}
}
#endif

//! recursive method to export "this" to an XML file.
void	CoreModifiable::Export(std::vector<CoreModifiable*>& savedList, XMLNode * currentNode, bool recursive, ExportSettings* settings)
{
#ifdef KEEP_NAME_AS_STRING
	bool unique = false;
	if (getAttribute("ExportUnique"))
	{
		unique = true;
		XMLAttribute *UniqueAttribute = new XMLAttribute("Unique", "true");
		currentNode->addAttribute(UniqueAttribute);
		RemoveDynamicAttribute("ExportUnique");
	}
	if (getAttribute("SeparatedFile"))
	{
		std::string filename;
		getValue("SeparatedFile", filename);

		//d'abord on enlève l'attribut
		RemoveDynamicAttribute("SeparatedFile");
		
		if (filename != "")
		{
			auto ext = filename.find_last_of('.');
			settings->PushID(filename.substr(0, ext) + "/");

			if (settings->external_files_exported.find(filename) == settings->external_files_exported.end())
			{
				//static std::unordered_map<std::string, CoreModifiable*> test;
				
				settings->external_files_exported.insert(filename);
				std::string old_filename = settings->current_xml_file;

				/*auto hdl = Platform_fopen(filename.c_str(), "rb");
				if (hdl->mStatus & FileHandle::Exist)
				{
					auto cm = test[filename];
					__debugbreak();
				}
				test.insert({ filename, this });*/

				Export(filename, this, recursive, settings);
				settings->current_xml_file = old_filename;
			}

			AddDynamicAttribute(ATTRIBUTE_TYPE::STRING, "SeparatedFile", filename.c_str());
			if(unique) AddDynamicAttribute(ATTRIBUTE_TYPE::BOOL, "ExportUnique", true);

			currentNode->setName("Inst");
			if (currentNode->getAttribute("P") == nullptr)
			{
				XMLAttribute *NameAttribute = new XMLAttribute("P", filename);
				currentNode->addAttribute(NameAttribute);
			}
			currentNode->setType(XML_NODE_ELEMENT);
			savedList.push_back(this);
			settings->PopID();
			return;
		}
	}

	currentNode->setName("Inst");
	if (currentNode->getAttribute("N") == nullptr)
	{
		XMLAttribute *NameAttribute = new XMLAttribute("N", getName());
		currentNode->addAttribute(NameAttribute);
	}	
	currentNode->setType(XML_NODE_ELEMENT);
	
	//! first check if node is already saved
	for(std::vector<CoreModifiable*>::iterator it=savedList.begin();it!=savedList.end();++it)
	{
		if(*it==this)
		{
			//! if already saved, just add a reference
			XMLAttribute *attribute = new XMLAttribute("Ref", (*it)->GetRuntimeType());
			currentNode->addAttribute( attribute );
			if (unique) AddDynamicAttribute(ATTRIBUTE_TYPE::BOOL, "ExportUnique", true);
			return;
		}
	}
	
	//
	PrepareExport(settings);
	EmitSignal(Signals::PrepareExport, this, settings);
	
	//! save all attributes
	XMLAttribute *attribute = new XMLAttribute("T", GetRuntimeType());
	currentNode->addAttribute(attribute);
	const kigs::unordered_map<KigsID, CoreModifiableAttribute*>* defaultAttributeMap = 0;
	
	CMSP defaultCopy = nullptr;
	defaultCopy = KigsCore::GetInstanceOf("defaultAttributeClone", GetRuntimeType());
	defaultAttributeMap = &defaultCopy->getAttributes();
	
	for (auto i = mAttributes.begin(); i != mAttributes.end(); ++i)
	{
		CoreModifiableAttribute* current = (*i).second;
		auto type = current->getType();

		if (type == CoreModifiable::ATTRIBUTE_TYPE::RAWPTR)
			continue;

		if (current->isDynamic() && type == ATTRIBUTE_TYPE::STRING)
		{
			std::vector<std::string> splitted = SplitStringByCharacter(current->getLabel()._id_name, '$');
			if (splitted.size() == 3 && splitted[1] == "LUA_CODE")
			{
				std::string func_name = splitted[2];
				XMLNode* luanode = new XMLNode(XML_NODE_ELEMENT, "LUA");
				XMLAttribute* attr = new XMLAttribute("N", func_name);
				std::string code;
				current->getValue(code);
				luanode->setString("<![CDATA[" + code + "]]>");
				luanode->addAttribute(attr);
				currentNode->addChild(luanode);
				continue;
			}
		}


		std::string value;
		current->getValue(value);
		if (defaultAttributeMap)
		{
			auto itfounddefault = defaultAttributeMap->find((*i).first);
			if (itfounddefault != defaultAttributeMap->end())
			{
				std::string defaultvalue;
				(*itfounddefault).second->getValue(defaultvalue);
				
				if (defaultvalue == value)
					continue;
			}
		}
		

		XMLNode*	modifiableAttrNode = new XMLNode();
		modifiableAttrNode->setType(XML_NODE_ELEMENT);
		modifiableAttrNode->setName("Attr");
		{
			if (current->isDynamic())
			{
				attribute = new XMLAttribute("T", CoreModifiableAttribute::typeToString(type));
				modifiableAttrNode->addAttribute(attribute);
			}
			attribute = new XMLAttribute("N", current->getLabel()._id_name);
			modifiableAttrNode->addAttribute(attribute);
			// CoreItem special case
			if (type == CoreModifiable::ATTRIBUTE_TYPE::COREITEM)
			{
				maCoreItem* currentCoreItem = (maCoreItem*)current;
				if (currentCoreItem->getRefFile() != "")
				{
					value = "#" + currentCoreItem->getRefFile();
					attribute = new XMLAttribute("V", value);
					modifiableAttrNode->addAttribute(attribute);
				}
				else
				{
					// add a CData
					XMLNode*	CDataNode = new XMLNode();
					CDataNode->setType(XML_NODE_TEXT_NO_CHECK);
					CDataNode->setName("CData");
					CDataNode->setString(value);
#ifdef USE_PARENT_AND_SIBLING
					CDataNode->setParent(modifiableAttrNode);
#endif
					modifiableAttrNode->addChild(CDataNode);
				}
			}
			else if (type == CoreModifiable::ATTRIBUTE_TYPE::COREBUFFER)
			{
				auto unique_id = settings->GetCurrentUniqueID();

				bool export_inline = true;

				if (unique_id.size())
				{
					CoreRawBuffer* buffer = nullptr;
					current->getValue((void*&)buffer);
					if (buffer->size() >= settings->export_buffer_attribute_as_external_file_size_threshold)
					{
						CMSP& compressManager = KigsCore::GetSingleton("KXMLManager");
						auto path = unique_id + "_" + current->getLabel()._id_name + (compressManager ? std::string(".kbin") : ".bin");
						attribute = new XMLAttribute("V", "#" + path);
						modifiableAttrNode->addAttribute(attribute);
						export_inline = false;
						if (compressManager)
						{
							auto result = OwningRawPtrToSmartPtr(new CoreRawBuffer);
							compressManager->SimpleCall("CompressData", buffer, result.get());
							ModuleFileManager::SaveFile(path.c_str(), (u8*)result->data(), result->size());
						}
						else
						{
							ModuleFileManager::SaveFile(path.c_str(), (u8*)buffer->data(), buffer->length());
						}
						if (settings->current_package)
							settings->current_package->AddFile(path, path);
					}
				}
				
				if(export_inline)
				{
					current->getValue(value);
					attribute = new XMLAttribute("V", value);
					modifiableAttrNode->addAttribute(attribute);
				}
			}
			else
			{
				attribute = new XMLAttribute("V", value);
				modifiableAttrNode->addAttribute(attribute);
			}

			if (current->isDynamic())
			{
				attribute = new XMLAttribute((std::string)"Dyn", (std::string)"yes");
				modifiableAttrNode->addAttribute(attribute);
				
			}
			
			// export modifier
			AttachedModifierBase* exportedModifier=current->getFirstAttachedModifier();
			while (exportedModifier)
			{
				XMLNode*	modifierNode = new XMLNode();
				modifierNode->setType(XML_NODE_ELEMENT);
				modifierNode->setName("Mod");
				XMLAttribute*	modifierType = new XMLAttribute((std::string)"T", (std::string)exportedModifier->GetModifierType());
				modifierNode->addAttribute(modifierType);
				XMLAttribute*	modifierV = new XMLAttribute((std::string)"V", (std::string)exportedModifier->GetModifierInitString());
				modifierNode->addAttribute(modifierV);
				
				if (!exportedModifier->isGetterModifier())
				{
					XMLAttribute* setter = new XMLAttribute((std::string)"Setter", (std::string)"yes");
					modifierNode->addAttribute(setter);
				}
#ifdef USE_PARENT_AND_SIBLING				
				modifierNode->setParent(modifierNode);
#endif //USE_PARENT_AND_SIBLING
				modifiableAttrNode->addChild(modifierNode);
				exportedModifier = exportedModifier->getNext();
			}
			
		}
#ifdef USE_PARENT_AND_SIBLING	
		modifiableAttrNode->setParent(currentNode);
#endif //USE_PARENT_AND_SIBLING
		currentNode->addChild(modifiableAttrNode);
	}
	
	
	defaultCopy = nullptr;
	
	
	// export binaries if needed (CData)
	s32 countCData = HasCDataToExport();
	s32 CDataIndex;
	for (CDataIndex = 0; CDataIndex < countCData; CDataIndex++)
	{
		std::string	toExport;
		if (GetCDataToExport(CDataIndex, toExport))
		{
			XMLNode*	CDataNode = new XMLNode();
			CDataNode->setType(XML_NODE_TEXT_NO_CHECK);
			CDataNode->setName("CData");
			CDataNode->setString(toExport);
#ifdef USE_PARENT_AND_SIBLING	
			CDataNode->setParent(currentNode);
#endif //USE_PARENT_AND_SIBLING
			currentNode->addChild(CDataNode);
		}
	}
	
	// export decorators
	if (mDecorators)
	{
		std::vector<std::string>::iterator itDecorStart = mDecorators->begin();
		std::vector<std::string>::iterator itDecorEnd = mDecorators->end();
		
		while (itDecorStart != itDecorEnd)
		{
			XMLNode*	modifiableAttrNode = new XMLNode();
			modifiableAttrNode->setType(XML_NODE_ELEMENT);
			modifiableAttrNode->setName("Deco");
			XMLAttribute *DecoratorAttribute = new XMLAttribute("N", (*itDecorStart));
			modifiableAttrNode->addAttribute(DecoratorAttribute);
#ifdef USE_PARENT_AND_SIBLING
			modifiableAttrNode->setParent(currentNode);
#endif //USE_PARENT_AND_SIBLING 
			currentNode->addChild(modifiableAttrNode);
			itDecorStart++;
		}
		
	}

	// export Upgrador
	UpgradorBase* upgradorfound = GetUpgrador();
	while (upgradorfound)
	{
		XMLNode* upgradorNode = new XMLNode();
		upgradorNode->setType(XML_NODE_ELEMENT);
		upgradorNode->setName("Upgrd");
		XMLAttribute* UpgradorAttribute = new XMLAttribute("N", upgradorfound->getID()._id_name);
		upgradorNode->addAttribute(UpgradorAttribute);
		currentNode->addChild(upgradorNode);
		upgradorfound = (UpgradorBase * )upgradorfound->getNext(LazyContentLinkedListItemStruct::ItemType::UpgradorType);
	}

	savedList.push_back(this);
	if (unique) AddDynamicAttribute(ATTRIBUTE_TYPE::BOOL, "ExportUnique", true);

	//! if recursive mode, save all sons
	if (recursive)
	{
		std::vector<ModifiableItemStruct>::iterator i = mItems.begin();
		std::vector<ModifiableItemStruct>::iterator e = mItems.end();
		for (; i != e; ++i)
		{
			CoreModifiable* current = (*i).mItem.Pointer();
			
			if (current->getAttribute("NoExport")) continue;

			XMLNode*	sonNode = new XMLNode();

			if ((*i).isAggregate())
			{
				// add aggregate attribute 
				XMLAttribute *AggregateAttribute = new XMLAttribute("Aggregate", "true");
				sonNode->addAttribute(AggregateAttribute);
			}
			if ((*i).mItem->isFlagAsAutoUpdateRegistered())
			{
				// add autoupdate attribute 
				XMLAttribute* AutoUpdateAttribute = new XMLAttribute("AutoUpdate", "true");
				sonNode->addAttribute(AutoUpdateAttribute);
			}
			
			current->Export(savedList, sonNode, recursive, settings);
			
#ifdef KEEP_XML_DOCUMENT
			if(settings->current_xml_file_node)
				current->mXMLNodes[settings->current_xml_file_node] = sonNode;
#endif
#ifdef USE_PARENT_AND_SIBLING
			sonNode->setParent(currentNode);
#endif // USE_PARENT_AND_SIBLING
			currentNode->addChild(sonNode);
		}
		
	}

	EmitSignal(Signals::EndExport, this, settings);
	EndExport(settings);

#else
	KIGS_ASSERT(0 && "Export is not possible if KEEP_NAME_AS_STRING flag is undefined");
#endif
}

//! return an new CoreModifiable instance using the description found in XML file given by name
CMSP	CoreModifiable::Import(const std::string &filename, bool noInit, bool keepImportFileName, ImportState* state, const std::string& override_name)
{
	auto xmlbasefile = XML::ReadFile(filename, 0);

	if (xmlbasefile && xmlbasefile->useStringRef())
	{
		auto xmlfile = std::shared_ptr<XMLStringRef>((XMLStringRef*)xmlbasefile);
		return Import<std::string_view>(xmlfile, filename, noInit, keepImportFileName, state, override_name);
	}
	auto xmlfile = std::shared_ptr<XML>((XML*)xmlbasefile);
	return Import<std::string>(xmlfile, filename, noInit, keepImportFileName, state, override_name);
}


bool	CoreModifiable::ImportAttributes(const std::string &filename)
{
	bool result = false;
	// load XML
	XMLTemplate<std::string_view> * xmlfile = (XMLTemplate<std::string_view> * )XML::ReadFile(filename, 0);
	
	if (xmlfile)
	{
		ImportState importState;
		importState.UTF8Enc = false;
		if (xmlfile->getEncoding() == "UTF-8" || xmlfile->getEncoding() == "utf-8")
		{
			importState.UTF8Enc = true;
		}
		
		// search for this in xml
		XMLNodeBase* rootnode = xmlfile->getRoot();
		std::vector<XMLNodeBase*> nodelist = rootnode->getNodes(XML_NODE_ELEMENT);
		
		std::vector<XMLNodeBase*>::iterator	itb = nodelist.begin();
		std::vector<XMLNodeBase*>::iterator	ite = nodelist.end();
		
		while (itb != ite)
		{
			if ((*itb)->compareName("Instance") || (*itb)->compareName("Inst"))
			{
				XMLNodeBase* currentNode = (*itb);
				// retreive type and name attributes
				XMLAttributeBase *NameAttribute = currentNode->getAttribute("Name");
				if (!NameAttribute)
					NameAttribute = currentNode->getAttribute("N");
				
				XMLAttributeBase*	TypeAttribute = currentNode->getAttribute("Type");
				if (!TypeAttribute)
					TypeAttribute = currentNode->getAttribute("T");
				
				if (NameAttribute && TypeAttribute)
				{
					std::string name = NameAttribute->getString();
					if (name == getName())
					{
						std::string type = TypeAttribute->getString();
						if (isSubType(type))
						{
							std::vector<XMLNodeBase*>	sons;
							sons.clear();
							ImportAttributes<std::string_view>(currentNode, this, importState,sons);
							ImportSons<std::string_view>(sons, this, importState);
							result = true;
							break;
						}
					}
				}
				
			}
			
			++itb;
		}
		
		delete xmlfile;
	}
	return result;
}

void	CoreModifiable::InitLuaScript(XMLNodeBase* currentNode, CoreModifiable* currentModifiable, ImportState& importState)
{
	CoreModifiable* luamodule = KigsCore::GetModule("LuaKigsBindModule");
	if (!luamodule)
		return;

	XMLAttributeBase* attrname = currentNode->getAttribute("N", "Name");

	if (!attrname)
		return;

	XMLAttributeBase* attrtype = currentNode->getAttribute("T", "Type");
	XMLAttributeBase* attrvalue = currentNode->getAttribute("V", "Value");

	std::string code = "";
	if (attrvalue)
	{
		code = attrvalue->getString();
		if (code.size() && code[0] == '#')
		{
			code.erase(code.begin());

			u64 size;
			CoreRawBuffer* rawbuffer = ModuleFileManager::LoadFileAsCharString(code.c_str(), size, 1);
			if (rawbuffer)
			{
				code = rawbuffer->buffer();
				rawbuffer->Destroy();
			}
			else
			{
				STACK_STRING(errstr, 1024, "Cannot load LUA script : %s", code.c_str());
				KIGS_ERROR(errstr, 3);
			}
		}
	}
	else
	{
		if (currentNode->getChildCount())
		{
			for (s32 i = 0; i < currentNode->getChildCount(); i++)
			{
				XMLNodeBase* sonXML = currentNode->getChildElement(i);
				if ((sonXML->getType() == XML_NODE_TEXT_NO_CHECK) || (sonXML->getType() == XML_NODE_TEXT))
				{
					code = sonXML->getString();
					break;
				}
			}
		}
	}


	std::vector<CoreModifiableAttribute*> params;
	maString pName("pName", (std::string)attrname->getString());
	maString pCode("pCode", code);
	maRawPtr pXML("pXML", currentNode);

	params.push_back(&pName);
	params.push_back(&pCode);
	params.push_back(&pXML);

	maReference localthis("pThis", { "" });
	localthis = currentModifiable;
	params.push_back(&localthis);

	maString cbType("cbType", attrtype ? attrtype->getString() : "");
	if (attrtype)
		params.push_back(&cbType);

	luamodule->CallMethod("RegisterLuaMethod", params);
}

AttachedModifierBase* CoreModifiable::InitAttributeModifier(XMLNodeBase* currentNode, CoreModifiableAttribute* attr)
{
	XMLAttributeBase* attrtype = currentNode->getAttribute("T", "Type");

	AttachedModifierBase* toAdd = 0;
	if (attrtype)
	{
		std::string modifiertype = attrtype->getString();
		if (modifiertype != "")
		{
			auto& instanceMap = KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap();
			auto itfound = instanceMap.find(modifiertype);
			if (itfound != instanceMap.end())
			{
				toAdd = (AttachedModifierBase*)(*itfound).second();
			}
		}

		if (toAdd != 0)
		{
			// is setter ?
			bool isSetter = false;
			XMLAttributeBase* attrsetter = currentNode->getAttribute("Setter", "isSetter");

			if (attrsetter)
			{
				if ((attrsetter->getRefString() == "true") || (attrsetter->getRefString() == "yes"))
				{
					isSetter = true;
				}
			}

			// search value
			std::string value = "";
			XMLAttributeBase* attrvalue = currentNode->getAttribute("V", "Value");

			if (attrvalue)
			{
				value = attrvalue->getString();
			}

			// check for direct string
			if (value == "")
			{
				for (s32 i = 0; i < currentNode->getChildCount(); i++)
				{
					XMLNodeBase* sonXML = currentNode->getChildElement(i);
					if ((sonXML->getType() == XML_NODE_TEXT_NO_CHECK) || (sonXML->getType() == XML_NODE_TEXT))
					{
						value = sonXML->getString();
						break;
					}
				}
			}

			toAdd->Init(attr, !isSetter, value);
			attr->attachModifier(toAdd);
		}
	}

	return toAdd;
}


void	CoreModifiable::ReleaseLoadedItems(std::vector<CMSP> &loadedItems)
{
	loadedItems.clear();
}


//
CoreModifiable* CoreModifiable::Find(const std::list<CoreModifiable*> &List, const std::string &Name)
{
	std::list<CoreModifiable*>::const_iterator Iter;
	for (Iter = List.begin(); Iter != List.end(); Iter++)
		if ((*Iter)->getName() == Name)
			return *Iter;
	return nullptr;
}

CoreModifiable* CoreModifiable::FindByType(const std::list<CoreModifiable*> &List, const std::string& type)
{
	std::list<CoreModifiable*>::const_iterator Iter;
	for (Iter = List.begin(); Iter != List.end(); Iter++)
		if ((*Iter)->isSubType(type))
			return *Iter;
	return nullptr;
}

//! static method : return the instance corresponding to the given path, and given search start

CMSP CoreModifiable::GetInstanceByGlobalPath(const std::string &path)
{
	std::string RemainingPath;
	std::string sonName = GetFirstNameInPath(path, RemainingPath);

	
	// check if sonName is composed by type+name 
	std::string searchType = "CoreModifiable";
	size_t pos = sonName.find(':');

	if (pos != std::string::npos)
	{
		searchType = sonName.substr(0, pos);
		sonName = sonName.substr(pos + 1, sonName.length() - pos - 1);
	}

	// search using each root instances
	std::vector<CMSP> instances =	GetInstancesByName(searchType, sonName);

	std::vector<CMSP>::iterator	itset;
	for (itset = instances.begin(); itset != instances.end(); itset++)
	{
		CMSP&	current = (*itset);
		
		CMSP	test = current->GetInstanceByPath(RemainingPath);
		if (test)
		{
			return test;
		}
		
	}
	return nullptr;
}

std::string CoreModifiable::GetFirstNameInPath(const std::string &path, std::string & remainingpath)
{
	remainingpath = "";
	std::string sonName = path;
	if (sonName == "")
	{
		return "";
	}
	// check that first char is not '/'
	if (sonName[0] == '/')
	{
		sonName = "/";
		remainingpath = path.substr(1, path.length() - 1);
		return sonName;
	}
	// check that first char is not '@'
	if (sonName[0] == '@')
	{
		sonName = "@";
		remainingpath = path.substr(1, path.length() - 1);
		return sonName;
	}

	// then isolate sonName only
	size_t posSlash = sonName.find('/');
	if (posSlash != std::string::npos)
	{
		remainingpath = sonName.substr(posSlash + 1, sonName.length() - posSlash - 1);
		sonName = sonName.substr(0, posSlash);
	}

	return sonName;
}


std::vector<CMSP> CoreModifiable::getRootParentsWithPath(std::string &remainingpath)
{
	std::string::size_type pos = remainingpath.find('/');
	std::string searchName = remainingpath;
	if (pos != std::string::npos)
	{
		searchName = remainingpath.substr(0, pos);
		remainingpath = remainingpath.substr(pos + 1, remainingpath.length() - pos - 1);
	}
	else
	{
		remainingpath = "";
	}

	std::string searchType = "CoreModifiable";
	pos = searchName.find(':');

	if (pos != std::string::npos)
	{
		searchType = searchName.substr(0, pos);
		searchName = searchName.substr(pos + 1, searchName.length() - pos - 1);
	}

	std::vector<CMSP> parents;
	recursiveGetRootParentsWithPath(searchType, searchName, parents);
	return parents;
}

void CoreModifiable::recursiveGetRootParentsWithPath(const std::string& searchType, const std::string& searchName, std::vector<CMSP>& parents)
{
	std::unique_lock<std::recursive_mutex> lk{ GetMutex() }; // Don't want to be modified by another thread here
	// check this
	if (getName() == searchName)
	{
		if (isSubType(searchType))
		{
			parents.push_back(CMSP(this, GetRefTag{}));
		}
	}

	// recurse to parents
	const std::vector<CoreModifiable*>& parentlist = GetParents();

	for (auto p : parentlist)
	{
		p->recursiveGetRootParentsWithPath(searchType, searchName, parents);
	}
}


CMSP CoreModifiable::SearchInstance(const std::string& infos, CoreModifiable* searchStart)
{
	if (searchStart) // relative search ?
	{
		CMSP found= searchStart->GetInstanceByPath(infos);
		if (found) // if not found, search global path
		{
			return found;
		}
	}

	// global search
	return GetInstanceByGlobalPath(infos);
}


//! return the instance corresponding to the given path in sons tree
CMSP CoreModifiable::GetInstanceByPath(const std::string &path)
{
	std::unique_lock<std::recursive_mutex> lk{ GetMutex() }; // don't want to be changed in another thread during search
	std::string RemainingPath;
	std::string sonName=GetFirstNameInPath(path,RemainingPath);

	if (sonName == "")
	{
		return CMSP(this, GetRefTag{});
	}


	// force reference
	if (sonName == "@")
	{
		std::string::size_type pos = path.find(':');

		std::string searchType = path.substr(1, pos-1); // remove @
		std::string searchName = path.substr(pos + 1, path.length() - pos - 1); // remove :

		return CoreModifiable::GetFirstInstanceByName(searchType, searchName);
	}

	if(sonName == "/") // path start with / => start from root parents
	{
		// search root of this with given name
		std::string RemainingPathForGlobal = RemainingPath;
		std::vector<CMSP> parents=	getRootParentsWithPath(RemainingPath);
		for (auto p : parents)
		{
			CMSP oneFound=p->GetInstanceByPath(RemainingPath);
			if (oneFound)
			{
				return oneFound;
			}
		}
		return GetInstanceByGlobalPath(RemainingPathForGlobal);
		
	}

	std::string::size_type pos = sonName.find(':');

	std::string searchType = "CoreModifiable";

	if (pos != std::string::npos)
	{
		searchType = sonName.substr(0, pos);
		sonName = sonName.substr(pos + 1, sonName.length() - pos - 1);
	}
	

	if (sonName == "**")
	{
		std::string nextpath = RemainingPath;
		std::string targetName = GetFirstNameInPath(nextpath, RemainingPath);

		std::vector<CMSP> instances;
		GetSonInstancesByName(searchType, targetName, instances, true);

		for (auto cm : instances)
		{
			auto result = cm->GetInstanceByPath(RemainingPath);
			if (result)
			{
				return result;
			}
		}
		return nullptr;
	}
	
	// go to father(s) and recurse search
	if(sonName=="..")
	{
		std::vector<CoreModifiable*>::iterator itfathers;
		for(itfathers=mUsers.begin();itfathers!=mUsers.end();itfathers++)
		{
			if ((*itfathers)->isSubType(searchType))
			{
				auto foundinpath = (*itfathers)->GetInstanceByPath(RemainingPath);
				if (foundinpath)
				{
					return foundinpath;
				}
			}
		}
		return nullptr;
	}
	
	// search son with son name
	std::vector<ModifiableItemStruct>::iterator itsons;
	bool is_wildcard = sonName == "*";
	for(itsons=mItems.begin();itsons!=mItems.end();itsons++)
	{
		if(is_wildcard || (*itsons).mItem->getName() == sonName)
		{
			if ((*itsons).mItem->isSubType(searchType))
			{
				auto result = (*itsons).mItem->GetInstanceByPath(RemainingPath);
				if (!is_wildcard || result) return result;
			}
		}
	}
	
	
	return nullptr;
}

void CoreModifiable::GetSonInstancesByName(KigsID cid, const std::string &name, std::vector<CMSP>& instances,bool recursive)
{
	std::unique_lock<std::recursive_mutex> lk{ GetMutex() }; // don't want to be changed in another thread
	std::vector<ModifiableItemStruct>::iterator itsons;
	for(itsons=mItems.begin();itsons!=mItems.end();itsons++)
	{
		if((*itsons).mItem->getName() == name)
		{
			if((*itsons).mItem->isSubType(cid))
			{
				instances.push_back((*itsons).mItem);
			}
		}
		if(recursive)
		{
			(*itsons).mItem->GetSonInstancesByName(cid,name,instances,recursive);
		}
	}
}

CMSP CoreModifiable::GetFirstSonByName(KigsID cid, const std::string &name, bool recursive)
{
	std::unique_lock<std::recursive_mutex> lk{ GetMutex() }; // don't want to be changed in another thread
	for (auto& son : getItems())
	{
		if (son.mItem->isSubType(cid) && name == son.mItem->getName())
			return son.mItem;
	}
	if (recursive)
	{
		for (auto& son : getItems())
		{
			auto found = son.mItem->GetFirstSonByName(cid, name, true);
			if (found) return found;
		}
	}
	return nullptr;
}

void CoreModifiable::GetSonInstancesByType(KigsID cid, std::vector<CMSP>& instances,bool recursive)
{
	std::unique_lock<std::recursive_mutex> lk{ GetMutex() }; // don't want to be changed in another thread
	std::vector<ModifiableItemStruct>::iterator itsons;
	for(itsons=mItems.begin();itsons!=mItems.end();itsons++)
	{
		if((*itsons).mItem->isSubType(cid))
		{
			instances.push_back((*itsons).mItem);
		}
		
		if(recursive)
		{
			(*itsons).mItem->GetSonInstancesByType(cid,instances,recursive);
		}
	}
}

CMSP CoreModifiable::GetFirstSonByType(KigsID cid, bool recursive)
{
	std::unique_lock<std::recursive_mutex> lk{ GetMutex() }; // don't want to be changed in another thread
	for (auto& son : getItems())
	{
		if (son.mItem->isSubType(cid))
			return son.mItem;
	}
	if (recursive)
	{
		for (auto& son : getItems())
		{
			auto found = son.mItem->GetFirstSonByType(cid, true);
			if (found) return found;
		}
	}
	return nullptr;
}




// attr with "eval(" + ")" expression
void	CoreModifiable::EvalAttribute(std::string& attr,CoreModifiable* owner, CoreModifiableAttribute* destattr)
{
	std::string toeval = attr.substr(4, attr.length() - 4);
	
	CoreItemSP ItemToEval;

	ATTRIBUTE_TYPE destType = ATTRIBUTE_TYPE::FLOAT;
	
	// if dest attribute was given, then use its type to know what to do
	if (destattr)
	{
		destType=destattr->getType();
	}
	switch (destType)
	{
		case ATTRIBUTE_TYPE::BOOL: // use float operator but result string will be "false" if float result is 0.0f, "true" if float result != 0  
		{
			ItemToEval = CoreItemOperator<kfloat>::Construct(toeval, owner, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
			kfloat result = *ItemToEval.get();
			if (result == 0)
			{
				attr = "false";
			}
			else
			{
				attr = "true";
			}

		}
		break;
		case ATTRIBUTE_TYPE::USSTRING: // use string operator. WARNING not tested feature  
		case ATTRIBUTE_TYPE::STRING: // use string operator. WARNING not tested feature  
		{
			ItemToEval = CoreItemOperator<std::string>::Construct(toeval, owner, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
			attr = (std::string)*ItemToEval.get();
		}
		break;
		case ATTRIBUTE_TYPE::COREITEM: // don't evaluate coreitems as we want them to be evaluated when called
		{
			// DO NOTHING
		}
		break;
		case ATTRIBUTE_TYPE::ARRAY:
		{
			// check for 2D / 3D vectors
			int arraySize = destattr->getNbArrayElements();
			if (arraySize == 2)
			{
				ItemToEval = CoreItemOperator<Point2D>::Construct(toeval, owner, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());

				Point2D result((Point2D)ItemToEval);
				char resultBuffer[128];
				sprintf(resultBuffer, "{%f,%f}", result.x,result.y);
				attr = resultBuffer;
			}
			else if (arraySize == 3)
			{
				ItemToEval = CoreItemOperator<Point3D>::Construct(toeval, owner, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
				Point3D result((Point3D)ItemToEval);
				char resultBuffer[128];
				sprintf(resultBuffer, "{%f,%f,%f}", result.x, result.y,result.z);
				attr = resultBuffer;
			}
			else if (arraySize == 4)
			{
				ItemToEval = CoreItemOperator<Vector4D>::Construct(toeval, owner, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
				Vector4D result(ItemToEval->operator Vector4D());
				char resultBuffer[200];
				sprintf(resultBuffer, "{%f,%f,%f,%f}", result.x, result.y, result.z,result.w);
				attr = resultBuffer;
			}
		}
		break;
		default:  // use float operator and return a float "printed" in a string 
		{
			ItemToEval = CoreItemOperator<kfloat>::Construct(toeval, owner, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());

			kfloat result((kfloat)ItemToEval);

			char resultBuffer[128];

			sprintf(resultBuffer, "%f", result);
			attr = resultBuffer;
		}
	}	
}

CoreModifiableAttribute* CoreModifiable::getParameter(const std::vector<CoreModifiableAttribute*>& params, KigsID ID)
{
	auto it = std::find_if(params.begin(), params.end(), [&](CoreModifiableAttribute* attr) { return attr->getLabel() == ID; });
	if (it == params.end()) return nullptr;
	return *it;
}


void CoreModifiable::RecursiveUpdate(const Timer&  timer, CoreModifiable* a_parent)
{ 
	if (mModifiableFlag&((u32)RecursiveUpdateFlag))
	{
		CallUpdate(timer,0);
		
		const std::vector<ModifiableItemStruct>& L_sons = getItems();
		if (!L_sons.empty())
		{
			std::vector<ModifiableItemStruct>::const_iterator L_it = L_sons.begin();
			std::vector<ModifiableItemStruct>::const_iterator L_ite = L_sons.end();
			
			while (L_it != L_ite)
			{
				(*L_it).mItem->RecursiveUpdate(timer, this);
				L_it++;
			}
		}
	}
}

CoreModifiable* CoreModifiable::recursiveGetRootParentByType(const KigsID& ParentClassID, s32 currentLevel, s32 &foundLevel) const
{
	const std::vector<CoreModifiable*>& parents=GetParents();

	if ((currentLevel > foundLevel) && (foundLevel >= 0))
	{
		return 0; // already found a best candidate
	}

	auto	search = parents.begin();
	auto	searchE = parents.end();

	while (search != searchE)
	{
		if ((*search)->isSubType(ParentClassID))
		{
			if ((currentLevel < foundLevel) || (foundLevel < 0))
			{
				foundLevel = currentLevel;
				return (*search);
			}
		}
		else
		{
			CoreModifiable *found = (*search)->recursiveGetRootParentByType(ParentClassID, currentLevel + 1, foundLevel);
			if (found)
			{
				return found;
			}
		}
		++search;
	}

	return 0;
}

void CoreModifiable::AddWeakRef(WeakRef* ref)
{
	GetLazyContent()->mWeakRefs.push_back(ref);
}

void CoreModifiable::RemoveWeakRef(WeakRef* ref)
{
	auto lz = GetLazyContent();
	auto itfind = std::find(lz->mWeakRefs.begin(), lz->mWeakRefs.end(), ref);
	if (*itfind != lz->mWeakRefs.back())
		*itfind = lz->mWeakRefs.back();
	lz->mWeakRefs.pop_back();
}

#ifdef KIGS_TOOLS
void CoreModifiable::GetRef()
{
	GenericRefCountedBaseClass::GetRef();
#ifdef KIGS_TOOLS
	if (mTraceRef)
		TRACEREF_RETAIN;
#endif
}
bool CoreModifiable::TryGetRef()
{
	bool ok = GenericRefCountedBaseClass::TryGetRef();
#ifdef KIGS_TOOLS
	if (ok && mTraceRef)
		TRACEREF_RETAIN;
#endif
	return ok;
}
void CoreModifiable::Destroy()
{
#ifdef KIGS_TOOLS
	if (mTraceRef)
		TRACEREF_RELEASE;
#endif
	GenericRefCountedBaseClass::Destroy();
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////

//! some utility MACROS for string to value and value to string conversion
#define IMPLEMENT_CONVERT_S2V_ANS_V2S(T,format) \
template<> bool CoreConvertString2Value<T>(const std::string &stringValue,T& value) \
{ \
	if(sscanf(stringValue.c_str(), format, &value)!=1) { \
		bool b; \
		if (CoreConvertString2Value<bool>(stringValue, b))\
		{\
			value = b ? std::numeric_limits<T>::max() : 0;\
			return true;\
		}\
		return false; \
	}\
	return true; \
} \
template<> bool CoreConvertValue2String<T>(std::string& stringValue,T value) \
{ \
	char convertBuffer[256]; \
	if(snprintf(convertBuffer, 256, format,value)==0) return false; \
	stringValue=convertBuffer; \
	return true; \
}


template<> bool CoreConvertString2Value<kfloat>(const std::string &stringValue,kfloat& value)
{
	float fvalue;
	if(sscanf(stringValue.c_str(), "%f", &fvalue)!=1)
	{
		return false;
	}
	value=(kfloat)fvalue;
	return true;
}
template<> bool CoreConvertValue2String<kfloat>(std::string& stringValue,kfloat value)
{
	char convertBuffer[256];
	if(sprintf(convertBuffer,"%f",CastToFloat(value))==0) return false;
	stringValue=convertBuffer;
	return true;
}

template<> bool CoreConvertString2Value<kdouble>(const std::string &stringValue,kdouble& value)
{
	double dvalue;
	if(sscanf(stringValue.c_str(), "%lf", &dvalue)!=1)
	{
		return false;
	}
	value=(kdouble)dvalue;
	return true;
}
template<> bool CoreConvertValue2String<kdouble>(std::string& stringValue,kdouble value)
{
	char convertBuffer[256];
	if(sprintf(convertBuffer,"%lf",CastToDouble(value))==0) return false;
	stringValue=convertBuffer;
	return true;
}

template<> bool CoreConvertString2Value<bool>(const std::string &stringValue,bool& value)
{
	if(stringValue=="false" || stringValue=="FALSE" || stringValue=="0")
	{
		value=false;
		return true;
	}
	if(stringValue=="true" || stringValue=="TRUE")
	{
		value=true;
		return true;
	}
	
	// check if numeric val
	float fvalue;
	if(sscanf(stringValue.c_str(), "%f", &fvalue)==1)
	{
		if(fvalue != 0.0f)
		{
			value=true;
			return true;
		}
		return false;
	}
	
	return false;
}
template<> bool CoreConvertValue2String<bool>(std::string& stringValue,bool value)
{
	stringValue=(value?"true":"false");
	return true;
}
template<> bool CoreConvertString2Value<std::string>(const std::string &stringValue,std::string& value)
{
	value=stringValue;
	return true;
}
template<> bool CoreConvertValue2String<std::string>(std::string& stringValue,std::string value)
{
	stringValue=value;
	return true;
}

IMPLEMENT_CONVERT_S2V_ANS_V2S(s8, "%hhi")
IMPLEMENT_CONVERT_S2V_ANS_V2S(s16, "%hi")
IMPLEMENT_CONVERT_S2V_ANS_V2S(s32, "%i")
IMPLEMENT_CONVERT_S2V_ANS_V2S(s64, "%lli")
IMPLEMENT_CONVERT_S2V_ANS_V2S(u8, "%hhu")
IMPLEMENT_CONVERT_S2V_ANS_V2S(u16, "%hu")
IMPLEMENT_CONVERT_S2V_ANS_V2S(u32, "%u")
IMPLEMENT_CONVERT_S2V_ANS_V2S(u64, "%llu")
//IMPLEMENT_CONVERT_S2V_ANS_V2S(kfloat,"%f")
//IMPLEMENT_CONVERT_S2V_ANS_V2S(kdouble,"%lf")

PackCoreModifiableAttributes::~PackCoreModifiableAttributes()
{
	for (auto attr : mAttributeList)
		delete attr;
}



WeakRef& WeakRef::operator=(const WeakRef& copy_that)
{
	if (&copy_that == this) return *this;
	to = copy_that.to;
	alive = copy_that.alive;
	if (IsValid()) to->AddWeakRef(this);
	return *this;
}

WeakRef& WeakRef::operator=(WeakRef&& move_that)
{
	if (&move_that == this) return *this;
	
	to = move_that.to;
	alive = move_that.alive;

	if (IsValid())
	{
		to->AddWeakRef(this);
		to->RemoveWeakRef(&move_that);
	}
	move_that.to = nullptr;
	move_that.alive = false;
	return *this;
}

WeakRef& WeakRef::operator=(CoreModifiable* item)
{
	if (item == to) return *this;

	to = item;
	alive = true;
	if (IsValid()) to->AddWeakRef(this);
	return *this;
}

WeakRef::~WeakRef()
{
	if (IsValid()) to->RemoveWeakRef(this);
}

void WeakRef::ItemIsBeingDeleted()
{
	alive = false;
}

SmartPointer<CoreModifiable> WeakRef::Lock() const
{
	if (!IsValid()) return {};
	if (to) to->GetRef();
	return OwningRawPtrToSmartPtr(to);
}

LazyContent::~LazyContent()
{
	// linked list so delete only the first one
/*	if (mUpgradors)
	{
		delete mUpgradors;
		mUpgradors = nullptr;
	}*/
}

void RegisterClassToInstanceFactory(KigsCore* core, const std::string& moduleName, KigsID classID, createMethod method)
{
	core->GetInstanceFactory()->RegisterClass(method, classID, moduleName);
}

void CoreModifiable::GetClassNameTree(CoreClassNameTree& classNameTree) 
{ 
	classNameTree.addClassName(CoreModifiable::mClassID, CoreModifiable::mRuntimeType);
}

//! static method : return the set of all root instances of the given type 
std::vector<CMSP> CoreModifiable::GetRootInstances(const KigsID& cid, bool exactTypeOnly)
{
	std::vector<CMSP> instances;
	CoreTreeNode* node = KigsCore::GetTypeNode(cid);
	if (node) node->getRootInstances(instances, !exactTypeOnly);

	return instances;
}


std::vector<CMSP> CoreModifiable::GetInstances(const KigsID& id, bool exact_type_only)
{
	std::vector<CMSP> result;
	CoreTreeNode* node = KigsCore::GetTypeNode(id);
	if (node) node->getInstances(result, !exact_type_only);
	return result;
}

std::vector<CMSP> CoreModifiable::GetInstancesByName(const KigsID& id, const std::string& name, bool exact_type_only)
{
	std::vector<CMSP> result;
	CoreTreeNode* node = KigsCore::GetTypeNode(id);
	if (node) node->getInstancesByName(result, !exact_type_only, name);
	return result;
}

CMSP CoreModifiable::GetFirstInstance(const KigsID& id,  bool exact_type_only)
{
	std::vector<CMSP> result;
	CoreTreeNode* node = KigsCore::GetTypeNode(id);
	if (node) node->getInstances(result, !exact_type_only, true);
	if (result.size()) return result.front();
	return nullptr;
}

CMSP CoreModifiable::GetFirstInstanceByName(const KigsID& id, const std::string& name, bool exact_type_only)
{
	std::vector<CMSP> result;
	CoreTreeNode* node = KigsCore::GetTypeNode(id);
	if (node) node->getInstancesByName(result, !exact_type_only, name, true);
	if (result.size()) return result.front();
	return nullptr;
}

LazyContentLinkedListItemStruct CoreModifiable::InsertForwardPtr(StructLinkedListBase* address)
{
	LazyContentLinkedListItemStruct result = GetLazyContent()->mLinkedListItem;
	GetLazyContent()->mLinkedListItem = LazyContentLinkedListItemStruct::FromAddressAndType(address, LazyContentLinkedListItemStruct::ItemType::ForwardSmartPtrType);
	return result;
}