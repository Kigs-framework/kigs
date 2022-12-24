#pragma once

#include "CoreTypes.h"

#define USE_CLASS_NAME_TREE 0

#if USE_CLASS_NAME_TREE
#define CLASS_NAME_TREE_ARG CoreClassNameTree* classNameTree
#define DECLARE_CLASS_NAME_TREE_ARG CLASS_NAME_TREE_ARG=0
#define PASS_CLASS_NAME_TREE_ARG EnrichClassNameTree(classNameTree, mClassID, mRuntimeType)
#else
#define CLASS_NAME_TREE_ARG std::vector<CoreModifiableAttribute*>* args
#define DECLARE_CLASS_NAME_TREE_ARG CLASS_NAME_TREE_ARG=0
#define PASS_CLASS_NAME_TREE_ARG args
#endif

#ifdef KEEP_NAME_AS_STRING
#define DECLARE_GetRuntimeType(currentClass) virtual const std::string& GetRuntimeType() const override {return currentClass::mRuntimeType._id_name;} 
#define DECLARE_getExactType(currentClass) virtual const std::string& getExactType() const override {return currentClass::mClassID._id_name;}
#define DECLARE_GetRuntimeTypeBase(currentClass) virtual const std::string& GetRuntimeType() const {return currentClass::mRuntimeType._id_name;} 
#define DECLARE_getExactTypeBase(currentClass) virtual const std::string& getExactType() const {return currentClass::mClassID._id_name;}
#else
#define DECLARE_GetRuntimeType(currentClass) virtual const KigsID& GetRuntimeType() const override {return currentClass::mRuntimeType;} 
#define DECLARE_getExactType(currentClass) virtual const KigsID& getExactType() const override {return currentClass::mClassID;}
#define DECLARE_GetRuntimeTypeBase(currentClass) virtual const KigsID& GetRuntimeType() const {return currentClass::mRuntimeType;} 
#define DECLARE_getExactTypeBase(currentClass) virtual const KigsID& getExactType() const {return currentClass::mClassID;}
#endif


#ifdef KIGS_TOOLS
#include "XMLNode.h"
#define CONNECT_FIELD Kigs::Xml::XMLNodeBase* xmlattr=nullptr;
#define CONNECT_PARAM_DEFAULT ,Kigs::Xml::XMLNodeBase* xmlattr=nullptr
#define CONNECT_PARAM ,Kigs::Xml::XMLNodeBase* xmlattr
#define CONNECT_PASS_PARAM ,xmlattr
#define CONNECT_PASS_MANAGED(a) ,a.xmlattr
#else
#define CONNECT_FIELD
#define CONNECT_PARAM_DEFAULT
#define CONNECT_PARAM
#define CONNECT_PASS_PARAM
#define CONNECT_PASS_MANAGED(a)
#endif


#define DEFINE_METHOD(currentclass,name)  bool currentclass::name(CoreModifiable* sender,std::vector<CoreModifiableAttribute*>& params,void* privateParams)

#define DECLARE_METHOD(name)						bool	name(CoreModifiable* sender,std::vector<CoreModifiableAttribute*>& params,void* privateParams);
#define DECLARE_VIRTUAL_METHOD(name)		virtual bool	name(CoreModifiable* sender,std::vector<CoreModifiableAttribute*>& params,void* privateParams);
#define DECLARE_PURE_VIRTUAL_METHOD(name)	virtual bool	name(CoreModifiable* sender,std::vector<CoreModifiableAttribute*>& params,void* privateParams)=0;
#define DECLARE_OVERRIDE_METHOD(name)		virtual bool	name(CoreModifiable* sender,std::vector<CoreModifiableAttribute*>& params,void* privateParams) override;

#define NOT_WRAPPED_METHOD_PUSH_BACK(name) table.push_back({ #name, static_cast<CoreModifiable::ModifiableMethod>(&CurrentClassType::name) });

#define COREMODIFIABLE_METHODS(...) \
public:\
static void GetNotWrappedMethodTable(std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& table)\
{\
	ParentClassType::GetNotWrappedMethodTable(table);\
	FOR_EACH(NOT_WRAPPED_METHOD_PUSH_BACK, __VA_ARGS__)\
}


#define WRAP_METHOD_NO_CTOR(name) inline bool	name##Wrap(CoreModifiable* sender,std::vector<CoreModifiableAttribute*>& params,void* privateParams){\
	kigs_impl::UnpackAndCall(&CurrentClassType::name, this, sender, params); return false; }


#define METHOD_PUSH_BACK(name) table.push_back({ #name, static_cast<CoreModifiable::ModifiableMethod>(&CurrentClassType::name##Wrap) });

// Need #include "AttributePacking.h"
#define WRAP_METHODS(...) FOR_EACH(WRAP_METHOD_NO_CTOR, __VA_ARGS__)\
public:\
static void GetMethodTable(std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& table)\
{\
	ParentClassType::GetMethodTable(table);\
	FOR_EACH(METHOD_PUSH_BACK, __VA_ARGS__)\
}


#define DEFINE_DYNAMIC_METHOD(currentclass,name)  class DynamicMethod##name : public currentclass \
{ \
 public:\
typedef DynamicMethod##name CurrentClassType;\
 DECLARE_METHOD(name);\
};\
DEFINE_METHOD(DynamicMethod##name,name)

#define DEFINE_DYNAMIC_INLINE_METHOD(currentclass,name)  class DynamicMethod##name : public currentclass \
{ \
 public:\
typedef DynamicMethod##name CurrentClassType;\
 inline DECLARE_METHOD(name);\
};\
inline DEFINE_METHOD(DynamicMethod##name,name)


#define INSERT_DYNAMIC_METHOD(methodname,callingname) InsertMethod(#callingname,static_cast<CoreModifiable::ModifiableMethod>(&DynamicMethod##methodname::methodname));

/*! macro used to define an abstract class
 to be set in the class definition
*/
#define BASE_DECLARE_ABSTRACT_CLASS_INFO(currentClass,parentClass,moduleManagerName) \
public:\
static const KigsID mClassID; \
static KigsID mRuntimeType; \
typedef bool (currentClass::*ModifiableMethod)(CoreModifiable* sender,std::vector<CoreModifiableAttribute*>&,void* privateParams); \
typedef currentClass CurrentClassType; \
typedef parentClass ParentClassType; \
bool Call(CoreModifiable::ModifiableMethod method,CoreModifiable* sender,std::vector<CoreModifiableAttribute*>& attr,void* privateParams) override\
{\
	currentClass::ModifiableMethod currentmethod=static_cast<currentClass::ModifiableMethod>(method);\
	return (this->*(currentmethod))(sender,attr,privateParams);\
}\
DECLARE_GetRuntimeType(currentClass);\
DECLARE_getExactType(currentClass);\
KigsID getExactTypeID() const override {return currentClass::mClassID;} \
bool isSubType(const KigsID& cid) const override {if(currentClass::mClassID==cid)return true;  return parentClass::isSubType(cid);} \
static void GetClassNameTree(CoreClassNameTree& classNameTree) {parentClass::GetClassNameTree(classNameTree); classNameTree.addClassName(currentClass::mClassID, currentClass::mRuntimeType);}\
virtual void ConstructClassNameTree(CoreClassNameTree& classNameTree) override {parentClass::ConstructClassNameTree(classNameTree); classNameTree.addClassName(currentClass::mClassID, currentClass::mRuntimeType);}\
SP<currentClass> SharedFromThis() { return debug_checked_pointer_cast<currentClass>(this->shared_from_this()); }\
static currentClass* Get()\
{\
	return parentClass::GetFirstInstance(#currentClass, false)->as<currentClass>();\
}\
static currentClass* Get(const std::string &name)\
{\
	return parentClass::GetFirstInstanceByName(#currentClass, name, false)->as<currentClass>();\
}\
public:

//static void GetClassNameTree(std::vector<KigsID>& classNameTree) {parentClass::GetClassNameTree(classNameTree); classNameTree.push_back(currentClass::mClassID);}\

//virtual void	callConstructor(RefCountedBaseClass* tocall,const std::string& instancename) const { ((currentClass*)tocall)->currentClass::currentClass(instancename);	};

//friend class maMethod;

/*! macro used to define a instanciable class
 to be set in the class definition
*/
#define DECLARE_ABSTRACT_CLASS_INFO(currentClass,parentClass,moduleManagerName) \
BASE_DECLARE_ABSTRACT_CLASS_INFO(currentClass,parentClass,moduleManagerName)

#define DECLARE_CLASS_INFO(currentClass,parentClass,moduleManagerName) \
DECLARE_ABSTRACT_CLASS_INFO(currentClass,parentClass,moduleManagerName) \
static CMSP CreateInstance(const std::string& instancename, std::vector<CoreModifiableAttribute*>* args=nullptr) \
{   \
	auto instance = std::make_shared<currentClass>(instancename, args); \
	instance->RegisterToCore();\
	return instance; \
};

/*! macro used to set static members
 to be set in the .cpp file of the class (implementation)
*/


#define  IMPLEMENT_CLASS_INFO(currentClass) \
const KigsID currentClass::mClassID = #currentClass; \
KigsID currentClass::mRuntimeType = "";

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
const KigsID currentClass<templatetype>::mClassID = #currentClass; \
template<typename templatetype> \
KigsID currentClass<templatetype>::mRuntimeType = "";
/*
#define IMPLEMENT_AND_REGISTER_TEMPLATE_CLASS_INFO(templatetype, currentClass, returnClassName, group)\
IMPLEMENT_TEMPLATE_CLASS_INFO(templatetype, currentClass)\
REGISTER_CLASS_INFO(currentClass<templatetype>, returnClassName, group)*/

/*! macro used to register the class so that the class can be instanciated by the instanceFactory
 generally set in the associated module Init method
*/
#define DECLARE_CLASS_INFO_WITHOUT_FACTORY(currentClass,returnclassname) \
{\
	currentClass::mRuntimeType = returnclassname;\
	CoreClassNameTree TypeBranch;\
	currentClass::GetClassNameTree(TypeBranch);\
	std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>> MethodTable;\
	currentClass::GetMethodTable(MethodTable);\
	currentClass::GetNotWrappedMethodTable(MethodTable);\
	KigsCore::RegisterType(TypeBranch, MethodTable);\
}

#define DECLARE_FULL_CLASS_INFO(core,currentClass,returnclassname,moduleManagerName) \
RegisterClassToInstanceFactory(core,#moduleManagerName, #returnclassname,&currentClass::CreateInstance); \
DECLARE_CLASS_INFO_WITHOUT_FACTORY(currentClass,#returnclassname)

#define ADD_ALIAS(name) ,#name

#define DECLARE_CLASS_ALIAS(core,alias,baseclass) \
{core->GetInstanceFactory()->addAlias(#alias,{#baseclass});}

#define DECLARE_CLASS_ALIAS_AND_UPGRADE(core,alias,baseclass,...) \
{core->GetInstanceFactory()->addAlias(#alias,{#baseclass FOR_EACH(ADD_ALIAS,__VA_ARGS__)});}

#define DECLARE_CONSTRUCTOR(currentClass) currentClass(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
#define DECLARE_INLINE_CONSTRUCTOR(currentClass) currentClass(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG) : currentClass::ParentClassType(name, PASS_CLASS_NAME_TREE_ARG)

#define IMPLEMENT_CONSTRUCTOR(currentClass) currentClass::currentClass(const std::string& name, CLASS_NAME_TREE_ARG) : currentClass::ParentClassType(name, PASS_CLASS_NAME_TREE_ARG)

#define BASE_ATTRIBUTE(name, ...) {*this, false, #name, __VA_ARGS__ }
#define INIT_ATTRIBUTE(name, ...) {*this, true, #name, __VA_ARGS__ }
