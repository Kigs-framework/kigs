#include "PrecompiledHeaders.h"

#include "Core.h"
#include "InstanceFactory.h"
#include "ModuleBase.h"
#include "TimeProfiler.h"
#include "DoNothingObject.h"
#include "StepByStepImporter.h"

#include "CoreBaseApplication.h"
#include "XML.h"
#include "XMLReaderFile.h"
#include "NotificationCenter.h"
#include "AsyncRequest.h"
#include "JSonFileParser.h"
#include "LocalizationManager.h"
#include "CoreItemOperator.h"

#include "AttributeModifier.h"

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

//! core unique instance, used also in all dll
KigsCore*			KigsCore::myCoreInstance=0;


kstl::vector<kstl::string>	SplitStringByCharacter(const kstl::string&  a_mstring, char delim)
{
	kstl::vector<kstl::string> elems;
	kstl::string ss = a_mstring;
	int start = 0;
	int pos = 0;
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

// decorators
//! class used to register a decorator
RegisterDecoratorClass::RegisterDecoratorClass(KigsCore* core, KigsID decoratorName, decorateMethod method, decorateMethod undecoratemethod)
{
	core->RegisterDecorator(method,undecoratemethod,decoratorName); 
}

void	KigsCore::RegisterDecorator(decorateMethod method,decorateMethod undecoratemethod, KigsID decoratorName)
{
	if(myDecoratorMap == 0)
	{
		myDecoratorMap=new kigs::unordered_map<KigsID,decorateMethodPair>;
	}

	decorateMethodPair toadd;
	toadd.m_decorate=method;
	toadd.m_undecorate=undecoratemethod;

	(*myDecoratorMap)[decoratorName]=toadd;
}

bool	KigsCore::DecorateInstance(CoreModifiable* cm, KigsID decoratorName)
{
	if(myCoreInstance->myDecoratorMap)
	{
#ifdef KEEP_NAME_AS_STRING
		cm->RegisterDecorator(decoratorName._id_name);
#endif
		auto itfound=(*myCoreInstance->myDecoratorMap).find(decoratorName);
		if( itfound != (*myCoreInstance->myDecoratorMap).end())
		{
			return ((*itfound).second.m_decorate)(cm);
		}

	}
	return false;
}

bool	KigsCore::UnDecorateInstance(CoreModifiable* cm, KigsID decoratorName)
{
	if(myCoreInstance->myDecoratorMap)
	{
#ifdef KEEP_NAME_AS_STRING
		cm->UnRegisterDecorator(decoratorName._id_name);
#endif
		auto itfound=(*myCoreInstance->myDecoratorMap).find(decoratorName);
		if( itfound != (*myCoreInstance->myDecoratorMap).end())
		{
			return ((*itfound).second.m_undecorate)(cm);
		}

	}
	return false;
}


void	KigsCore::addAsyncRequest(AsyncRequest* toAdd)
{
	if (!myCoreInstance->myAsyncRequestList)
	{
		myCoreInstance->myAsyncRequestList = new kstl::vector<AsyncRequest*>;
	}

	myCoreInstance->myAsyncRequestList->push_back(toAdd);
}

void KigsCore::ManageAsyncRequests()
{
	if (myCoreInstance->myAsyncRequestList)
	{
		myCoreInstance->ProtectedManageAsyncRequests();
	}
}

void KigsCore::ProtectedManageAsyncRequests()
{
	// copy, so that a process can add new requests
	kstl::vector<AsyncRequest*> requestlist=*myAsyncRequestList;

	kstl::vector<AsyncRequest*>::const_iterator itstart = requestlist.begin();
	kstl::vector<AsyncRequest*>::const_iterator itend = requestlist.end();

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
	itstart = myAsyncRequestList->begin();
	itend = myAsyncRequestList->end();

	while (itstart != itend)
	{
		if ((*itstart)->isProcessed())
		{
			// processed ? destroy it
			(*itstart)->Destroy();
		}
		else
		{
			requestlist.push_back((*itstart));
		}
		++itstart;
	}
	*myAsyncRequestList = requestlist;
}

void KigsCore::SetCoreApplication(CoreBaseApplication* _instance)
{
	myCoreInstance->myCoreBaseApplication = _instance;
}

CoreBaseApplication* KigsCore::GetCoreApplication()
{
	return myCoreInstance->myCoreBaseApplication;
}

void KigsCore::SetNotificationCenter(NotificationCenter* _instance)
{
	myCoreInstance->myNotificationCenter = _instance;
}

NotificationCenter* KigsCore::GetNotificationCenter()
{
	return myCoreInstance->myNotificationCenter;
}


///////////////////////////////////////////////////////////////

//! Init create the unique instance and all members (memory manager, instance factory...)
void KigsCore::Init()
{
	#ifdef MM_IS_INIT_BY_CORE
	MMManagerinitCallBack();
	#endif
	myCoreInstance=new KigsCore();

	KIGS_SET_MESSAGES_PRINTF(printf);

	myCoreInstance->mySemaphore=0;

	// profilers
	myCoreInstance->myProfilerManager=new GlobalProfilerManager();

	myCoreInstance->SetNotificationCenter(new NotificationCenter());

	//! root node used by inheritance tree
	myCoreInstance->myRootNode=new CoreTreeNode(0, 0u);
MEMORYMANAGEMENT_START
#ifndef _NO_MEMORY_MANAGER_
	//! memory manager
	myCoreInstance->myMemoryInstance=MemoryManager::m_first_memory_manager;
	myCoreInstance->myMemoryManagerOutOfMainAlloc=&MemoryManager::m_OutOfMainAllocCount;
#endif
MEMORYMANAGEMENT_END

	//! instance factory
	myCoreInstance->myInstanceFactory = new InstanceFactory(myCoreInstance);

	//! map for all initialised modules
	myCoreInstance->myModuleBaseInstanceMap = new kigs::unordered_map<KigsID,ModuleBase*>;

	//! map for all singleton
	myCoreInstance->mySingletonMap = new kigs::unordered_map<KigsID,CMSP>;

	myCoreInstance->myMultiThread=new bool;
	(*myCoreInstance->myMultiThread)=false;
#if KIGS_ERROR_LEVEL<=2
	//! init error management
	myCoreInstance->myErrorList=new kstl::vector<kstl::string>;
#endif

	//! map for references
	myCoreInstance->myReferenceMap = new kigs::unordered_map<CoreModifiable*, kstl::vector<CoreModifiableAttribute*> >;

	myCoreInstance->myCoreMainModuleList=new ModuleBase*[8];
	int i=0;
	for(i=01;i<8;i++)
	{
		myCoreInstance->myCoreMainModuleList[i]=0;
	}

	//! declare DoNothingObject (created when unknown object is asked to InstanceFactory)
	DECLARE_FULL_CLASS_INFO(myCoreInstance,DoNothingObject,DoNothingObject,KigsCore)
	DECLARE_FULL_CLASS_INFO(myCoreInstance,StepByStepImporter,StepByStepImporter,KigsCore)
	DECLARE_FULL_CLASS_INFO(myCoreInstance, DictionaryFromJson, DictionaryFromJson, KigsCore)
	DECLARE_FULL_CLASS_INFO(myCoreInstance, DictionaryFromJsonUTF16, DictionaryFromJsonUTF16, KigsCore)
	DECLARE_FULL_CLASS_INFO(myCoreInstance, LocalizationManager, LocalizationManager, KigsCore);

	kstl::vector<SpecificOperator> specificList;
	SpecificOperator toAdd;
	toAdd.myKeyWord = "CoreItemOperatorModifier";
	toAdd.myCreateMethod = &CoreItemOperatorModifier::create;
	specificList.push_back(toAdd);

	CoreItemOperator<kfloat>::ConstructContextMap(myCoreInstance->myCoreItemOperatorCreateMethodMap, &specificList);

/*	AddToAutoRegister({}, "Core");
	AddToAutoRegister({}, "StandAlone");*/
}


kigs::unordered_map<kstl::string, CoreItemOperatorCreateMethod>&	KigsCore::GetDefaultCoreItemOperatorConstructMap()
{
	return myCoreItemOperatorCreateMethodMap;
}

#pragma optimize("", off)

#if KIGSID_CHECK_COLLISIONS
#include <set>
std::set<std::string> KigsIDCollisionStrings;
#endif
//! delete all unique instance members
void KigsCore::Close(bool closeMemoryManager)
{
	

	if(myCoreInstance)
	{
		myCoreInstance->CleanSingletonMap();
		delete myCoreInstance->mySingletonMap;
		myCoreInstance->mySingletonMap = 0;

		delete myCoreInstance->myInstanceFactory;
		myCoreInstance->myInstanceFactory = 0;

		delete myCoreInstance->myModuleBaseInstanceMap;
		myCoreInstance->myModuleBaseInstanceMap = 0;

		delete myCoreInstance->myNotificationCenter;
		delete myCoreInstance->myMultiThread;
		delete myCoreInstance->myProfilerManager;

#if KIGS_ERROR_LEVEL<=2
		myCoreInstance->myErrorList->clear();
		delete myCoreInstance->myErrorList;
#endif

		delete myCoreInstance->myReferenceMap;

		

		delete [] myCoreInstance->myCoreMainModuleList;


		if(myCoreInstance->myDecoratorMap)
		{
			delete myCoreInstance->myDecoratorMap;
			myCoreInstance->myDecoratorMap=0;
		}

		if (myCoreInstance->myAsyncRequestList)
		{
			delete myCoreInstance->myAsyncRequestList;
			myCoreInstance->myAsyncRequestList = 0;
		}


#ifdef _DEBUG
		CoreModifiable::debugPrintfFullTree();
#endif

		delete myCoreInstance->myRootNode;

		myCoreInstance->myTypeNodeMap.clear();

		delete myCoreInstance;

		myCoreInstance=0;


#if KIGSID_CHECK_COLLISIONS
		kigs::unordered_map<u32, std::string> test;
		for (auto& s : GetKigsIDCollisionStrings())
		{
			auto id = fastGetID(s.c_str(), (unsigned int)s.length());
			if (!test.insert({ id, s }).second)
			{
				auto val = test[id];
				__debugbreak();
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
	return myMemoryInstance;
}
#endif
MEMORYMANAGEMENT_END

#pragma optimize("", on)

//! return inheritance tree root node
CoreTreeNode* KigsCore::GetRootNode()
{
	return Instance()->myRootNode;
}

CoreTreeNode* KigsCore::GetTypeNode(const KigsID& id)
{
	auto it = Instance()->myTypeNodeMap.find(id);

	if (it == Instance()->myTypeNodeMap.end())
		return nullptr;

	return it->second;
}


CoreTreeNode* KigsCore::AddToTreeNode(KigsID parent_cid, CoreTreeNode* parent, const kstl::vector<CoreClassNameTree::TwoNames>& branch, const kstl::vector<std::pair<KigsID, RefCountedClass::ModifiableMethod>>& method_table, size_t current_index)
{
	if (!parent)
	{
		KIGS_ERROR("RefCountedClass::addList : ERROR !node", 1);
		return 0;
	}

	// if the class name vector is empty 
	if (current_index == branch.size())
	{
		return parent;
	}

	auto& iClassNames = branch[current_index];

	// get current class ID
	KigsID cid = iClassNames.myClassName;
	if (iClassNames.myRuntimeName != KigsID(""))
	{
		cid = iClassNames.myRuntimeName;
	}

	
	CoreTreeNode* nextNode = 0;
	if (parent->myChildren.find(cid) != parent->myChildren.end())
	{
		if (parent->myChildren[cid] != 0)
		{
			nextNode = parent->myChildren[cid];
		}
	}
	if (nextNode == 0)
	{
		if (cid == parent_cid)
			nextNode = parent;
		else
		{
			nextNode = new CoreTreeNode(parent, cid);
			parent->myChildren[cid] = nextNode;
			for (auto& pair : method_table)
			{
				nextNode->myMethods.insert({ pair.first, ModifiableMethodStruct{ pair.second, "" } });
			}
			KigsCore::Instance()->myTypeNodeMap[cid] = nextNode;
		}
	}
	return AddToTreeNode(cid, nextNode, branch, method_table, current_index+1);
}

CoreTreeNode* KigsCore::RegisterType(const CoreClassNameTree& type_hierarchy, const kstl::vector<std::pair<KigsID, RefCountedClass::ModifiableMethod>>& table)
{
	return AddToTreeNode("", KigsCore::GetRootNode(), type_hierarchy.classNames(), table, 2);
}


void	KigsCore::RegisterMainModuleList(ModuleBase* toRegister, CoreModuleIndex index)
{
	myCoreMainModuleList[(unsigned int)index]=toRegister;
}

//! return unique instance of KigsCore
KigsCore* KigsCore::Instance()
{
	return myCoreInstance;
}

//! called by dll module to init correctly their unique instance (Windows dll => each dll has its own static members)
void	KigsCore::ModuleStaticInit(KigsCore* core)
{
	myCoreInstance=core;

MEMORYMANAGEMENT_START
#ifndef _NO_MEMORY_MANAGER_
	MemoryManager::m_first_memory_manager=core->GetMemoryManager();
	MemoryManager::m_OutOfMainAllocCount=*(core->myMemoryManagerOutOfMainAlloc);
#endif
MEMORYMANAGEMENT_END
}

//! register a module in the map
void KigsCore::ModuleInit(KigsCore* /* core */,ModuleBase* newmodule)
{
	auto* instancemap = myCoreInstance->myModuleBaseInstanceMap;
	(*instancemap)[newmodule->getExactTypeID()]=newmodule;
}

//! return instance factory
InstanceFactory*    KigsCore::GetInstanceFactory()
{
	return myInstanceFactory;
}

void		KigsCore::Connect(CoreModifiable* a, KigsID asignalID, CoreModifiable* b, KigsID bslotID CONNECT_PARAM)
{
	a->Connect(asignalID, b, bslotID CONNECT_PASS_PARAM);
	//Instance()->myConnectionManager->Connect(a, asignalID, b, bslotID CONNECT_PASS_PARAM);
}

void		KigsCore::Disconnect(CoreModifiable* a, KigsID asignalID, CoreModifiable* b, KigsID bslotID)
{
	a->Disconnect(asignalID, b, bslotID);
	//Instance()->myConnectionManager->Disconnect(a, asignalID, b, bslotID);
}

/*
bool		KigsCore::Emit(CoreModifiable* a, KigsID asignalID, kstl::vector<CoreModifiableAttribute*>& params, void* privateParams)
{
	return a->CallEmit(asignalID, params, privateParams);
	//return Instance()->myConnectionManager->Emit(a, asignalID, params, privateParams);
}*/

/*
//! return an instance of the given class type with given name
CoreModifiable* KigsCore::GetInstanceOf(const kstl::string& instancename, KigsID classname)
{
	//! ask instance factory to do the job
	CoreModifiable* instance=Instance()->GetInstanceFactory()->GetInstance(instancename, classname);

	//! if instance factory fail then create a DoNothingObject (and print debug messages)
	if(!instance)
	{
#ifdef _DEBUG
#ifdef KEEP_NAME_AS_STRING
		printf("unknown object : %s of type %s \n", instancename.c_str(), classname._id_name.c_str());
#else
		printf("unknown object : %s \n", instancename.c_str());
#endif
#endif
		instance=Instance()->GetInstanceFactory()->GetInstance(instancename, "DoNothingObject");
#ifdef _DEBUG
#ifdef KEEP_NAME_AS_STRING
		if (!instance)
			printf("ALLOCATION ERROR : %s \n",classname._id_name.c_str());
#endif
#endif
	}

	return instance;
}
*/

void	KigsCore::GetSemaphore()
{
	if (mySemaphore)
	{
		//			printf("get Sema\n");
		mySemaphore->addItem(mySemaphore);
	}
}


void KigsCore::ReleaseSemaphore()
{
	if (mySemaphore)
	{
		mySemaphore->removeItem(mySemaphore);
		//			printf("release Sema\n");
	}
}

CMSP& KigsCore::GetSingleton(const KigsID& classname)
{
	CMSP* found=nullptr;
	Instance()->GetSemaphore();

	//! search for an already existing instance
	auto& singletonmap = *(Instance()->mySingletonMap);
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
	kstl::string instancename("Singleton_");
	#ifdef KEEP_NAME_AS_STRING
	instancename += classname._id_name;
	#else
	char buffer[128];
	sprintf(buffer,"%d", classname._id);
	instancename += buffer;
	#endif

	CMSP newone= OwningRawPtrToSmartPtr(Instance()->GetInstanceFactory()->GetInstance(instancename,classname));

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

	return Instance()->myNullPtr;
}

void	KigsCore::CleanSingletonMap()
{
	Instance()->GetSemaphore();
	auto it = mySingletonMap->begin();
	//! do nothing if no instance of the class
	while(it!=mySingletonMap->end())
	{
		CMSP found = (*it).second;
		mySingletonMap->erase(it);

		if(found)
		{
			Instance()->ReleaseSemaphore();
			found = nullptr; // destroy pointed object
			Instance()->GetSemaphore();
		}
		it = mySingletonMap->begin();
	}

	Instance()->ReleaseSemaphore();
}

void KigsCore::ReleaseSingleton(KigsID classname)
{
	Instance()->GetSemaphore();

	//! search for an already existing instance
	auto& singletonmap=*(Instance()->mySingletonMap);
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
ModuleBase*		KigsCore::GetModule(const KigsID& classname)
{
	auto* instancemap = Instance()->myModuleBaseInstanceMap;
	if (!instancemap)
		return NULL;

	auto Iter = instancemap->find(classname);
	if (Iter==instancemap->end())
		return NULL;

	return Iter->second;
}

bool KigsCore::IsInit()
{
	return (myCoreInstance!=0);
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
	for(i=0;i<myErrorList->size();i++)
	{
		kstl::string errorstring=(*myErrorList)[(unsigned int)i];
		errorstring+="\n";
#ifdef WUP	
		kigsprintf(errorstring.c_str());
#else
		KigsMsgPrintf(errorstring.c_str());
#endif
	}

	(*myErrorList).clear();
}

void	KigsCore::AddError(const char* errfile,long errline,const kstl::string& errorstring,int errorLevel)
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
	kstl::string Error=KigsPrefixError[errorLevel];
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
void	KigsCore::ProtectedAddError(const kstl::string& Error)
{
	// check size and dump messages if size is critical
	int total_size=0;
	unsigned int i;
	for(i=0;i<(*myErrorList).size();i++)
	{
		total_size+=((int)(*myErrorList).size())+2;
	}

	total_size+=((int)(Error.size()))+2;

	if(total_size>=KIGS_ERROR_BUFFER_SIZE)
	{
		DumpMessageBuffer();
	}

	(*myErrorList).push_back(Error);
}

#endif
#if KIGS_ERROR_LEVEL<=1
void	KigsCore::AddWarning(const char* errfile,long errline,const kstl::string& errorstring,int errorLevel)
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
	kstl::string Error=KigsPrefixWarning[errorLevel];
	Error+=errfile;
	Error+="(";
	Error+=lineToChar;
	Error+=") : ";
	Error+=errorstring;

	Instance()->ProtectedAddError(Error);
}
#endif
#if KIGS_ERROR_LEVEL==0

void	KigsCore::AddMessage(const kstl::string& errorstring)
{
	kstl::string Error=KigsPrefixMessage;
	Error+=errorstring;

	Instance()->ProtectedAddError(Error);
}
#endif


//! return true if thread module is init
bool	KigsCore::IsMultiThread()
{
	return (*Instance()->myMultiThread);
}

void	KigsCore::SetMultiThread()
{
	(*Instance()->myMultiThread)=true;
MEMORYMANAGEMENT_START
#ifndef _NO_MEMORY_MANAGER_
	Instance()->GetMemoryManager()->mySemaphore=(void*)KigsCore::GetInstanceOf("MemManagerSemaphore","Semaphore");
#endif
MEMORYMANAGEMENT_END
	Instance()->mySemaphore=KigsCore::GetInstanceOf("CoreSemaphore","Semaphore");
}

void	KigsCore::CloseMultiThread()
{
	(*Instance()->myMultiThread)=false;
MEMORYMANAGEMENT_START
#ifndef _NO_MEMORY_MANAGER_
	if(myCoreInstance->GetMemoryManager()->mySemaphore)
	{
		CoreModifiable* tmp=(CoreModifiable*)myCoreInstance->GetMemoryManager()->mySemaphore;
		myCoreInstance->GetMemoryManager()->mySemaphore=0;
		tmp->Destroy();
	}
#endif
MEMORYMANAGEMENT_END
	if(myCoreInstance->mySemaphore)
	{
		myCoreInstance->mySemaphore=nullptr;
	
	}
}

// define MemoryManager lock/unlock here
// because methods use KigsCore methods, and linker don't want MemoryManager to use core methods
/*
void	MemoryManager::Lock() const
{
	if(mySemaphore)
	{
		CoreModifiable*	semaphore=(CoreModifiable*)mySemaphore;
		semaphore->addItem(semaphore);
	}
}

void	MemoryManager::UnLock() const
{
	if(mySemaphore)
	{
		CoreModifiable*	semaphore=(CoreModifiable*)mySemaphore;
		semaphore->removeItem(semaphore);
	}
}
*/

GlobalProfilerManager* KigsCore::GetProfileManager()
{
	return 	myCoreInstance->myProfilerManager;
}

bool	KigsCore::ParseXml(const kstl::string& filename,CoreModifiable*	delegateObject,const char* force_as_format)
{
	return XML::ReadFile(filename,delegateObject,force_as_format);
}

bool	KigsCore::ParseXml(char* buffer,CoreModifiable*	delegateObject,unsigned long buffsize,char* encoding)
{
	return XMLReaderFile::ReadFile(buffer,delegateObject,buffsize,encoding);
}

//! manage post destruction
void KigsCore::ManagePostDestruction()
{
	std::lock_guard<std::mutex> lk{ myPostDestructionListMutex };
	for(auto c : myPostDestructionList)
		c->Destroy();
	myPostDestructionList.clear();
}
	
void KigsCore::AddToPostDestroyList(CoreModifiable* c)
{
	std::lock_guard<std::mutex> lk{ myPostDestructionListMutex };
	myPostDestructionList.push_back(c);
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