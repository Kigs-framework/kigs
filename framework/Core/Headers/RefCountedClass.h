#ifndef _REFCOUNTEDCLASS_H
#define _REFCOUNTEDCLASS_H

#include "RefCountedBaseClass.h"
#include "kstlvector.h"
#include "kstlunordered_map.h"
#include <mutex>
#include <atomic>
#include "robin_hood.h"

class CoreClassNameTree;
class CoreTreeNode;
class RefCountedClass;

#define USE_CLASS_NAME_TREE 0

#if USE_CLASS_NAME_TREE
#define CLASS_NAME_TREE_ARG CoreClassNameTree* classNameTree
#define DECLARE_CLASS_NAME_TREE_ARG CLASS_NAME_TREE_ARG=0
#define PASS_CLASS_NAME_TREE_ARG EnrichClassNameTree(classNameTree, myClassID, myRuntimeType)
#else
#define CLASS_NAME_TREE_ARG kstl::vector<CoreModifiableAttribute*>* args
#define DECLARE_CLASS_NAME_TREE_ARG CLASS_NAME_TREE_ARG=0
#define PASS_CLASS_NAME_TREE_ARG args
#endif



#ifdef KIGS_TOOLS
#include "XMLNode.h"
#define CONNECT_FIELD XMLNodeBase* xmlattr=nullptr;
#define CONNECT_PARAM_DEFAULT ,XMLNodeBase* xmlattr=nullptr
#define CONNECT_PARAM ,XMLNodeBase* xmlattr
#define CONNECT_PASS_PARAM ,xmlattr
#define CONNECT_PASS_MANAGED(a) ,a.xmlattr
#else
#define CONNECT_FIELD
#define CONNECT_PARAM_DEFAULT
#define CONNECT_PARAM
#define CONNECT_PASS_PARAM
#define CONNECT_PASS_MANAGED(a)
#endif




// Requires Modern CPP


// utils defines to declare maMethod

#define DEFINE_METHOD(currentclass,name)  bool currentclass::name(CoreModifiable* sender,kstl::vector<CoreModifiableAttribute*>& params,void* privateParams)


#define DECLARE_METHOD(name)	bool	name(CoreModifiable* sender,kstl::vector<CoreModifiableAttribute*>& params,void* privateParams);\
MethodConstructor name##Constructor = { this, #name, static_cast<RefCountedClass::ModifiableMethod>(&CurrentClassType::name)};
#define DECLARE_VIRTUAL_METHOD(name)	virtual bool	name(CoreModifiable* sender,kstl::vector<CoreModifiableAttribute*>& params,void* privateParams);\
MethodConstructor name##Constructor = { this, #name, static_cast<RefCountedClass::ModifiableMethod>(&CurrentClassType::name)};
#define DECLARE_PURE_VIRTUAL_METHOD(name)	virtual bool	name(CoreModifiable* sender,kstl::vector<CoreModifiableAttribute*>& params,void* privateParams)=0;\
MethodConstructor name##Constructor = { this, #name, static_cast<RefCountedClass::ModifiableMethod>(&CurrentClassType::name)};
#define CONSTRUCT_METHOD(currentclass, name)

#define WRAP_METHOD(name) inline bool	name##Wrap(CoreModifiable* sender,kstl::vector<CoreModifiableAttribute*>& params,void* privateParams){\
	kigs_impl::UnpackAndCall(&CurrentClassType::name, this, sender, params); return false; }\
MethodConstructor name##WrapConstructor = { this, #name, static_cast<RefCountedClass::ModifiableMethod>(&CurrentClassType::name##Wrap)};

#define WRAP_METHOD_NO_CTOR(name) inline bool	name##Wrap(CoreModifiable* sender,kstl::vector<CoreModifiableAttribute*>& params,void* privateParams){\
	kigs_impl::UnpackAndCall(&CurrentClassType::name, this, sender, params); return false; }


#define WRAP_METHODS_CTOR_PARAMS(name) MethodsConstructor::MethodCtorParams(this, #name, static_cast<RefCountedClass::ModifiableMethod>(&CurrentClassType::name##Wrap)), 
//#define WRAP_METHODS(...) FOR_EACH(WRAP_METHOD_NO_CTOR, __VA_ARGS__) MethodsConstructor CONCAT_TOKENS(MultiConstructor_, __COUNTER__) = { FOR_EACH(WRAP_METHODS_CTOR_PARAMS, __VA_ARGS__) };
#define METHOD_PUSH_BACK(name) table.push_back({ #name, static_cast<RefCountedClass::ModifiableMethod>(&CurrentClassType::name##Wrap) });

// Need #include "AttributePacking.h"
#define WRAP_METHODS(...) FOR_EACH(WRAP_METHOD_NO_CTOR, __VA_ARGS__)\
public:\
static void GetMethodTable(kstl::vector<std::pair<KigsID, RefCountedClass::ModifiableMethod>>& table)\
{\
	ParentClassType::GetMethodTable(table);\
	FOR_EACH(METHOD_PUSH_BACK, __VA_ARGS__)\
}


#define DECLARE_OVERRIDE_METHOD(name) virtual bool	name(CoreModifiable* sender,kstl::vector<CoreModifiableAttribute*>& params,void* privateParams) override;\
MethodConstructor name##Constructor = { this, #name, static_cast<RefCountedClass::ModifiableMethod>(&CurrentClassType::name)};




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


#define INSERT_DYNAMIC_METHOD(methodname,callingname) InsertMethod(#callingname,static_cast<RefCountedClass::ModifiableMethod>(&DynamicMethod##methodname::methodname));


//! CoreClassNameTree manage the list of inhereted class names
class CoreClassNameTree
{
public:
	struct	TwoNames
	{
		KigsID myClassName;
		KigsID myRuntimeName;
	};

	// constructors
	CoreClassNameTree(KigsID className, KigsID runtimeName) : myClassNames{} { addClassName(className, runtimeName); }
	CoreClassNameTree() : myClassNames() {}

	CoreClassNameTree(const CoreClassNameTree& tree) = delete;
	CoreClassNameTree& operator=(const CoreClassNameTree& tree) = delete;

	// push class name
	void addClassName(KigsID className, KigsID runtimeName)
	{
		for (const auto& names : myClassNames)
		{
			if (names.myClassName == className)
				return;
		}
		myClassNames.push_back({ className, runtimeName });
	}

	// return classname vector
	const kstl::vector<TwoNames>& classNames() const { return myClassNames; }

protected:
	kstl::vector<TwoNames> myClassNames;
};



// ****************************************
// * RefCountedClass class
// * --------------------------------------
/**
* \file	RefCountedClass.h
* \class	RefCountedClass
* \ingroup KigsCore
* \brief	the base class for classes that need to be refcounted
* \author	ukn
* \version ukn
* \date	ukn
*
* the base class for classes that need to be refcounted and to know
* their instances and inheritance tree
*/
// ****************************************
struct CoreClassMethods;

class RefCountedClass : public RefCountedBaseClass
{
	virtual RefCountedBaseClass& operator=(const RefCountedBaseClass&) { return *this; }
public:
	static const KigsID myClassID; 
	static KigsID myRuntimeType; 
	typedef bool (RefCountedClass::*ModifiableMethod)(CoreModifiable* sender, kstl::vector<CoreModifiableAttribute*>&, void* privateParams); 
	typedef RefCountedClass CurrentClassType; 
	typedef RefCountedBaseClass ParentClassType; 
	virtual bool Call(RefCountedClass::ModifiableMethod method, CoreModifiable* sender, kstl::vector<CoreModifiableAttribute*>& attr, void* privateParams)
	{
		RefCountedClass::ModifiableMethod currentmethod = static_cast<RefCountedClass::ModifiableMethod>(method); 
		return (this->*(currentmethod))(sender, attr, privateParams); 
	}
	DECLARE_GetRuntimeType(RefCountedClass); 
	DECLARE_getExactType(RefCountedClass); 
	virtual KigsID getExactTypeID() const override { return RefCountedClass::myClassID; } 
	virtual bool isSubType(const KigsID& cid) const override { if (RefCountedClass::myClassID == cid)return true;  return RefCountedBaseClass::isSubType(cid); } 
	static void GetClassNameTree(CoreClassNameTree& classNameTree) { RefCountedBaseClass::GetClassNameTree(classNameTree); classNameTree.addClassName(RefCountedClass::myClassID, RefCountedClass::myRuntimeType); }

	static void GetMethodTable(kstl::vector<std::pair<KigsID, RefCountedClass::ModifiableMethod>>& method_table){}

	//! unique ID for an instance
	typedef intptr_t InstanceUniqueID;


	/**
	* \fn 		static RefCountedClass* GetFromInstanceUniqueID(InstanceUniqueID Id) {return (RefCountedClass*) Id;}
	* \brief	find the RefCountedClass with a given ID
	* \param	Id : unique Id of the class to retrieve
	* \return	the RefCountedClass which has Id as InstanceUniqueID
	*/
	static RefCountedClass* GetFromInstanceUniqueID(InstanceUniqueID Id) { return (RefCountedClass*)Id; }

	/**
	* \fn 		InstanceUniqueID GetInstanceUniqueId(void) const {return (InstanceUniqueID)(INT_PTR)this;}
	* \brief	get the InstanceUniqueID for this instance
	* \return	the InstanceUniqueID of the instance
	*/
	InstanceUniqueID GetInstanceUniqueId(void) const { return (InstanceUniqueID)this; }

	/*
	//! cast operator
	virtual operator RefCountedClass*()
	{
		return this;
	};
	*/
	
	/**
	* \fn 		static void GetInstances(const kstl::string &cid,kstl::set<CoreModifiable*>& instances,bool exactTypeOnly=false);
	* \brief	retreive all instances of the given type (exactTypeOnly define if the type should be exact or not)
	* \param	cid : class Id
	* \param	instances : list of instances (out param)
	* \param	exactTypeOnly : if TRUE, only the exact type would be taken
	*/
	static void GetInstances(const KigsID& cid, kstl::set<CoreModifiable*>& instances, bool exactTypeOnly = false, bool only_one = false, bool getref = false);
	static CoreModifiable* GetFirstInstance(const KigsID& cid, bool exactTypeOnly=false, bool getref = false);
	
	/**
	* \fn 		static void GetInstanceByRuntimeID(const kstl::string& runtimeID,kstl::set<CoreModifiable*>& instances);
	* \brief	retreive the instance by runtime ID
	* \param	runtimeID : runtime ID ("name:type:pointer:uid")
	* \param	instances : list of instances (out param)
	*/
	static void GetInstanceByRuntimeID(const kstl::string &runtimeID, kstl::set<CoreModifiable*>& instances, bool getref = false);

	/**
	* \fn 		static void GetRootInstances(const kstl::string &cid,kstl::set<CoreModifiable*>& instances,bool exactTypeOnly=false);
	* \brief	retreive all instances of the given type (exactTypeOnly define if the type should be exact or not) with no parents (root instances)
	* \param	cid : class Id
	* \param	instances : list of instances (out param)
	* \param	exactTypeOnly : if TRUE, only the exact type would be taken
	*/
	static void GetRootInstances(const KigsID& cid, kstl::set<CoreModifiable*>& instances, bool exactTypeOnly = false, bool getref = false);

	/**
	* \fn 		static void GetInstancesByName(const kstl::string &cid,const kstl::string &name,kstl::set<RefCountedClass*>& instances,bool exactTypeOnly=false);
	* \brief	retreive all instances of the given type for the given name (exactTypeOnly define if the type should be exact or not)
	* \param	cid : class Id
	* \param	name : class name
	* \param	instances : list of instances (out param)
	* \param	exactTypeOnly : if TRUE, only the exact type would be taken
	*/
	static void GetInstancesByName(const KigsID& cid, const kstl::string &name, kstl::set<CoreModifiable*>& instances, bool exactTypeOnly = false, bool only_one = false,bool getref=false);
	static CoreModifiable* GetFirstInstanceByName(const KigsID& cid, const kstl::string &name, bool exactTypeOnly = false, bool getref = false);


	// \brief  debug : print the class inheritance tree
	// void printClassNameTree(std::ostream& os) const;

	/**
	* \fn 		const kstl::vector<CoreClassNameTree::TwoNames>& getClassNameTree() const;
	* \brief	retreive the inheritance string list for this class
	* \return	the list of inherited classes
	*/
	const kstl::vector<CoreClassNameTree::TwoNames>& getClassNameTree() const;

	/**
	  * \fn		static kstl::string DefaultName();
	  * \brief	create a default name for a new instance
	  * \return	the default name
	  */
	static kstl::string DefaultName();

	/**
	  * \fn		 kstl::string getName() const {return myName;}
	  * \brief	 return name of the instance
	  * \return	 the name of the instance
	  */
	const kstl::string& getName() const { return myName; }
	const KigsID& getNameID() const { return myNameID; }

	/**
	 * \fn		 void setName(const kstl::string &name) {myName=name;}
	 * \brief	 set new instance name
	 */
	void setName(const kstl::string &name);

	inline unsigned int getUID() { return myUID; }

	virtual kstl::vector<KigsID> GetSignalList() { return {}; }

	// Don't call this manually!
	void RegisterToCore();

	CoreTreeNode* GetTypeNode() const { return myTypeNode; }


	virtual void			Destroy() override;
protected:
	/**
	* \fn 		~RefCountedClass();
	* \brief	destructor
	*/
	virtual ~RefCountedClass()
	{
		ProtectedDestructor();
	}

	void ProtectedDestructor();

	/*! \brief  protected constructor
	*/
	RefCountedClass(const kstl::string& name = DefaultName(), DECLARE_CLASS_NAME_TREE_ARG);


	/*! \brief  used by constructor to add this class to the inheritance tree
	*/
	//static CoreClassNameTree* EnrichClassNameTree(CoreClassNameTree* classNameTree, KigsID classID, KigsID runtimetype);

	

	//! unique ID
	unsigned int			myUID;
	static std::atomic<unsigned int> myUIDCounter;
	CoreTreeNode*			myTypeNode=nullptr;

private:
	
	explicit RefCountedClass(const RefCountedClass& object) = delete;

	//! \brief  the node in inheritance tree for this class
	//CoreTreeNode* myTreeNode;

	//! counter for creating an instance name
	static unsigned int myDefaultNameCounter;

	//! name of this instance
	kstl::string	myName;
	KigsID			myNameID;
};

#pragma pack(4)
struct ModifiableMethodStruct
{

	ModifiableMethodStruct() : m_Function(nullptr), m_Method(nullptr), m_Name("")
	{

	}

	ModifiableMethodStruct(const RefCountedClass::ModifiableMethod& m, const std::string& n) : m_Function(nullptr), m_Method(m), m_Name(n)
	{
		
	}

	ModifiableMethodStruct(const ModifiableMethodStruct& other) : m_Function(nullptr), m_Method(other.m_Method), m_Name("")
	{
		if (other.m_Function)
		{
			m_Name.~basic_string();
			// placement new at reserved UnionFunction
			m_Function = new (&m_UnionFunction) std::function<bool(kstl::vector<CoreModifiableAttribute*>&)>(*other.m_Function);
		}
		else
		{
			m_Name = other.m_Name;
		}
		
	}

	void	setFunction(const std::function<bool(kstl::vector<CoreModifiableAttribute*>&)>& func)
	{
		if (m_Function)
		{
			m_Function->~function();
			m_Function = nullptr;
		}
		else
		{
			m_Name.~basic_string();
		}
		// placement new at reserved UnionFunction
		m_Function = new (&m_UnionFunction) std::function<bool(kstl::vector<CoreModifiableAttribute*>&)>(func);

	}
	
	~ModifiableMethodStruct() {
		if (m_Function)
		{
			m_Function->~function();
			m_Function = nullptr;
		}
		else
		{
			m_Name.~basic_string();
		}
	}

	// as std::function is "big", and if lambda is used, m_Name and m_Method are unused, try to pack everything in a union
	union {
		struct {
			kstl::string						m_Name;
			RefCountedClass::ModifiableMethod	m_Method;
		};
		std::function<bool(kstl::vector<CoreModifiableAttribute*>&)>	m_UnionFunction;
	};
	std::function<bool(kstl::vector<CoreModifiableAttribute*>&)>* m_Function;
	
	CONNECT_FIELD
};

#pragma pack()

// ****************************************
// * CoreTreeNode class
// * --------------------------------------
/**
* \class	CoreTreeNode
* \ingroup KigsCore
* \brief	a class that keep all the instances of a given type
* \author	ukn
* \version ukn
* \date	ukn
*
* a class that keep all the instances of a given type and all the inheritance tree.
* this class is used to search the whole inheritance tree of RefCountedClass
*/
// ****************************************

struct RefCountedClassNameSorter
{
	//! overload operator () for comparison
	bool operator()(const RefCountedClass* a1, const RefCountedClass* a2) const
	{
		int test = a1->getName().compare(a2->getName());

		if (test == 0)
			return ((uptr)a1<(uptr)a2);

		return test<0;
	}
};



class CoreTreeNode
{
public:
	//! constructor
	explicit CoreTreeNode(CoreTreeNode* father, KigsID id) : myChildren(), myInstances(), myFather(father), myID(id) /*, myClassNameTree(0) */ {}
	explicit CoreTreeNode(const CoreTreeNode& node) = delete;
	CoreTreeNode& operator=(const CoreTreeNode& node) = delete;

	~CoreTreeNode();

	void	RemoveInstance(RefCountedClass* toRemove);
	void	AddInstance(RefCountedClass* toAdd);

	/**
	* \fn 		void getTreeNodes(bool OnlyAppendNodesWithInstances, kstl::list<const CoreTreeNode *> &nodes) const;
	* \brief	Append the tree nodes to a list
	* \param	OnlyAppendNodesWithInstances : if TRUE only append node with instance
	* \param	nodes : list of nodes (out param)
	*/
	void getTreeNodes(bool OnlyAppendNodesWithInstances, kstl::list<const CoreTreeNode*>& nodes) const;

	/**
	* \fn 		void getInstances(kstl::set<RefCountedClass*>& instances,bool recursive) const
	* \brief	return the list of instances for this class and son classes if recursive is true
	* \param	instances : class to look for (out param)
	* \param	recursive : if TRUE look for the class in all sons
	*/
	void getInstances(kstl::set<CoreModifiable*>& instances, bool recursive, bool only_one=false,bool getref=false) const;

	/**
	* \fn 		void getRootInstances(kstl::set<RefCountedClass*>& instances,bool recursive) const
	* \brief	return the list of instances for this class and son classes if recursive is true
	* \param	instances : class to look for (out param)
	* \param	recursive : if TRUE look for the class in all sons
	*/
	void getRootInstances(kstl::set<CoreModifiable*>& instances, bool recursive,bool get_ref=false) const;

	size_t GetInstanceCount() const
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

	/**
	* \fn 		void getInstancesByName(kstl::set<CoreModifiable*>& instances,bool recursive,const kstl::string& name) const;
	* \brief	return the list of instances which match the comparator, for this class and sons classes if recursive is true wich match the comparator
	* \param	instances : class to look for (out param)
	* \param	recursive : if TRUE look for the class in all sons
	* \param	comparator : used comparator
	*/
	void getInstancesByName(kstl::set<CoreModifiable*>& instances, bool recursive, const kstl::string& name, bool only_one=false,bool getref=false);

	RefCountedClass::ModifiableMethod GetMethod(KigsID id) const;

	//! map of children
	kstl::unordered_map<KigsID, CoreTreeNode*> myChildren;

	robin_hood::unordered_map <KigsID, ModifiableMethodStruct, KigsIDHash> myMethods;

	//! keep track of instance list for this class type
	//kstl::set<RefCountedClass*, RefCountedClassNameSorter> myInstances;
	kstl::vector< RefCountedClass* >	myInstances;
	bool							    myInstanceVectorNeedSort=false;

	void	sortInstanceVector();
	CoreTreeNode* myFather;
	KigsID myID;

	mutable std::recursive_mutex myMutex;
};



// used when comparing only pointer is hazardous
// assure that if an object is destroyed and another created during same engine loop
// and they get the same adress, the uid will be different
class CheckUniqueObject
{
protected:
	RefCountedClass*		m_Object;
	unsigned int			m_UID;
public:
	bool operator<(const CheckUniqueObject& other) const
	{
		if (m_Object == other.m_Object)
		{
			return m_UID < other.m_UID;
		}
		return m_Object < other.m_Object;
	}
	CheckUniqueObject() :m_Object(0), m_UID((unsigned int)-1)
	{
	}

	CheckUniqueObject(RefCountedClass* node) : m_Object(node), m_UID((unsigned int)-1)
	{
		if (node)
		{
			m_UID = node->getUID();
		}
	}

	CheckUniqueObject(const CheckUniqueObject& other) : m_Object(other.m_Object), m_UID(other.m_UID)
	{
	}

	const CheckUniqueObject&	operator = (RefCountedClass* otherNode)
	{
		m_Object = otherNode;
		m_UID = -1;
		if (m_Object)
		{
			m_UID = m_Object->getUID();
		}
		return *this;
	}

	bool	operator == (RefCountedClass* otherNode)
	{
		if ((m_Object) && (m_Object == otherNode))
		{
			if (otherNode->getUID() == m_UID)
			{
				return true;
			}
		}
		return false;
	}

	bool	operator == (const CheckUniqueObject& otherNode)
	{
		if ((m_Object) && (m_Object == otherNode.m_Object))
		{
			if (otherNode.m_UID == m_UID)
			{
				return true;
			}
		}
		return false;
	}

	operator RefCountedClass*() const
	{
		return m_Object;
	}
};



#endif //_REFCOUNTEDCLASS_H