#pragma once

#include "CoreTypes.h"


#define CLASS_NAME_TREE_ARG std::vector<std::pair<KigsID,CoreModifiableAttribute*>>* mappedAttr, std::vector<CoreModifiableAttribute*>* args
#define DECLARE_CLASS_NAME_TREE_ARG std::vector<std::pair<KigsID,CoreModifiableAttribute*>>* mappedAttr=nullptr, std::vector<CoreModifiableAttribute*>* args=0
#define PASS_CLASS_NAME_TREE_ARG addAllMappedAttributes<true>(this,mappedAttr),args

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
template<bool T>\
std::vector<std::pair<KigsID,CoreModifiableAttribute*>>* addAllMappedAttributes(CurrentClassType* localthis,std::vector<std::pair<KigsID,CoreModifiableAttribute*>>* parent=nullptr)\
{\
	return parent; \
}\
public:


/*! macro used to define a instanciable class
 to be set in the class definition
*/
#define DECLARE_ABSTRACT_CLASS_INFO(currentClass,parentClass,moduleManagerName) \
BASE_DECLARE_ABSTRACT_CLASS_INFO(currentClass,parentClass,moduleManagerName)

#define DECLARE_CLASS_INFO(currentClass,parentClass,moduleManagerName) \
DECLARE_ABSTRACT_CLASS_INFO(currentClass,parentClass,moduleManagerName) \
static CMSP CreateInstance(const std::string& instancename, std::vector<CoreModifiableAttribute*>* args=nullptr) \
{   \
	CMallocator<currentClass> cmalloc;\
	auto instance = std::allocate_shared<currentClass>(cmalloc, instancename, nullptr, args); \
	instance->RegisterToCore();\
	return instance; \
};

/*! macro used to set static members
 to be set in the .cpp file of the class (implementation)
*/


#define  IMPLEMENT_CLASS_INFO(currentClass) \
const KigsID currentClass::mClassID = #currentClass; \
KigsID currentClass::mRuntimeType = "";


#define  IMPLEMENT_TEMPLATE_CLASS_INFO(templatetype,currentClass) \
template<typename templatetype> \
const KigsID currentClass<templatetype>::mClassID = #currentClass; \
template<typename templatetype> \
KigsID currentClass<templatetype>::mRuntimeType = "";


template <class T1, typename T2>
inline constexpr uintptr_t myoffset_of(T1* localthis, T2* member) {
	return uintptr_t(intptr_t(member) - intptr_t(localthis));
}


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

#define BASE_ATTRIBUTE(name, ...) {*this, #name, __VA_ARGS__ }

#define CREATE_LOCAL_ATTRIBUTE(name)  uintptr_t name##WrapAttr[(sizeof(CoreModifiableAttributeMap<decltype(name),0>)+sizeof(uintptr_t)-1)/sizeof(uintptr_t)];
#define ADD_MAPPED_ATTRIBUTE(name) localthis->addMappedAttribute<decltype(localthis->name)>(myoffset_of(localthis,&localthis->name),#name,parent,(CoreModifiableAttribute*)(localthis->name##WrapAttr));

// WRAP_ATTRIBUTES uses uintptr_t placeholders for CoreModifiableAttributeMap
#define WRAP_ATTRIBUTES(...) \
protected:\
FOR_EACH(CREATE_LOCAL_ATTRIBUTE, __VA_ARGS__)\
public:\
template<>\
std::vector<std::pair<KigsID,CoreModifiableAttribute*>>* addAllMappedAttributes<true>(CurrentClassType* localthis,std::vector<std::pair<KigsID,CoreModifiableAttribute*>>* parent)\
{\
	if(!parent) {parent=new std::vector<std::pair<KigsID,CoreModifiableAttribute*>>();} \
	FOR_EACH(ADD_MAPPED_ATTRIBUTE, __VA_ARGS__)\
	return parent; \
}

// This one is to be used only inside the owner class code
#define AS_WRAPPED(name)	((CoreModifiableAttribute*)(name##WrapAttr))
