#ifndef _REFCOUNTEDBASECLASS_H_
#define _REFCOUNTEDBASECLASS_H_

#ifdef _MSC_VER
#pragma warning(disable : 4355)
#endif

#include "GenericRefCountedBaseClass.h"
#include "CoreTypes.h"
#include "kstlstring.h"
#include "kstlmap.h"
#include "kstlvector.h"
#include "kstllist.h"
#include "kstlset.h"
//#include "InstanceFactory.h"
class CoreClassNameTree;
class CoreModifiable;
class CoreModifiableAttribute;

typedef     CoreModifiable* (*createMethod)(const kstl::string& instancename, kstl::vector<CoreModifiableAttribute*>* args);

#ifdef _KIGS_ONLY_STATIC_LIB_
#include "Platform/Core/PlatformCore.h"
#endif

//#define USE_REFCOUNTED_LINKEDLIST

#ifdef USE_REFCOUNTED_LINKEDLIST

#define	LINKEDLIST_PARENT_ARRAY_POW2_DECAL		12
#define	LINKEDLIST_PARENT_ARRAY_SIZE			(1<<LINKEDLIST_PARENT_ARRAY_POW2_DECAL)
#define	LINKEDLIST_PARENT_ARRAY_INDEX_DECAL		(32-LINKEDLIST_PARENT_ARRAY_POW2_DECAL)
#define	LINKEDLIST_PARENT_ARRAY_MASK			((LINKEDLIST_PARENT_ARRAY_SIZE-1)<<LINKEDLIST_PARENT_ARRAY_INDEX_DECAL)
#define	LINKEDLIST_PAGE_MASK					(0xFFFFFFFF^LINKEDLIST_PARENT_ARRAY_MASK)

#endif // USE_REFCOUNTED_LINKEDLIST


//#define TRACK_ITEM
#ifdef TRACK_ITEM
#define TRACK_ITEM_STATICDECL static kstl::vector<RefCountedBaseClass*> * sObj;
#define TRACK_ITEM_STATICIMPL  kstl::vector<RefCountedBaseClass*> * RefCountedBaseClass::sObj = NULL;
#define TRACK_ITEM_ADDOBJ if(!sObj) sObj = new kstl::vector<RefCountedBaseClass*>(); sObj->push_back(this);
#define TRACK_ITEM_DELOBJ if(sObj) for(kstl::vector<RefCountedBaseClass*>::iterator it=sObj->begin();it!=sObj->end();++it) { if((*it)==this){ sObj->erase(it); break; } }
#else
#define TRACK_ITEM_STATICDECL
#define TRACK_ITEM_STATICIMPL
#define TRACK_ITEM_ADDOBJ
#define TRACK_ITEM_DELOBJ
#endif


#ifdef KIGS_TOOLS
#define TRACEREF_DECL bool myTraceRef
#define TRACEREF_IMPL myTraceRef=false
#else
#define TRACEREF_DECL
#define TRACEREF_IMPL
#endif




//#define CC_HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100)
//#define CC_HOST_IS_BIG_ENDIAN false

// fast meta coded (at compile time) string comparison 
template<int unsignedICount>
inline bool	cmpMem(const unsigned int* a, const unsigned int* b)
{
	int index=unsignedICount; 
	while(index--) 
	{ 
		if(*--b!=*--a) 
		{ 
			return false; 
		} 
	}
	return true; 
} 

template<unsigned int mask>
inline bool	cmpMask(const unsigned int* a, const unsigned int* b)
{
	printf("can not be in not specialized func here\n"); 
	return true;
} 

template<>
inline bool	cmpMask<0>(const unsigned int* a, const unsigned int* b)
{
	return true; 
} 

template<>
inline bool	cmpMask<1>(const unsigned int* a, const unsigned int* b)
{
	if(CC_HOST_IS_BIG_ENDIAN == true)
	{
		return (((*a)&0xFF000000) == ((*b)&0xFF000000));
	}
	else
	{
		return (((*a)&0xFF) == ((*b)&0xFF));
	}
} 

template<>
inline bool	cmpMask<2>(const unsigned int* a, const unsigned int* b)
{
	if(CC_HOST_IS_BIG_ENDIAN == true)
	{
		return (((*a)&0xFFFF0000) == ((*b)&0xFFFF0000));
	}
	else
	{
		return (((*a)&0xFFFF) == ((*b)&0xFFFF));
	}
} 

template<>
inline bool	cmpMask<3>(const unsigned int* a, const unsigned int* b)
{
	if(CC_HOST_IS_BIG_ENDIAN == true)
	{
		return (((*a)&0xFFFFFF00) == ((*b)&0xFFFFFF00));
	}
	else
	{
		return (((*a)&0xFFFFFF) == ((*b)&0xFFFFFF));
	}
} 

template<unsigned int stringSize>
inline bool	cmpString(const char* a, const char* b)
{
	int indexMask=stringSize&3;
	a+=stringSize-indexMask;
	b+=stringSize-indexMask;
	
	return cmpMask<stringSize&3>((const unsigned int*)a,(const unsigned int*)b) && cmpMem<stringSize/4 >((const unsigned int*)a,(const unsigned int*)b) ;
}

#define fastCmp(a,b)	cmpString<sizeof(#a)>(#a,b)

//#if defined(_DEBUG) && defined(_WINDOWS)
//	#define _TRACKREFCOUNT
//	#ifdef _TRACKREFCOUNT
//	#define Log printf
//	#endif
//#endif

class KigsCore;
class CoreModifiableAttribute;

struct MethodCallingStruct
{
	unsigned int    myMethodID;
	void*			myPrivateParams;
	CoreModifiable* myMethodInstance;
};

// ****************************************
// * RegisterClassToInstanceFactory class
// * --------------------------------------
/**
 * \class	RegisterClassToInstanceFactory
 * \ingroup KigsCore
 * \brief	class used to register other classes in instanceFactory
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************

void RegisterClassToInstanceFactory(KigsCore* core, kstl::string moduleName, KigsID classID, createMethod method);


class CoreModifiable;


#ifdef KEEP_NAME_AS_STRING
#define DECLARE_GetRuntimeType(currentClass) virtual const kstl::string& GetRuntimeType() const override {return currentClass::myRuntimeType._id_name;} 
#define DECLARE_getExactType(currentClass) virtual const kstl::string& getExactType() const override {return currentClass::myClassID._id_name;}
#define DECLARE_GetRuntimeTypeBase(currentClass) virtual const kstl::string& GetRuntimeType() const {return currentClass::myRuntimeType._id_name;} 
#define DECLARE_getExactTypeBase(currentClass) virtual const kstl::string& getExactType() const {return currentClass::myClassID._id_name;}
#else
#define DECLARE_GetRuntimeType(currentClass) virtual const KigsID& GetRuntimeType() const override {return currentClass::myRuntimeType;} 
#define DECLARE_getExactType(currentClass) virtual const KigsID& getExactType() const override {return currentClass::myClassID;}
#define DECLARE_GetRuntimeTypeBase(currentClass) virtual const KigsID& GetRuntimeType() const {return currentClass::myRuntimeType;} 
#define DECLARE_getExactTypeBase(currentClass) virtual const KigsID& getExactType() const {return currentClass::myClassID;}
#endif


// ****************************************
// * RefCountedBaseClass class
// * --------------------------------------
/**
 * \file	RefCountedBaseClass.h
 * \class	RefCountedBaseClass
 * \ingroup KigsCore
 * \brief	Base class for classes with refcounting
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************
class RefCountedBaseClass : public GenericRefCountedBaseClass
{
	
#ifdef USE_REFCOUNTED_LINKEDLIST
	protected:
	RefCountedBaseClass*				m_prev;
	RefCountedBaseClass*				m_next;
	unsigned int						m_allocPage;
	static	RefCountedBaseClass*		m_root;
	static	unsigned int				m_currentAllocPage;
	
	static	RefCountedBaseClass*		m_ParentArray[LINKEDLIST_PARENT_ARRAY_SIZE]; // store parent CCObject list (array or dictionnary)
	
	static void	markCoreModifiableItems(RefCountedBaseClass* current,unsigned int index);
	static void	markCoreItems(RefCountedBaseClass* current,unsigned int index);
	
	inline unsigned int		getObjectAllocPage() const
	{
		return m_allocPage&LINKEDLIST_PAGE_MASK;
	}
	
	inline unsigned int		getObjectParentObject() const
	{
		return (m_allocPage&LINKEDLIST_PARENT_ARRAY_MASK)>>LINKEDLIST_PARENT_ARRAY_INDEX_DECAL;
	}
	
	void	insertThisInDLinkedList()
	{
		m_prev=0;
		if(m_root)
		{
			m_root->m_prev=this;
		}
		m_next=m_root;
		m_root=this;
		m_allocPage=m_currentAllocPage;
	}
	
	void	removeThisFromDLinkedList()
	{
		if(m_prev)
		{
			m_prev->m_next=m_next;
		}
		if(m_next)
		{
			m_next->m_prev=m_prev;
		}
		if(this == m_root)
		{
			m_root=m_next;
		}
	}
	
	static void	prepareParentArrayList();
#endif // USE_REFCOUNTED_LINKEDLIST
	
	public:
	TRACK_ITEM_STATICDECL;
	
#ifdef USE_REFCOUNTED_LINKEDLIST
	
	static unsigned int getAllocPage()
	{
		return m_currentAllocPage;
	}
	static void setAllocPage(unsigned int page)
	{
		m_currentAllocPage=page;
	}
	static void incAllocPage()
	{
		m_currentAllocPage++;
	}
	
	static inline void	logObjectInfos(RefCountedBaseClass* obj);
	static void	searchObjectWithPage(unsigned int page);
	static void	searchObjectWithinPage(unsigned int pagemin,unsigned int pagemax);
	static void	searchObjectWithPageList(const std::vector<int>& pagelist);
#endif //USE_REFCOUNTED_LINKEDLIST

	//! id for this class
	static const KigsID myClassID;
	static KigsID myRuntimeType;

	DECLARE_GetRuntimeTypeBase(RefCountedBaseClass);
	DECLARE_getExactTypeBase(RefCountedBaseClass);

	
	virtual KigsID getExactTypeID() const { return RefCountedBaseClass::myClassID; }


	virtual bool isSubType(const KigsID& cid) const { return  myClassID == cid; }
	
	/**
	 * \fn		static void GetClassNameTree(kstl::vector<KigsID>& classNameTree) 
	 * \brief	return the list of classes this class inherits from
	 * \return	classNameTree : list of classes this class inherits from
	 */
	static void GetClassNameTree(CoreClassNameTree& classNameTree);

	
	/**
	* \fn 		virtual bool    checkDestroy();
	* \brief	
	*
	*/
	virtual bool			checkDestroy() override;
	
	/**
	* \fn		void            GetRef();
	* \brief	increment reference count
	*/
#if defined (_DEBUG) && defined(WIN32)
	// GetRef only needs to be virtual for debug purpose
	void            GetRef() override; 
#endif
	
	protected:
	/**
	  * \fn		explicit RefCountedBaseClass(kstl::string name=DefaultName());
	  * \brief	constructor
	  * \param	name : name of the new instance
	  */
	explicit RefCountedBaseClass();
	
	/**
	 * \brief	destructor
	 * \fn 		~RefCountedBaseClass();
	 */
	virtual         ~RefCountedBaseClass()
	{
#ifdef USE_REFCOUNTED_LINKEDLIST
		removeThisFromDLinkedList();
#endif //USE_REFCOUNTED_LINKEDLIST
		TRACK_ITEM_DELOBJ;
	}
	
	public:
	TRACEREF_DECL;
	protected:
	
	/**
	 * \fn		virtual void    ProtectedDestroy();
	 * \brief	do the real object destruction
	   to be done by son classes
	 */
	virtual void ProtectedDestroy() = 0;
	
	private:
	// don't use copy constructor and operator
	//! brief  avoid calling copy contructor
	explicit RefCountedBaseClass(const RefCountedBaseClass&) = delete;
	//! brief  avoid calling copy operator
	RefCountedBaseClass& operator=(const RefCountedBaseClass&) = delete;
	
	
};

/*! macro used to define an abstract class
 to be set in the class definition
*/
#define BASE_DECLARE_ABSTRACT_CLASS_INFO(currentClass,parentClass,moduleManagerName) \
public:\
static const KigsID myClassID; \
static KigsID myRuntimeType; \
typedef bool (currentClass::*ModifiableMethod)(CoreModifiable* sender,kstl::vector<CoreModifiableAttribute*>&,void* privateParams); \
typedef currentClass CurrentClassType; \
typedef parentClass ParentClassType; \
bool Call(RefCountedClass::ModifiableMethod method,CoreModifiable* sender,kstl::vector<CoreModifiableAttribute*>& attr,void* privateParams) override\
{\
	currentClass::ModifiableMethod currentmethod=static_cast<currentClass::ModifiableMethod>(method);\
	return (this->*(currentmethod))(sender,attr,privateParams);\
}\
DECLARE_GetRuntimeType(currentClass);\
DECLARE_getExactType(currentClass);\
KigsID getExactTypeID() const override {return currentClass::myClassID;} \
bool isSubType(const KigsID& cid) const override {if(currentClass::myClassID==cid)return true;  return parentClass::isSubType(cid);} \
static void GetClassNameTree(CoreClassNameTree& classNameTree) {parentClass::GetClassNameTree(classNameTree); classNameTree.addClassName(currentClass::myClassID, currentClass::myRuntimeType);}\
static currentClass* Get()\
{\
	return GetFirstInstance(#currentClass, false)->as<currentClass>();\
}\
static currentClass* Get(const std::string &name)\
{\
	return GetFirstInstanceByName(#currentClass, name, false)->as<currentClass>();\
}\
public:

//static void GetClassNameTree(kstl::vector<KigsID>& classNameTree) {parentClass::GetClassNameTree(classNameTree); classNameTree.push_back(currentClass::myClassID);}\

//virtual void	callConstructor(RefCountedBaseClass* tocall,const kstl::string& instancename) const { ((currentClass*)tocall)->currentClass::currentClass(instancename);	};

//friend class maMethod;

/*! macro used to define a instanciable class
 to be set in the class definition
*/
#define DECLARE_ABSTRACT_CLASS_INFO(currentClass,parentClass,moduleManagerName) \
BASE_DECLARE_ABSTRACT_CLASS_INFO(currentClass,parentClass,moduleManagerName)

#define DECLARE_CLASS_INFO(currentClass,parentClass,moduleManagerName) \
DECLARE_ABSTRACT_CLASS_INFO(currentClass,parentClass,moduleManagerName) \
static CoreModifiable* CreateInstance(const kstl::string& instancename, kstl::vector<CoreModifiableAttribute*>* args=nullptr) \
{   \
	CoreModifiable* instance = static_cast<CoreModifiable*>(new currentClass(instancename, args)); \
	instance->RegisterToCore();\
	return instance; \
};

/*! macro used to set static members
 to be set in the .cpp file of the class (implementation)
*/


#define  IMPLEMENT_CLASS_INFO(currentClass) \
const KigsID currentClass::myClassID = #currentClass; \
KigsID currentClass::myRuntimeType = "";

// auto registring does not work when not in "unity" mode 
/*#define REGISTER_CLASS_INFO(currentClass, returnClassName, group)\
struct AutoRegisterType##returnClassName { \
	AutoRegisterType##returnClassName()\
	{\
		AddToAutoRegister([]\
		{\
			DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), currentClass, returnClassName, group);\
		}, #group);\
	}\
}; \
AutoRegisterType##returnClassName AutoRegisterVar##returnClassName;
*/


/*

#define IMPLEMENT_AND_REGISTER_CLASS_INFO(currentClass, returnClassName, group)\
IMPLEMENT_CLASS_INFO(currentClass)\
REGISTER_CLASS_INFO(currentClass, returnClassName, group)
*/


#define  IMPLEMENT_TEMPLATE_CLASS_INFO(templatetype,currentClass) \
template<typename templatetype> \
const KigsID currentClass<templatetype>::myClassID = #currentClass; \
template<typename templatetype> \
KigsID currentClass<templatetype>::myRuntimeType = "";
/*
#define IMPLEMENT_AND_REGISTER_TEMPLATE_CLASS_INFO(templatetype, currentClass, returnClassName, group)\
IMPLEMENT_TEMPLATE_CLASS_INFO(templatetype, currentClass)\
REGISTER_CLASS_INFO(currentClass<templatetype>, returnClassName, group)*/

/*! macro used to register the class so that the class can be instanciated by the instanceFactory
 generally set in the associated module Init method
*/
#define DECLARE_CLASS_INFO_WITHOUT_FACTORY(currentClass,returnclassname) \
{\
	currentClass::myRuntimeType = returnclassname;\
	CoreClassNameTree TypeBranch;\
	currentClass::GetClassNameTree(TypeBranch);\
	kstl::vector<std::pair<KigsID, RefCountedClass::ModifiableMethod>> MethodTable;\
	currentClass::GetMethodTable(MethodTable);\
	KigsCore::RegisterType(TypeBranch, MethodTable);\
}

#define DECLARE_FULL_CLASS_INFO(core,currentClass,returnclassname,moduleManagerName) \
RegisterClassToInstanceFactory(core,#moduleManagerName, #returnclassname,&currentClass::CreateInstance); \
DECLARE_CLASS_INFO_WITHOUT_FACTORY(currentClass,#returnclassname)



#define DECLARE_CONSTRUCTOR(currentClass) currentClass(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
#define DECLARE_INLINE_CONSTRUCTOR(currentClass) currentClass(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG) : currentClass::ParentClassType(name, PASS_CLASS_NAME_TREE_ARG)

#define IMPLEMENT_CONSTRUCTOR(currentClass) currentClass::currentClass(const kstl::string& name, CLASS_NAME_TREE_ARG) : currentClass::ParentClassType(name, PASS_CLASS_NAME_TREE_ARG)

#define BASE_ATTRIBUTE(name, ...) {*this, false, #name, __VA_ARGS__ }
#define INIT_ATTRIBUTE(name, ...) {*this, true, #name, __VA_ARGS__ }

#endif //_REFCOUNTEDBASECLASS_H_
