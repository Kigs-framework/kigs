#include "PrecompiledHeaders.h"

#include "Core.h"
#include "InstanceFactory.h"
#include "ModuleBase.h"
#include "TimeProfiler.h"
#include "DoNothingObject.h"

#include "CoreBaseApplication.h"
#include "XML.h"
#include "XMLReaderFile.h"
#include "NotificationCenter.h"
#include "AsyncRequest.h"
#include "JSonFileParser.h"
#include "LocalizationManager.h"
#include "CoreItemOperator.h"

#include "MiniInstanceFactory.h"

#ifdef _KIGS_ONLY_STATIC_LIB_
#include "Platform/Core/PlatformCore.h"
#endif

#ifndef MEMSIZE_FOR_MEMORY_MANAGER
static void * MM_base_adress;
#endif

#if KIGS_ERROR_LEVEL<=2
KIGS_PREFIX_ERROR
#endif

#if KIGS_ERROR_LEVEL<=1
KIGS_PREFIX_WARNING
#endif

#if KIGS_ERROR_LEVEL==0
KIGS_PREFIX_MESSAGE
#endif

#include <stdio.h>
#include <algorithm>

using namespace Kigs::Core;
//! core unique instance, used also in all dll
KigsCore*			KigsCore::mCoreInstance=0;


std::vector<std::string>	Kigs::Core::SplitStringByCharacter(const std::string&  a_mstring, char delim)
{
	std::vector<std::string> elems;
	std::string ss = a_mstring;
	size_t start = 0;
	size_t pos = 0;
	while (!a_mstring.empty())
	{
		pos = ss.find_first_of(delim, pos + 1);
		elems.push_back(ss.substr(start, pos - start));
		start = pos + 1;
		if (pos == -1)
			break;
	}

	return elems;
}


void	KigsCore::addAsyncRequest(SP<AsyncRequest> toAdd)
{
	if (!mCoreInstance->mAsyncRequestList)
	{
		mCoreInstance->mAsyncRequestList = new std::vector<SP<AsyncRequest>>;
	}
	mCoreInstance->mAsyncRequestList->push_back(toAdd);
}

void KigsCore::ManageAsyncRequests()
{
	if (mCoreInstance->mAsyncRequestList)
	{
		mCoreInstance->ProtectedManageAsyncRequests();
	}
}

void KigsCore::ProtectedManageAsyncRequests()
{
	// copy, so that a process can add new requests
	std::vector<SP<AsyncRequest>> requestlist=*mAsyncRequestList;

	auto itstart = requestlist.begin();
	auto itend = requestlist.end();

	while (itstart != itend)
	{
		if (((*itstart)->isDone()) && !((*itstart)->isProcessed()))
		{
			// process
			(*itstart)->Process();
		}
		++itstart;
	}

	// create new list with unprocessed request
	requestlist.clear();
	itstart = mAsyncRequestList->begin();
	itend = mAsyncRequestList->end();

	while (itstart != itend)
	{
		if (!(*itstart)->isProcessed())
		{
			requestlist.push_back((*itstart));
		}
		++itstart;
	}
	*mAsyncRequestList = requestlist;
}

void KigsCore::SetCoreApplication(CoreBaseApplication* _instance)
{
	mCoreInstance->mCoreBaseApplication = _instance;
}

CoreBaseApplication* KigsCore::GetCoreApplication()
{
	return mCoreInstance->mCoreBaseApplication;
}

void KigsCore::SetNotificationCenter(NotificationCenter* _instance)
{
	mCoreInstance->mNotificationCenter = _instance;
}

NotificationCenter* KigsCore::GetNotificationCenter()
{
	return mCoreInstance->mNotificationCenter;
}

///////////////////////////////////////////////////////////////

//! Init create the unique instance and all members (memory manager, instance factory...)
void KigsCore::Init()
{
	#ifdef MM_IS_INIT_BY_CORE
	MMManagerinitCallBack();
	#endif
	mCoreInstance=new KigsCore();

	KIGS_SET_MESSAGES_PRINTF(printf);

	mCoreInstance->mSemaphore=nullptr;

	// profilers
	mCoreInstance->mProfilerManager=new Time::GlobalProfilerManager();

	mCoreInstance->SetNotificationCenter(new NotificationCenter());

	//! root node used by inheritance tree
	mCoreInstance->mRootNode=new CoreTreeNode(0, 0u);
MEMORYMANAGEMENT_START
#ifndef _NO_MEMORY_MANAGER_
	//! memory manager
	mCoreInstance->mMemoryInstance=MemoryManager::m_first_memory_manager;
	mCoreInstance->mMemoryManagerOutOfMainAlloc=&MemoryManager::m_OutOfMainAllocCount;
#endif
MEMORYMANAGEMENT_END

	//! instance factory
	mCoreInstance->mInstanceFactory = new InstanceFactory(mCoreInstance);

	//! map for all initialised modules
	mCoreInstance->mModuleBaseInstanceMap = new unordered_map<KigsID,SP<ModuleBase>>;

	//! map for all singleton
	mCoreInstance->mSingletonMap = new unordered_map<KigsID,CMSP>;

	mCoreInstance->mMultiThread=new bool;
	(*mCoreInstance->mMultiThread)=false;
#if KIGS_ERROR_LEVEL<=2
	//! init error management
	mCoreInstance->mErrorList=new std::vector<std::string>;
#endif

	mCoreInstance->mCoreMainModuleList=new ModuleBase*[8];
	int i=0;
	for(i=01;i<8;i++)
	{
		mCoreInstance->mCoreMainModuleList[i]=0;
	}

	//! declare DoNothingObject (created when unknown object is asked to InstanceFactory)
	DECLARE_FULL_CLASS_INFO(mCoreInstance,DoNothingObject,DoNothingObject,KigsCore)
	DECLARE_FULL_CLASS_INFO(mCoreInstance, DictionaryFromJson, DictionaryFromJson, KigsCore)
	DECLARE_FULL_CLASS_INFO(mCoreInstance, DictionaryFromJsonUTF16, DictionaryFromJsonUTF16, KigsCore)
	DECLARE_FULL_CLASS_INFO(mCoreInstance, LocalizationManager, LocalizationManager, KigsCore);
	DECLARE_FULL_CLASS_INFO(mCoreInstance, MiniInstanceFactory, MiniInstanceFactory, KigsCore)

	std::vector<SpecificOperator> specificList;
	CoreItemOperator<float>::ConstructContextMap(mCoreInstance->mCoreItemOperatorCreateMethodMap, &specificList);

	CMSP createUpgradorFactory = GetInstanceOf("UpgradorFactory", "MiniInstanceFactory");
	mCoreInstance->mUpgradorFactory = ((MiniInstanceFactory*)createUpgradorFactory.get())->SharedFromThis();
}


unordered_map<std::string, CoreItemOperatorCreateMethod>&	KigsCore::GetDefaultCoreItemOperatorConstructMap()
{
	return mCoreItemOperatorCreateMethodMap;
}

#pragma optimize("", off)

#if KIGSID_CHECK_COLLISIONS
#include <set>
std::set<std::string> KigsIDCollisionStrings;
#endif
//! delete all unique instance members
void KigsCore::Close(bool closeMemoryManager)
{
	

	if(mCoreInstance)
	{
		mCoreInstance->ManagePostDestruction();

		mCoreInstance->mUpgradorFactory = nullptr;

		mCoreInstance->CleanSingletonMap();
		delete mCoreInstance->mSingletonMap;
		mCoreInstance->mSingletonMap = 0;

		delete mCoreInstance->mInstanceFactory;
		mCoreInstance->mInstanceFactory = 0;

		delete mCoreInstance->mModuleBaseInstanceMap;
		mCoreInstance->mModuleBaseInstanceMap = 0;

		delete mCoreInstance->mNotificationCenter;
		delete mCoreInstance->mMultiThread;
		delete mCoreInstance->mProfilerManager;

#if KIGS_ERROR_LEVEL<=2
		mCoreInstance->mErrorList->clear();
		delete mCoreInstance->mErrorList;
#endif
		delete [] mCoreInstance->mCoreMainModuleList;

		if (mCoreInstance->mAsyncRequestList)
		{
			delete mCoreInstance->mAsyncRequestList;
			mCoreInstance->mAsyncRequestList = 0;
		}
		
		mCoreInstance->mSemaphore = nullptr;

#ifdef _DEBUG
		CoreModifiable::debugPrintfFullTree();
#endif

		delete mCoreInstance->mRootNode;
		
		mCoreInstance->mTypeNodeMap.clear();

		delete mCoreInstance;

		mCoreInstance=0;

#ifdef GenericRefCountedBaseClassLeakCheck
		if (AllObjects.size())
		{
#ifdef WIN32			
			__debugbreak(); 
			for (auto obj : AllObjects)
			{
				kigsprintf("leaked %s\n", typeid(obj).name());
			}
#else
			for (auto obj : AllObjects)
			{
				kigsprintf("leaked %p\n", obj);
			}
#endif	

		}
#endif


#if KIGSID_CHECK_COLLISIONS
		unordered_map<u32, std::string> test;
		for (auto& s : GetKigsIDCollisionStrings())
		{
			auto id = fastGetID(s.c_str(), (unsigned int)s.length());
			if (!test.insert({ id, s }).second)
			{
				auto val = test[id];
#ifdef WIN32			
				__debugbreak();
#endif
			}
		}
#endif


MEMORYMANAGEMENT_START
#ifndef _NO_MEMORY_MANAGER_
		if(closeMemoryManager)
		{
#ifdef MM_DEBUG
	the_mem_manager.MMDumpBlockList();
	the_mem_manager.GetNbAllocatedBlocks();
#endif

			the_mem_manager.MMCloseAll();

#ifndef MEMSIZE_FOR_MEMORY_MANAGER
			free(MM_base_adress);
			MM_base_adress=0;
#endif
		}
#endif
MEMORYMANAGEMENT_END
	}
}
MEMORYMANAGEMENT_START
#ifndef _NO_MEMORY_MANAGER_
//! return memory manager
MemoryManager*	KigsCore::GetMemoryManager()
{
	return mMemoryInstance;
}
#endif
MEMORYMANAGEMENT_END

#pragma optimize("", on)

//! return inheritance tree root node
CoreTreeNode* KigsCore::GetRootNode()
{
	return Instance()->mRootNode;
}

CoreTreeNode* KigsCore::GetTypeNode(const KigsID& id)
{
	auto it = Instance()->mTypeNodeMap.find(id);

	if (it == Instance()->mTypeNodeMap.end())
		return nullptr;

	return it->second;
}

CoreTreeNode* KigsCore::AddToTreeNode(KigsID parent_cid, CoreTreeNode* parent, const std::vector<CoreClassNameTree::TwoNames>& branch, const std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& method_table, size_t current_index)
{
	if (!parent)
	{
		KIGS_ERROR("KigsCore::AddToTreeNode : ERROR !node", 1);
		return 0;
	}

	// if the class name vector is empty 
	if (current_index == branch.size())
	{
		return parent;
	}

	auto& iClassNames = branch[current_index];

	// get current class ID
	KigsID cid = iClassNames.mClassName;
	if (iClassNames.mRuntimeName != KigsID(""))
	{
		cid = iClassNames.mRuntimeName;
	}

	
	CoreTreeNode* nextNode = 0;
	if (parent->mChildren.find(cid) != parent->mChildren.end())
	{
		if (parent->mChildren[cid] != 0)
		{
			nextNode = parent->mChildren[cid];
		}
	}
	if (nextNode == 0)
	{
		if (cid == parent_cid)
			nextNode = parent;
		else
		{
			nextNode = new CoreTreeNode(parent, cid);
			parent->mChildren[cid] = nextNode;
			for (auto& pair : method_table)
			{
				ModifiableMethodStruct toAdd("", pair.second);
				nextNode->mMethods.insert({ pair.first, toAdd });
			}
			KigsCore::Instance()->mTypeNodeMap[cid] = nextNode;
		}
	}
	return AddToTreeNode(cid, nextNode, branch, method_table, current_index+1);
}

CoreTreeNode* KigsCore::RegisterType(const CoreClassNameTree& type_hierarchy, const std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& table)
{
	return AddToTreeNode("", KigsCore::GetRootNode(), type_hierarchy.classNames(), table, 0);
}


void	KigsCore::RegisterMainModuleList(ModuleBase* toRegister, CoreModuleIndex index)
{
	mCoreMainModuleList[(unsigned int)index]=toRegister;
}

//! return unique instance of KigsCore
KigsCore* KigsCore::Instance()
{
	return mCoreInstance;
}

//! called by dll module to init correctly their unique instance (Windows dll => each dll has its own static members)
void	KigsCore::ModuleStaticInit(KigsCore* core)
{
	mCoreInstance=core;

MEMORYMANAGEMENT_START
#ifndef _NO_MEMORY_MANAGER_
	MemoryManager::m_first_memory_manager=core->GetMemoryManager();
	MemoryManager::m_OutOfMainAllocCount=*(core->mMemoryManagerOutOfMainAlloc);
#endif
MEMORYMANAGEMENT_END
}

//! register a module in the map
void KigsCore::ModuleInit(KigsCore* /* core */,ModuleBase* newmodule)
{
	auto* instancemap = mCoreInstance->mModuleBaseInstanceMap;
	(*instancemap)[newmodule->getExactTypeID()] = newmodule->SharedFromThis();
}

//! return instance factory
InstanceFactory*    KigsCore::GetInstanceFactory()
{
	return mInstanceFactory;
}

void		KigsCore::Connect(CoreModifiable* a, KigsID asignalID, CoreModifiable* b, KigsID bslotID CONNECT_PARAM)
{
	a->Connect(asignalID, b->SharedFromThis(), bslotID CONNECT_PASS_PARAM);
}

void		KigsCore::Disconnect(CoreModifiable* a, KigsID asignalID, CoreModifiable* b, KigsID bslotID)
{
	a->Disconnect(asignalID, b->SharedFromThis(), bslotID);
}

void	KigsCore::GetSemaphore()
{
	if (mSemaphore)
	{
		mSemaphore->GetMutex().lock();
	}
}


void KigsCore::ReleaseSemaphore()
{
	if (mSemaphore)
	{
		mSemaphore->GetMutex().unlock();
	}
}

CMSP KigsCore::GetSingleton(const KigsID& classname)
{
	CMSP* found=nullptr;
	Instance()->GetSemaphore();

	//! search for an already existing instance
	auto& singletonmap = *(Instance()->mSingletonMap);
	auto	testFound = singletonmap.find(classname);
	if(testFound != singletonmap.end())
	{
		found= &(*testFound).second;
	}
	Instance()->ReleaseSemaphore();
	//! and return it if found
	if(found)
	{
		return *found;
	}

	//! else create a new instance
	std::string instancename("Singleton_");
	#ifdef KEEP_NAME_AS_STRING
	instancename += classname._id_name;
	#else
	char buffer[128];
	sprintf(buffer,"%d", classname._id);
	instancename += buffer;
	#endif

	CMSP newone= Instance()->GetInstanceFactory()->GetInstance(instancename,classname);

	Instance()->GetSemaphore();
	
	if (newone)
	{
		singletonmap.insert({ classname, newone });
		testFound = singletonmap.find(classname);
		found = &(*testFound).second;
	}
	
	Instance()->ReleaseSemaphore();
	
	if(found)
		return *found;

	return nullptr;
}

void	KigsCore::CleanSingletonMap()
{
	Instance()->GetSemaphore();
	auto it = mSingletonMap->begin();
	//! do nothing if no instance of the class
	while(it!=mSingletonMap->end())
	{
		CMSP found = (*it).second;
		mSingletonMap->erase(it);

		if(found)
		{
			Instance()->ReleaseSemaphore();
			found = nullptr; // destroy pointed object
			Instance()->GetSemaphore();
		}
		it = mSingletonMap->begin();
	}

	Instance()->ReleaseSemaphore();
}

void KigsCore::ReleaseSingleton(KigsID classname)
{
	Instance()->GetSemaphore();

	//! search for an already existing instance
	auto& singletonmap=*(Instance()->mSingletonMap);
	auto it = singletonmap.find(classname);
	//! do nothing if no instance of the class
	if(it==singletonmap.end())
	{
		Instance()->ReleaseSemaphore();
		return;
	}
	CMSP found = (*it).second;
	singletonmap.erase(it);

	Instance()->ReleaseSemaphore();

	found=nullptr;
}

//! get the registered module given its name
SP<ModuleBase>		KigsCore::GetModule(const KigsID& classname)
{
	auto* instancemap = Instance()->mModuleBaseInstanceMap;
	if (!instancemap)
		return nullptr;

	auto Iter = instancemap->find(classname);
	if (Iter==instancemap->end())
		return nullptr;

	return Iter->second;
}

void KigsCore::RemoveModule(const std::string& classname)
{
	auto* instancemap = Instance()->mModuleBaseInstanceMap;
	if (!instancemap)
		return;
	instancemap->erase(classname);
}

bool KigsCore::IsInit()
{
	return (mCoreInstance!=0);
}

#if KIGS_ERROR_LEVEL<=2

	// use KIGS_DUMP_MESSAGES
void	KigsCore::DumpMessageBuffer()
{
	Instance()->ProtectedDumpMessageBuffer();
}

void	KigsCore::SetMessagePrintf(int (*pfunc)(const char *format... ))
{
	Instance()->KigsMsgPrintf=pfunc;
}

void KigsCore::ProtectedDumpMessageBuffer()
{
	unsigned int i;
	for(i=0;i<mErrorList->size();i++)
	{
		std::string errorstring=(*mErrorList)[(unsigned int)i];
		errorstring+="\n";
#ifdef WUP	
		kigsprintf(errorstring.c_str());
#else
		KigsMsgPrintf(errorstring.c_str());
#endif
	}

	(*mErrorList).clear();
}

void	KigsCore::AddError(const char* errfile,long errline,const std::string& errorstring,int errorLevel)
{
	if(errorLevel>2)
	{
		errorLevel=2;
	}
	if(errorLevel<0)
	{
		errorLevel=0;
	}
	char	lineToChar[128];
	sprintf(lineToChar,"%ld",errline);
	std::string Error=KigsPrefixError[errorLevel];
	Error+=errfile;
	Error+="(";
	Error+=lineToChar;
	Error+=") : ";
	Error+=errorstring;

	Instance()->ProtectedAddError(Error);

#ifdef STOP_ON_ERROR
	sprintf(lineToChar,"%d",__LINE__);
	Error=KigsPrefixError[errorLevel];
	Error+=__FILE__;
	Error+="(";
	Error+=lineToChar;
	Error+=") : ";
	Error+="Program stopped on error";

	Instance()->ProtectedAddError(Error);

	DumpMessageBuffer();

	while(1)
	{
	}

#endif
}

// same code for error, warning and messages :
// message level is 0
// warning level is from 1 to 4
// error level is from 5 to 9
void	KigsCore::ProtectedAddError(const std::string& Error)
{
	// check size and dump messages if size is critical
	int total_size=0;
	unsigned int i;
	for(i=0;i<(*mErrorList).size();i++)
	{
		total_size+=((int)(*mErrorList).size())+2;
	}

	total_size+=((int)(Error.size()))+2;

	if(total_size>=KIGS_ERROR_BUFFER_SIZE)
	{
		DumpMessageBuffer();
	}

	(*mErrorList).push_back(Error);
}

#endif
#if KIGS_ERROR_LEVEL<=1
void	KigsCore::AddWarning(const char* errfile,long errline,const std::string& errorstring,int errorLevel)
{
	if(errorLevel>2)
	{
		errorLevel=2;
	}
	if(errorLevel<0)
	{
		errorLevel=0;
	}
	char	lineToChar[128];
	sprintf(lineToChar,"%ld",errline);
	std::string Error=KigsPrefixWarning[errorLevel];
	Error+=errfile;
	Error+="(";
	Error+=lineToChar;
	Error+=") : ";
	Error+=errorstring;

	Instance()->ProtectedAddError(Error);
}
#endif
#if KIGS_ERROR_LEVEL==0

void	KigsCore::AddMessage(const std::string& errorstring)
{
	std::string Error=KigsPrefixMessage;
	Error+=errorstring;

	Instance()->ProtectedAddError(Error);
}
#endif


//! return true if thread module is init
bool	KigsCore::IsMultiThread()
{
	return (*Instance()->mMultiThread);
}

void	KigsCore::SetMultiThread()
{
	(*Instance()->mMultiThread)=true;
MEMORYMANAGEMENT_START
#ifndef _NO_MEMORY_MANAGER_
	Instance()->GetMemoryManager()->mSemaphore=(void*)KigsCore::GetInstanceOf("MemManagerSemaphore","Semaphore");
#endif
MEMORYMANAGEMENT_END
	Instance()->mSemaphore=KigsCore::GetInstanceOf("CoreSemaphore","Semaphore");
}

void	KigsCore::CloseMultiThread()
{
	(*Instance()->mMultiThread)=false;
MEMORYMANAGEMENT_START
#ifndef _NO_MEMORY_MANAGER_
	if(mCoreInstance->GetMemoryManager()->mSemaphore)
	{
		CoreModifiable* tmp=(CoreModifiable*)mCoreInstance->GetMemoryManager()->mSemaphore;
		mCoreInstance->GetMemoryManager()->mSemaphore=0;
		tmp->Destroy();
	}
#endif
MEMORYMANAGEMENT_END
	if(mCoreInstance->mSemaphore)
	{
		// get a ref on mCoreInstance->mSemaphore
		CMSP tmp = mCoreInstance->mSemaphore;
		// so now, we can set mCoreInstance->mSemaphore to nullptr without locking semaphore
		mCoreInstance->mSemaphore=nullptr;
	}
}

Kigs::Time::GlobalProfilerManager* KigsCore::GetProfileManager()
{
	return 	mCoreInstance->mProfilerManager;
}

bool	KigsCore::ParseXml(const std::string& filename,CoreModifiable*	delegateObject,const char* force_as_format)
{
	return Kigs::Xml::XML::ReadFile(filename,delegateObject,force_as_format);
}

bool	KigsCore::ParseXml(char* buffer,CoreModifiable*	delegateObject,unsigned long buffsize,char* encoding)
{
	return Kigs::Xml::XMLReaderFile::ReadFile(buffer,delegateObject,buffsize,encoding);
}

//! manage post destruction
void KigsCore::ManagePostDestruction()
{
	std::lock_guard<std::mutex> lk{ mPostDestructionListMutex };
	for (auto it = mPostDestructionList.begin(); it != mPostDestructionList.end();)
	{
		if (it->second.use_count() == 1)
		{
			it = mPostDestructionList.erase(it);
		}
		else
			++it;
	}
}

void KigsCore::AddToPostDestroy(CMSP obj)
{
	std::lock_guard lk{ mPostDestructionListMutex };
	mPostDestructionList[obj.get()] = obj;
}

void KigsCore::RemoveFromPostDestroy(CMSP obj)
{
	std::lock_guard lk{ mPostDestructionListMutex };
	mPostDestructionList.erase(obj.get());
}

// utility func
void Kigs::Core::replaceAll(std::string& source, const std::string& from, const std::string& to)
{
	std::string newString;
	newString.reserve(source.length());  // avoids a few memory allocations

	std::string::size_type lastPos = 0;
	std::string::size_type findPos;

	while (std::string::npos != (findPos = source.find(from, lastPos)))
	{
		newString.append(source, lastPos, findPos - lastPos);
		newString += to;
		lastPos = findPos + from.length();
	}

	// Care for the rest after last occurrence
	newString += source.substr(lastPos);

	source.swap(newString);
}

MEMORYMANAGEMENT_START
#ifndef _NO_MEMORY_MANAGER_
////////////////////////////END OF FILE : not inlined


//! memory manager init
#ifndef MM_USE_CUSTOM_CALLBACK
// define only if not already defined
#define MEMSIZE_FOR_MEMORY_MANAGER	0x02000000		// 32MB
//! callback called the first time memory manager is needed
void MMManagerinitCallBack(void)
{
	MM_base_adress = (void *) malloc( MEMSIZE_FOR_MEMORY_MANAGER );
	the_mem_manager.MMSetPrintFunction( printf );
	the_mem_manager.MMInitAll( MM_base_adress, MEMSIZE_FOR_MEMORY_MANAGER );
}
#else
extern void MMManagerinitCallBack(void);
#endif
#endif
MEMORYMANAGEMENT_END