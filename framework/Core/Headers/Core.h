
/**
 @mainpage

Kigs framework is a C++ modular multi-purpose cross-platform basement framework for Rapid Application Development.

The main goal is to be able to develop different types of applications (games, simulators, viewers...) quickly, in a totally independent manner.

GitHub project : https://github.com/Kigs-framework/kigs
 */

 /*! \defgroup Core Core module
  *  Base classes and mechanisms used by all other classes
  */

#ifndef _CORE_H_
#define _CORE_H_

/*
#if defined(_DEBUG) && !defined(KIGS_TOOLS)
#define KIGS_TOOLS
#endif
*/

#include "CoreModifiable.h"
#include "InstanceFactory.h"
#include "CoreTreeNode.h"

class MiniInstanceFactory;
class CoreModifiableAttribute;
class InstanceFactory;
class ModuleBase;
class CoreTreeNode;
class GlobalProfilerManager;
class CoreBaseApplication;
class NotificationCenter;
class AsyncRequest;
class CoreItem;
class CoreVector;


typedef     CoreVector* (*CoreItemOperatorCreateMethod)();

// keep a fast array access on main generic modules (using enum as index)
enum CoreModuleIndex
{
	TimerModuleCoreIndex			=	0,
	FileManagerModuleCoreIndex		=	1,
	GUIModuleCoreIndex				=	2,
	InputModuleCoreIndex			=	3,
	SceneGraphModuleCoreIndex		=	4,
	CoreAnimationModuleCoreIndex	=	5,
	RendererModuleCoreIndex			=	6,
	Layer2DModuleCoreIndex			=	7
} ;

// decorator management
typedef     bool (*DecorateMethod)(CoreModifiable* toDecorate);
class RegisterDecoratorClass
{
public:
	/**
	 * \fn 		RegisterDecorator(KigsCore* core,kstl::string decoratorName,decorateMethod method);
	 * \brief	register a decorator in core
	 * \core : KigsCore instance
	 * \decoratorName : name of the decorator class
	 * \method : static decorator method
	 */
	RegisterDecoratorClass(KigsCore* core, KigsID decoratorName, DecorateMethod method, DecorateMethod undecoratemethod);

	/**
	 * \brief	destructor
	 * \fn 		~RegisterDecorator();
	 */
	~RegisterDecoratorClass() {}
};

#define DECLARE_FULL_DECORATOR_INFO(core,decoratorClass) \
    RegisterDecoratorClass RegisterDecorator##decoratorClass(core, #decoratorClass, &decoratorClass::Decorate,&decoratorClass::UnDecorate); 

#define REGISTER_UPGRADOR(name) \
	{KigsCore::Instance()->GetUpgradorFactory()->RegisterNewClass<name>(#name);}


extern kstl::vector<kstl::string>	SplitStringByCharacter(const kstl::string& a_mstring, char a_value);


// ****************************************
// * KigsCore class
// * --------------------------------------
/**
 * \file	Core.h
 * \class	KigsCore
 * \ingroup Core	
 *
 * Singleton class used to manage all others.
 */
// ****************************************
	
class KigsCore
{
public:
	/**
	 * \fn			static void Init();
	 * Init KigsCore singleton and all its members 
	 */
	static void Init();

	/**
	 * \fn			static void ModuleInit(KigsCore* core,ModuleBase* module); 
	 * Called by modules to be registered in KigsCore
	 * \param		core : link to the core, NOT NULL
	 * \param		module : module to register, CAN BE NULL
	 */
	static void ModuleInit(KigsCore* core,ModuleBase* module);

	/**
	 * \fn			static void Close(bool closeMemoryManager=true);
	 * Close KigsCore 
	 * \param		closeMemoryManager : set to true if the memory manager needs to be closed too ( in case Kigs framework is build with MemoryManager )
	 */
	static void Close(bool closeMemoryManager=true);

	/**
	 * \fn			static KigsCore* Instance();
	 * Get the KigsCore instance (Design Pattern Singleton)
	 * \return		the unique instance of the KigsCore
	 */
	static KigsCore* Instance();

	friend class CoreBaseApplication;

	/**
	* \fn		 static CoreBaseApplication* GetCoreApplication();
	* Get the GetCoreApplication instance (Design Pattern Singleton)
	* \return		the unique instance of the CoreBaseApplication or null if no application is defined
	*/
	static CoreBaseApplication* GetCoreApplication();
	
	/**
	* \fn		static NotificationCenter* GetNotificationCenter();	
	* Get the NotificationCenter instance (Design Pattern Singleton)
	* \return	the unique instance of the NotificationCenter 
	*/
	static NotificationCenter* GetNotificationCenter();			

	/**
	 * \fn			InstanceFactory*    GetInstanceFactory();
	 * Get the instance factory instance (Design Pattern Singleton)
	 * \return		the unique instance factory
	 */
	InstanceFactory* GetInstanceFactory();

	/**
	 * \fn			MiniInstanceFactory* GetUpgradorFactory() const
	 * Get the MiniInstanceFactory used by Upgrador mechanism
	 * \return		the MiniInstanceFactory
	 */
	MiniInstanceFactory* GetUpgradorFactory() const
	{
		return mUpgradorFactory.get();
	}

	// create connection between a signal and a slot 
	static void		Connect(CoreModifiable* a, KigsID asignalID, CoreModifiable* b, KigsID bslotID CONNECT_PARAM_DEFAULT);

	template<typename T>
	static void		Connect(CoreModifiable* a, KigsID asignalID, CoreModifiable* b, KigsID bslotID, T&& func)
	{
		b->InsertFunction(bslotID, func);
		Connect(a, asignalID, b, bslotID);
	}

	// remove connection between a signal and a slot 
	static void		Disconnect(CoreModifiable* a, KigsID asignalID, CoreModifiable* b, KigsID bslotID);

/*	static bool		Emit(CoreModifiable* a, KigsID asignalID, kstl::vector<CoreModifiableAttribute*>& params, void* privateParams);

	template<typename... T>
	static inline bool		Emit(CoreModifiable* a, KigsID asignalID, T&&... args)
	{
		return a->Emit(asignalID, std::forward<T>(args)...);
		//return Instance()->mConnectionManager->Emit(a, asignalID, std::forward<T>(args)...);
	}
	*/
	/**
	* \fn			kigs::unordered_map<kstl::string, CoreItemOperatorCreateMethod>&	GetDefaultCoreItemOperatorConstructMap();
	* \brief		return the map for CoreItemOperator creation in general purpose case
	* \return		kigs::unordered_map<kstl::string, CoreItemOperatorCreateMethod>&
	*/
	kigs::unordered_map<kstl::string, CoreItemOperatorCreateMethod>&	GetDefaultCoreItemOperatorConstructMap();

	/**
	 * \fn			static ModuleBase*		GetModule(const STRINGS_NAME_TYPE& classname);
	 * \brief		Get a module from its name
	 * \param		classname : name of the module to retreive
	 * \return		the asked module or NULL if not exist
	 */
	static SP<ModuleBase> GetModule(const KigsID& classname);

	template<typename T>
	static SP<T> GetModule()
	{
		return std::static_pointer_cast<T>(GetModule(T::mClassID));
	}

	/**
	* \fn			static	CMSP GetInstanceOf(const kstl::string& instancename,const STRINGS_NAME_TYPE& classname);
	* \brief		return an instance of the given class
	* \param		instancename : name of the instance
	* \param		classname : name of the class
	* \return		an CMSP (smartpointer on instance) of the asked class or a DoNothingObject if failed
	*
	* get the instance of 'classname' named 'instancename' or call the instance factory if not exist<br>
	* return a DoNothingObject if the factory failed
	*/
	//static	CMSP GetInstanceOf(const kstl::string& instancename, const KigsID& classname);


	template<typename... Args>
	static CMSP GetInstanceOf(const kstl::string& instancename, const KigsID& classname, Args&&... args);

	template<typename T>
	static SP<T> CreateInstance(const kstl::string& instancename)
	{
		return std::static_pointer_cast<T>(GetInstanceOf(instancename, T::mClassID));
	}

	/**
	 * \brief		return an unique instance of the given class (Design Pattern Singleton)
	 * \param		classname : name of the class
	 * \return		the unique instance of the asked class or NULL if failed
	 *
	 * get the instance of 'classname' or call the instance factory if not exist<br>
	 * return NULL if the factory failed
	 */
	static CMSP GetSingleton(const KigsID& classname);

	template<typename askedType>
	static SP<askedType> Singleton()
	{
		return std::static_pointer_cast<askedType>(GetSingleton(askedType::mClassID));
	}
	/**
	 * \brief		destroy the unique instance of the given class
	 * \param		classname : name of the class
	 *
	 * destroy the instance of 'classname' and remove reference as singleton (allow to create another one)
	 */
	static	void ReleaseSingleton(KigsID classname);

	void	CleanSingletonMap();


	MEMORYMANAGEMENT_START
	#ifndef _NO_MEMORY_MANAGER_
	//! retreive the memory manager
	MemoryManager*	GetMemoryManager();
	#endif
	MEMORYMANAGEMENT_END
	/**
	 * \fn			static	CoreTreeNode*	getRootNode();
	 * \brief		get the root of the core tree node
	 * \return		the root of the KigsCore tree node
	 */
	static	CoreTreeNode*	GetRootNode();
	static	CoreTreeNode*	GetTypeNode(const KigsID& id);

	static CoreTreeNode*	RegisterType(const CoreClassNameTree& type_hierarchy, const kstl::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& table);
	


	/**
	 * \fn			static void	ModuleStaticInit(KigsCore* core);
	 * \brief		for the core unique instance to be unique even in windows dll
	 * \param		core : instance of the KigsCore
	 */
	static void	ModuleStaticInit(KigsCore* core);

	/**
	 * \fn			static bool IsInit();
	 * \brief		check for init
	 * \return		TRUE if the KigsCore has been initialized
	 */
	static bool IsInit();

	/**
	 * \fn			static bool	IsMultiThread();
	 * \brief		check for multithread
	 * \return		TRUE if the KigsCore is multiThread
	 */
	static bool	IsMultiThread();

	/**
	 * \fn			template<class Module_Type> static Module_Type* LoadKigsModule()
	 * \brief		Load and initialize the given module
	 * \return		the loaded module
	 */
	template<class Module_Type> static SP<Module_Type> LoadKigsModule(const std::string& Module_Type_Name,const kstl::vector<CoreModifiableAttribute*>* params=0)
	{
		DECLARE_CLASS_INFO_WITHOUT_FACTORY(Module_Type, Module_Type_Name);
		SP<Module_Type> Module = GetModule<Module_Type>();
		if (!Module)
			Module = SP<Module_Type>(new Module_Type(kstl::string("the")+kstl::string(Module_Type_Name)));
		if (Module)
			Module->Init(Instance(), params);
		return Module;
	}

	static void RemoveModule(const std::string& classname);
	

	/**
	 * \fn			static void	GetSemaphore()
	 * \brief		add a semaphore to the core
	 */
	void	GetSemaphore();

	/**
	 * \fn			static void ReleaseSemaphore()
	 * \brief		release a semaphore from the core
	 */
	void ReleaseSemaphore();

#if KIGS_ERROR_LEVEL<=2
	// Error Management use Macro KIGS_ERROR to add errors

	/**
	 * \fn			static const char*	GetErrorBuffer();
	 * \brief		get the error buffer
	 * \return		the error buffer
	 *
	 * use Macro KIGS_ERROR to add errors
	 */
	static const char*	GetErrorBuffer();

	/**
	 * \fn			static void			DumpMessageBuffer();
	 * \brief		dump the message buffer
	 *
	 * use KIGS_DUMP_MESSAGES 
	 */
	static void			DumpMessageBuffer();

	/**
	 * \fn			static void SetMessagePrintf(int (*)(const char *format... ));
	 * \brief		print function
	 */
	static void SetMessagePrintf(int (*)(const char *format... ));
	
	/**
	 * \fn			static void SetMessagePrintf(void* func)
	 * \brief		print function
	 */
	static void			SetMessagePrintf(void* func)
	{
		SetMessagePrintf((int (*)(const char *... ))func);
	}

	/**
	 * \fn			static void	AddError(const char*, long,const kstl::string&, int errorLevel=0);
	 * \brief		add an error in the error buffer
	 * \param		char* : file in which the error appears
	 * \param		long : line number of the error appears
	 * \param		string : error message
	 * \param		errorLevel : error level in [0,3] 
	 */
	static void	AddError(const char*, long,const kstl::string&, int errorLevel=0);
#endif
#if KIGS_ERROR_LEVEL<=1
	/**
	 * \fn			static void	AddWarning(const char*, long,const kstl::string&, int warningLevel=0);
	 * \brief		add a warning in the error buffer
	 * \param		char : file in which the warning appears
	 * \param		long : line number of the warning appears
	 * \param		string : warning message
	 * \param		warningLevel : warning level in [0,3]
	 */
	static void	AddWarning(const char*, long,const kstl::string&, int warningLevel=0);
#endif
#if KIGS_ERROR_LEVEL==0
	/**
	 * \fn			static void	AddMessage(const kstl::string&);
	 * \brief		add a message in the error buffer
	 * \param		string : message
	 */
	static void	AddMessage(const kstl::string&);
#endif


	/**
	 * \fn			static GlobalProfilerManager* GetProfileManager();
	 * \brief		Get the unique instance of GlobalProfilerManager
	 * \return		the unique instance of the GlobalProfilerManager
	 */
	static GlobalProfilerManager* GetProfileManager();

	/**
	* \fn			static CoreModifiable* GetThreadProfiler();
	* \brief		Get the unique instance of ThreadProfiler
	* \return		the unique instance of the ThreadProfiler
	*/
	static CoreModifiable* GetThreadProfiler(){ return mCoreInstance->mThreadProfiler; }
	static void SetThreadProfiler(CoreModifiable* tp){ mCoreInstance->mThreadProfiler = tp; }

	/**
	 * \fn			bool	ParseXml(const kstl::string& filename,CoreModifiable*	delegateObject,const char* force_as_format=0)
	 * \brief		Parse the given xml file, using delegateObject as delegate
	 * \param		filename : the name of the file to parse
	 * \param		delegateObject : the CoreModifiable used as delegate
	 * \param		force_as_format : "xml" or "kxml". if nothing is specified, use real extension to define xml or kxml.
	 * \return		true if everything is fine

		The delegateObject will receive the parse information using the following method if it declare/implement them :

	 	DECLARE_METHOD(XMLElementStartDescription);
		DECLARE_METHOD(XMLElementEndDescription);
		DECLARE_METHOD(XMLDeclHandler);
		DECLARE_METHOD(XMLCharacterHandler);
		COREMODIFIABLE_METHODS(XMLElementStartDescription,XMLElementEndDescription,XMLDeclHandler,XMLCharacterHandler);
		If the format is force, the extension of the file can be anything (plist, dat, ...)


	 */
	static	bool	ParseXml(const kstl::string& filename,CoreModifiable*	delegateObject,const char* force_as_format=0);

	static	bool	ParseXml(char* buffer,CoreModifiable*	delegateObject,unsigned long buffsize,char* encoding=0);

	void			RegisterMainModuleList(ModuleBase* toRegister,	CoreModuleIndex index);

	ModuleBase*		GetMainModuleInList(CoreModuleIndex index)
	{
		return mCoreMainModuleList[(unsigned int)index];
	}

	void	RegisterDecorator(DecorateMethod method, DecorateMethod undecoratemethod, KigsID decoratorName);

	static  bool	DecorateInstance(CoreModifiable* cm, KigsID decoratorName);
	static  bool	UnDecorateInstance(CoreModifiable* cm, KigsID decoratorName);


	// asynchronous request management
	// only add is available, and must be called in main thread
	// management is done automatically
	static void	addAsyncRequest(AsyncRequest*);

	bool	hasAsyncRequestPending()
	{
		if (mAsyncRequestList)
		{
			if (mAsyncRequestList->size())
			{
				return true;
			}
		}
		return false;
	}

	void AddToPostDestroy(CMSP obj);
	void RemoveFromPostDestroy(CMSP obj);

protected:

	/**
	 * \fn		static void SetCoreApplication(CoreBaseApplication* _instance);
	 * Set current CoreBaseApplication
	 * \param	_instance CoreBaseApplication instance
	 */
	static void SetCoreApplication(CoreBaseApplication* _instance);

	/**
	* \fn		 static void SetNotificationCenter(NotificationCenter* _instance);
	* Set the NotificationCenter member
	* \param	_instance NotificationCenter instance
	*/
	static void SetNotificationCenter(NotificationCenter* _instance);

	CoreBaseApplication* mCoreBaseApplication;
	NotificationCenter* mNotificationCenter;

	kigs::unordered_map<kstl::string, CoreItemOperatorCreateMethod>	mCoreItemOperatorCreateMethodMap;

#if KIGS_ERROR_LEVEL<=2	
	/**
	 * \fn			void ProtectedAddError(const kstl::string& Error);
	 * \brief		add a message in the error buffer
	 * \param		Error : message
	 */
	void ProtectedAddError(const kstl::string& Error);
	
	/**
	 * \fn			void ProtectedDumpMessageBuffer();
	 * \brief		dump the message buffer
	 */
	void ProtectedDumpMessageBuffer();
#endif

	static CoreTreeNode* AddToTreeNode(KigsID parent_cid, CoreTreeNode* parent, const kstl::vector<CoreClassNameTree::TwoNames>& branch, const kstl::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& method_table, size_t current_index);


	/**
	 * \fn			KigsCore()
	 * \brief		protected constructor
	 */
	KigsCore()
	{
		mCoreBaseApplication = NULL;
		mCoreMainModuleList=NULL;

		mDecoratorMap=NULL;

		mAsyncRequestList = NULL;

		mProfilerManager = NULL;
	}
	
	/**
	 * \fn 		~KigsCore();
	 * \brief	protected destructor
	 */
	~KigsCore() {};

	//! pointer to instance factory singleton
	InstanceFactory*		mInstanceFactory;
	SP<MiniInstanceFactory>	mUpgradorFactory;

	//! pointer to initialised modules
	kigs::unordered_map<KigsID, SP<ModuleBase>>*			mModuleBaseInstanceMap;

	//! decorator map
	struct DecorateMethodPair
	{
		DecorateMethod	mDecorate;
		DecorateMethod	mUndecorate;
	};
	kigs::unordered_map<KigsID, DecorateMethodPair>*		mDecoratorMap;

	// current pending async requests
	kstl::vector<SP<AsyncRequest>>*							mAsyncRequestList;

	// called by base application update
	static void ManageAsyncRequests();

	// called by static ManageAsyncRequests
	void ProtectedManageAsyncRequests();


	//! pointer to created singletons
	kigs::unordered_map<KigsID, CMSP>*		mSingletonMap;

	//! manage post destruction
	std::mutex								mPostDestructionListMutex;
	std::unordered_map<CoreModifiable*, SP<CoreModifiable>>			mPostDestructionList;
	void ManagePostDestruction();

	friend class ModuleBase;
	//! static pointer to the KigsCore singleton
	static KigsCore*		mCoreInstance;

MEMORYMANAGEMENT_START
#ifndef _NO_MEMORY_MANAGER_
	//! pointer the memory manager instance
	MemoryManager*		mMemoryInstance;
	unsigned int*		mMemoryManagerOutOfMainAlloc;
#endif
MEMORYMANAGEMENT_END


	//! pointer to the root of the class inheritance tree
	CoreTreeNode*			 mRootNode;
	kigs::unordered_map<KigsID, CoreTreeNode*>		mTypeNodeMap;


	//! list of semaphore
	CMSP			mSemaphore;
	friend class	ModuleThread;


	ModuleBase**	mCoreMainModuleList;


	
	/**
	 * \fn 		static void		SetMultiThread();
	 * \brief	set the KigsCore multiTread
	 */
	static void		SetMultiThread();

	/**
	 * \fn 		static void		CloseMultiThread();
	 * \brief	remove the multiThread management of the KigsCore
	 */
	static void		CloseMultiThread();

	//! TRUE if the KigsCore is multiThread
	bool*			mMultiThread;


#if KIGS_ERROR_LEVEL<=2
	// construct error list here
	//! list of error
	kstl::vector<kstl::string>*	mErrorList;
	/**
	 * \fn 		int	(*KigsMsgPrintf)(const char *format... );
	 * \brief	output function for error and warning msb
	 */
	int	(*KigsMsgPrintf)(const char *format... );
#endif
	//! Profilers management
	GlobalProfilerManager*		mProfilerManager;
	CoreModifiable*				mThreadProfiler;

};

#if KIGS_ERROR_LEVEL==0
#define KIGS_MESSAGE(a)		KigsCore::AddMessage(a)

#ifndef KIGS_PREFIX_MESSAGE
#define KIGS_PREFIX_MESSAGE const char*	KigsPrefixMessage="Message   :";
#endif

#else
#define KIGS_MESSAGE(a)		
#endif


#if KIGS_ERROR_LEVEL<=1
#define KIGS_WARNING(a,b)	KigsCore::AddWarning(__FILE__, __LINE__,a,((int)b))

#ifndef KIGS_PREFIX_WARNING
#define KIGS_PREFIX_WARNING const char*	KigsPrefixWarning[3]={"Warning *   :","Warning **  :","Warning *** :"};
#endif

#else
#define KIGS_WARNING(a,b)	
#endif


#if KIGS_ERROR_LEVEL<=2
#define KIGS_ERROR(a,b)					KigsCore::AddError(__FILE__, __LINE__,a,((int)b))
#define KIGS_DUMP_MESSAGES				KigsCore::DumpMessageBuffer();
#define KIGS_SET_MESSAGES_PRINTF(a)		KigsCore::SetMessagePrintf(a);

#ifndef KIGS_PREFIX_ERROR
#define KIGS_PREFIX_ERROR const char*	KigsPrefixError[3]={"ERROR *   :","ERROR **  :","ERROR *** :"};
#endif

#else
#define KIGS_ERROR(a,b)	
#define KIGS_DUMP_MESSAGES
#define KIGS_SET_MESSAGES_PRINTF(a)
#endif

// DEFINE ASSERTS

#ifdef KIGS_TOOLS
#define KIGS_ASSERT(a)	assert(a)
#else
#define KIGS_ASSERT(a)	
#endif


//#include "AttributePacking.h"
template<>
inline CMSP KigsCore::GetInstanceOf(const kstl::string& instancename, const KigsID& classname)
{
	CMSP instance = Instance()->GetInstanceFactory()->GetInstance(instancename, classname);
	//! if instance factory fail then create a DoNothingObject (and print debug messages)
	if (!instance)
	{
		instance = Instance()->GetInstanceFactory()->GetInstance(instancename, "DoNothingObject");
#ifdef _DEBUG
#ifdef KEEP_NAME_AS_STRING
		kigsprintf("unknown object : %s of type %s \n", instancename.c_str(), classname._id_name.c_str());
#else
		kigsprintf("unknown object : %s \n", instancename.c_str());
#endif

#ifdef KEEP_NAME_AS_STRING
		if (!instance)
			kigsprintf("ALLOCATION ERROR : %s \n", classname._id_name.c_str());
#endif
#endif
	}
	return instance;
}


#define STACK_STRING(name, size, ...) char name[size]; snprintf(name, size, __VA_ARGS__);


template<typename... Args>
inline CMSP KigsCore::GetInstanceOf(const kstl::string& instancename, const KigsID& classname, Args&&... args)
{	 
	PackCoreModifiableAttributes packer{ nullptr };
	int expander[]
	{
		(packer << std::forward<Args>(args), 0)...
	};
	(void)expander;
	CMSP instance = Instance()->GetInstanceFactory()->GetInstance(instancename, classname, &(kstl::vector<CoreModifiableAttribute*>&)packer);
		
	//! if instance factory fail then create a DoNothingObject (and print debug messages)
	if (!instance)
	{
		instance = Instance()->GetInstanceFactory()->GetInstance(instancename, "DoNothingObject");
#ifdef _DEBUG
#ifdef KEEP_NAME_AS_STRING
		kigsprintf("unknown object : %s of type %s \n", instancename.c_str(), classname._id_name.c_str());
#else
		kigsprintf("unknown object : %s \n", instancename.c_str());
#endif

#ifdef KEEP_NAME_AS_STRING
		if (!instance)
			kigsprintf("ALLOCATION ERROR : %s \n", classname._id_name.c_str());
#endif
#endif
	}
	return instance;
}

void replaceAll(std::string& source, const std::string& from, const std::string& to);

#endif //_CORE_H_
