#ifndef _COREMODIFIABLE_H
#define _COREMODIFIABLE_H

#include "GenericRefCountedBaseClass.h"
#include "CoreModifiableMacros.h"
#include "robin_hood.h"
#include "usString.h"
#include "TecLibs/Tec3D.h"
#include "SmartPointer.h"

#include <mutex>
#include <memory>
#include <type_traits>
#include <set>
#include <variant>

#ifdef KEEP_XML_DOCUMENT
#include "XML.h"
#include "XMLNode.h"
#endif


#define NOQUALIFIER

#define EXPAND_MACRO_FOR_NUMERIC_TYPES(CVQUALIFIER, REFQUALIFIER, FUNC)\
FUNC(CVQUALIFIER s8 REFQUALIFIER)\
FUNC(CVQUALIFIER s16 REFQUALIFIER)\
FUNC(CVQUALIFIER s32 REFQUALIFIER)\
FUNC(CVQUALIFIER s64 REFQUALIFIER)\
FUNC(CVQUALIFIER u8 REFQUALIFIER)\
FUNC(CVQUALIFIER u16 REFQUALIFIER)\
FUNC(CVQUALIFIER u32 REFQUALIFIER)\
FUNC(CVQUALIFIER u64 REFQUALIFIER)\
FUNC(CVQUALIFIER float REFQUALIFIER)\
FUNC(CVQUALIFIER double REFQUALIFIER)

#define EXPAND_MACRO_FOR_BASE_TYPES(CVQUALIFIER, REFQUALIFIER, FUNC)\
FUNC(CVQUALIFIER bool REFQUALIFIER)\
EXPAND_MACRO_FOR_NUMERIC_TYPES(CVQUALIFIER, REFQUALIFIER, FUNC)

#define EXPAND_MACRO_FOR_STRING_TYPES(CVQUALIFIER, REFQUALIFIER, FUNC)\
FUNC(CVQUALIFIER std::string REFQUALIFIER)\
FUNC(CVQUALIFIER usString REFQUALIFIER)

#define EXPAND_MACRO_FOR_EXTRA_TYPES(CVQUALIFIER, REFQUALIFIER, FUNC)\
FUNC(CVQUALIFIER CoreItem* REFQUALIFIER)\
FUNC(CVQUALIFIER CoreModifiable* REFQUALIFIER)\
FUNC(CVQUALIFIER void* REFQUALIFIER)

class CoreModifiableAttribute;
class AttachedModifierBase;
class CoreTreeNode;

#include "XML.h"
#include "XMLNode.h"
class Timer;
class CoreItem;

// utility class to pack variable argument list in std::vector<CoreModifiableAttribute*>

#if defined(KIGS_TOOLS) || defined(_DEBUG)
#ifndef KEEP_XML_DOCUMENT
#define KEEP_XML_DOCUMENT
#endif
#endif

#define DECLARE_DEFAULT_LINK_NAME
#define DECLARE_LINK_NAME
#define PASS_LINK_NAME(a)


class CorePackage;

struct ExportSettings
{
	CorePackage* current_package = nullptr;
	std::set<std::string> external_files_exported;
	std::string current_xml_file;
	std::vector<std::string> current_unique_id_path;

	size_t export_buffer_attribute_as_external_file_size_threshold = 0;

#ifdef KEEP_XML_DOCUMENT
	XML* current_xml_file_node = nullptr;
#endif

	void PushID(std::string path)
	{
		current_unique_id_path.push_back(std::move(path));
	}

	void PopID()
	{
		current_unique_id_path.pop_back();
	}

	std::string GetCurrentUniqueID()
	{
		std::string result;
		for (auto& s : current_unique_id_path)
			result += s;
		return result;
	}

};

struct WeakRef
{
	WeakRef() = default;
	WeakRef(const WeakRef& copy_that)
	{
		*this = copy_that;
	}
	WeakRef(WeakRef&& move_that)
	{
		*this = std::move(move_that);
	}
	WeakRef& operator=(const WeakRef& copy_that);
	WeakRef& operator=(WeakRef&& move_that);
	WeakRef(CoreModifiable* item)
	{
		*this = item;
	}
	WeakRef& operator=(CoreModifiable* item);

	bool operator==(const WeakRef& other) const { return to == other.to; }
	bool operator!=(const WeakRef& other) const { return to != other.to; }
	bool operator<(const WeakRef& other) const { return to < other.to; }
	
	CoreModifiable* Pointer() const { return to; }

	~WeakRef();
	void ItemIsBeingDeleted();
	SmartPointer<CoreModifiable> Lock() const;
	bool IsValid() const { return alive && to; }
private:
	bool alive = false;
	CoreModifiable* to = nullptr;
};


namespace std
{
	template <>
	struct hash<WeakRef>
	{
		std::size_t operator()(const WeakRef& k) const
		{
			return std::hash<CoreModifiable*>{}(k.Pointer());
		}
	};
}

struct LazyContent;
struct ModifiableMethodStruct;
class UpgradorBase;

typedef SmartPointer<CoreModifiable> CMSP;

class ModifiableItemStruct
{
protected:

	enum
	{
		EmptyFlag = 0,
		AgreggateFlag = 1,
	};

public:
	ModifiableItemStruct(const CMSP& item) : myItem(item), myItemFlag(0) {}

	operator CMSP() { return myItem; }
	CoreModifiable* operator->() const { return (CoreModifiable*)myItem.get(); }
	CoreModifiable& operator*() const { return *((CoreModifiable*)myItem.get()); }

	CMSP myItem;

	inline bool	isAggregate() const
	{
		return myItemFlag&(u32)AgreggateFlag;
	}
	inline void	setAggregate()
	{
		myItemFlag |= (u32)AgreggateFlag;
	}
	inline void	unsetAggregate()
	{
		myItemFlag &= 0xFFFFFFFF ^ (u32)AgreggateFlag;
	}

private:
	// flag used to store item or link type (item, aggregate...)
	u32	myItemFlag;
};

typedef	const int	CoreMessageType;


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
	const std::vector<TwoNames>& classNames() const { return myClassNames; }

protected:
	std::vector<TwoNames> myClassNames;
};



// generic flag
#define InitFlag							(1U)
#define PostDestroyFlag						(2U)
#define RecursiveUpdateFlag					(4U)
#define NotificationCenterRegistered		(8U)
#define ReferenceRegistered					(16U)
#define AutoUpdateRegistered				(32U)
#define AggregateParentRegistered			(64U)
#define AggregateSonRegistered				(128U)
#define AllowChanges						(256U)

// more generic UserFlags
constexpr u32 UserFlagsBitSize = 16;
constexpr u32 UserFlagsShift = (32 - UserFlagsBitSize);
// user flag is set at 0xFF000000
constexpr u32 UserFlagsMask = 0xFFFFFFFF<< UserFlagsShift;

#define SIGNAL_ARRAY_CONTENT(a) #a, 
#define SIGNAL_ENUM_CONTENT(a) a, 
#define SIGNAL_PUSH_BACK(a) signals.push_back(#a);

#define SIGNAL_CASE(a) case Signals::a: return CoreModifiable::EmitSignal(#a, std::forward<T>(params)...); break;

#define SIGNALS(...)\
enum class Signals : u32\
{\
	FOR_EACH(SIGNAL_ENUM_CONTENT, __VA_ARGS__)\
};\
template<typename... T>\
inline bool EmitSignal(Signals signal,  T&&... params){\
switch(signal){\
FOR_EACH(SIGNAL_CASE, __VA_ARGS__)\
default: break;} return false;\
}\
virtual std::vector<KigsID> GetSignalList() override\
{\
    auto signals = ParentClassType::GetSignalList();\
    FOR_EACH(SIGNAL_PUSH_BACK, __VA_ARGS__)\
    return signals;\
}

#define SIGNALS_BASE(...)\
enum class Signals : u32\
{\
	FOR_EACH(SIGNAL_ENUM_CONTENT, __VA_ARGS__)\
};\
template<typename... T>\
inline bool EmitSignal(Signals signal,  T&&... params){\
switch(signal){\
FOR_EACH(SIGNAL_CASE, __VA_ARGS__)\
default: break;} return false;\
}\
virtual std::vector<KigsID> GetSignalList()\
{\
    std::vector<KigsID> signals;\
    FOR_EACH(SIGNAL_PUSH_BACK, __VA_ARGS__)\
    return signals;\
}




// ****************************************
// * CoreModifiable class
// * --------------------------------------
/**
 * \file	CoreModifiable.h
 * \class	CoreModifiable
 * \ingroup KigsCore
 * \brief	 base class for objects with a list of modifiable attributes and a list of sons (items)
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
 // ****************************************
class CoreModifiable : public GenericRefCountedBaseClass
{
public:
	//DECLARE_ABSTRACT_CLASS_INFO(CoreModifiable, RefCountedClass, KigsCore);
	static const KigsID myClassID;
	static KigsID myRuntimeType;
	typedef bool (CoreModifiable::* ModifiableMethod)(CoreModifiable* sender, std::vector<CoreModifiableAttribute*>&, void* privateParams);
	typedef CoreModifiable CurrentClassType;
	typedef GenericRefCountedBaseClass ParentClassType;
	virtual bool Call(CoreModifiable::ModifiableMethod method, CoreModifiable* sender, std::vector<CoreModifiableAttribute*>& attr, void* privateParams)
	{
		CoreModifiable::ModifiableMethod currentmethod = static_cast<CoreModifiable::ModifiableMethod>(method);
		return (this->*(currentmethod))(sender, attr, privateParams);
	}
	static CoreModifiable* Get()
	{
		return GetFirstInstance("CoreModifiable", false);
	}
	static CoreModifiable* Get(const std::string& name)
	{
		return GetFirstInstanceByName("CoreModifiable", name, false);
	}

	DECLARE_GetRuntimeTypeBase(CoreModifiable);
	DECLARE_getExactTypeBase(CoreModifiable);
	virtual KigsID getExactTypeID() const { return CoreModifiable::myClassID; }
	virtual bool isSubType(const KigsID& cid) const { return CoreModifiable::myClassID == cid; }
	static void GetClassNameTree(CoreClassNameTree& classNameTree);

	static void GetMethodTable(std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& method_table) {}
	static void GetNotWrappedMethodTable(std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& method_table) {}


	static std::vector<CMSP> FindInstances(const KigsID& id, bool exact_type_only = false);
	static std::vector<CMSP> FindInstancesByName(const KigsID& id, const std::string& name, bool exact_type_only = false);
	static CMSP FindFirstInstance(const KigsID& id, const std::string& name, bool exact_type_only = false);
	static CMSP FindFirstInstanceByName(const KigsID& id, const std::string& name, bool exact_type_only = false);

	static void GetInstances(const KigsID& cid, std::set<CoreModifiable*>& instances, bool exactTypeOnly = false, bool only_one = false, bool getref = false);
	static CoreModifiable* GetFirstInstance(const KigsID& cid, bool exactTypeOnly = false, bool getref = false);
	static void GetInstanceByRuntimeID(const std::string& runtimeID, std::set<CoreModifiable*>& instances, bool getref = false);
	static void GetRootInstances(const KigsID& cid, std::set<CoreModifiable*>& instances, bool exactTypeOnly = false, bool getref = false);
	static void GetInstancesByName(const KigsID& cid, const std::string& name, std::set<CoreModifiable*>& instances, bool exactTypeOnly = false, bool only_one = false, bool getref = false);
	static CoreModifiable* GetFirstInstanceByName(const KigsID& cid, const std::string& name, bool exactTypeOnly = false, bool getref = false);


	const std::string& getName() const { return myName; }
	void setName(const std::string& name);
	const KigsID& getNameID() const { return myNameID; }
	inline unsigned int getUID() { return myUID; }
	CoreTreeNode* GetTypeNode() const { return myTypeNode; }

#ifdef KIGS_TOOLS
	void GetRef() override;
	bool TryGetRef() override;
	void Destroy() override;
#endif
	// Don't call this manually!
	void RegisterToCore();

protected:
	explicit CoreModifiable(std::string name, DECLARE_CLASS_NAME_TREE_ARG) : GenericRefCountedBaseClass() 
	{
		myUID = myUIDCounter.fetch_add(1);
		if (name.empty())
			name = "nobody" + std::to_string(myUID);
		myName = name;
		myNameID = name;
	}
	virtual ~CoreModifiable();

	struct ImportState;
public:	
	SIGNALS_BASE(PreInit,
		PostInit,
		Uninit,
		Destroy,
		Update, // Called before the actual update
		NotifyUpdate,
		AddItem,
		RemoveItem,
		PrepareExport,
		EndExport);

	// List of attribute types
	enum class ATTRIBUTE_TYPE
	{
		BOOL,
		CHAR,
		SHORT,
		INT,
		LONG,
		UCHAR,
		USHORT,
		UINT,
		ULONG,
		FLOAT,
		DOUBLE,
		STRING,
		ARRAY,
		ENUM,
		REFERENCE,
		USSTRING, // u16 string for UTF-16 style
		COREITEM,
		COREBUFFER,
		RAWPTR,
		LUAREF,
		ANY,
		UNKNOWN = 999
	};
	// Position when adding to items
	enum ItemPosition
	{
		First = 0,
		Last = -1
	};

	template<typename T>
	T* as() const
	{
		//KIGS_ASSERT(isSubType(T::myClassID));
		static_assert(std::is_base_of<CoreModifiable, T>::value, "using as but type is not a coremodifiable");
		return (T*)static_cast<const T*>(this);
	}

	/// Modifiable managmenent
	// Call InitModifiable
	void Init();

	// Call UninitModifiable
	void UnInit() { UninitModifiable(); }

	// Call UnInit then Init
	void ReInit() { UnInit(); Init(); }

	// Call InitModifiable on childs recursively
	void RecursiveInit(bool a_childInFirst = false);

	// true if the modifiable is correctly initialized
	bool IsInit() { return isInitFlagSet(); }

	// Call Update on aggregate sons then call Update
	void CallUpdate(const Timer& timer, void* addParam);

	// Call Update recursively
	void RecursiveUpdate(const Timer&  timer, CoreModifiable* a_parent = NULL);

	// Clear the item list
	void EmptyItemList();

	// Get the item list
	const std::vector<ModifiableItemStruct>& getItems() const { return _items; }

	// Get the attribute list
	const kigs::unordered_map<KigsID, CoreModifiableAttribute*>& getAttributes() const { return _attributes; }
	
	// Number of attributes
	size_t getAttributeCount() const { return _attributes.size(); }
	
	// Get parents/users
	const std::vector<CoreModifiable*>& GetParents() const { return _users; }
	
	// Get parents/users count
	size_t GetParentCount() const { return _users.size(); }

	// Get the first parent matching the ID
	CoreModifiable* getFirstParent(KigsID ParentClassID) const;

	CoreModifiable* getRootParentByType(KigsID ParentClassID) const
	{
		int foundLevel = -1;
		return recursiveGetRootParentByType(ParentClassID, 0, foundLevel);
	}

	// Return true if the modifiable has a least one parent/user
	bool isUsed() const { return !_users.empty(); }

	// Search in sons for by id and name
	void GetSonInstancesByName(KigsID cid, const std::string &name, std::set<CoreModifiable*>& instances, bool recursive = false, bool getRef = false);

	// Search in sons for by id and name, return the first one
	CoreModifiable* GetFirstSonByName(KigsID cid, const std::string &name, bool recursive = false);

	template<typename T>
	T* GetSon(const std::string& name, bool recursive = false)
	{
		return GetFirstSonByName(T::myClassID, name, recursive)->template as<T>();
	}

	template<typename T>
	T* GetSon(const char* name, bool recursive = false)
	{
		return GetFirstSonByName(T::myClassID, name, recursive)->template as<T>();
	}

	// Search in sons for by id
	void GetSonInstancesByType(KigsID cid, std::set<CoreModifiable*>& instances, bool recursive = false);

	// Search in sons for by id, return the first one
	CoreModifiable* GetFirstSonByType(KigsID cid, bool recursive = false);

	template<typename T>
	T* GetSon(bool recursive = false)
	{
		return GetFirstSonByType(T::myClassID, recursive)->template as<T>();
	}

	// Search in sons for by path
	CoreModifiable* GetInstanceByPath(const std::string &path, bool getRef = false);

	// Update attributes with another modifiable attributes
	void UpdateAttributes(const CoreModifiable& tocopy);

	// Call a method.
	bool SimpleCall(KigsID methodNameID);
	// Call a method with a set of arguement. Need to include AttributePacking.h
	template<typename Ret, typename... T>
	Ret SimpleCall(KigsID methodName, T&&... params);

	template<typename Ret>
	Ret SimpleCall(KigsID methodName);

	template<typename Ret, typename... T>
	Ret InvokeReturn(KigsID methodName, T&&... params)
	{
		return SimpleCall<Ret, T...>(methodName, std::forward<T>(params)...);
	}

	template<typename... T>
	bool SimpleCall(KigsID methodName, T&&... params);



	// Emit a signal
	bool EmitSignal(const KigsID& signalID);
	// Emit a signal with a set of arguement. Need to include AttributePacking.h
	template<typename... T>
	bool EmitSignal(const KigsID& signalID, T&&... params);
	//  Emit a signal to CoreModifiable methods
	bool EmitSignal(const KigsID& signalID, std::vector<CoreModifiableAttribute*>& params, void* privateParams = 0);

	// Avoid using ! call a method, with a list of CoreModifiableAttribute as parameters
	bool CallMethod(KigsID methodNameID, std::vector<CoreModifiableAttribute*>& params, void* privateParams = 0, CoreModifiable* sender = 0);
	// Avoid using ! call a meethod with the list of CoreModifiableAttribute of the given CoreModifiable as parameter
	bool CallMethod(KigsID methodNameID, CoreModifiable * params, void* privateParams = 0, CoreModifiable * sender = 0) {

		std::vector<CoreModifiableAttribute*> p;
		if (params)
		{
			p = (std::vector<CoreModifiableAttribute*>) (*params);
		}

		return CallMethod(methodNameID, p, privateParams, sender);
	}
	// Avoid using ! call a method, with the list of CoreModifiableAttribute of the given CoreModifiable as parameter
	bool CallMethod(KigsID methodNameID, CoreModifiable& params, void* privateParams = 0, CoreModifiable* sender = 0) {
		std::vector<CoreModifiableAttribute*> p = (std::vector<CoreModifiableAttribute*>) params;
		return CallMethod(methodNameID, p, privateParams, sender); }



	/// ID
	// RuntimeID is name:runtimeType:pointer:UID
	std::string	GetRuntimeID() const;


	/// Attribute management
	// Get an attribute by id
	CoreModifiableAttribute* getAttribute(KigsID labelid) const;

	bool setReadOnly(KigsID id, bool val = true);

	// Return array element count for an attribute
	u32 getNbArrayElements(KigsID attributeLabel) const;

	#define DECLARE_SET_VALUE(type) bool setValue(KigsID attributeLabel, type value);
	EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SET_VALUE);
	EXPAND_MACRO_FOR_STRING_TYPES(const, &, DECLARE_SET_VALUE);
	EXPAND_MACRO_FOR_EXTRA_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SET_VALUE);
	DECLARE_SET_VALUE(const char*);
	DECLARE_SET_VALUE(const u16*);

	#define DECLARE_GET_VALUE(type) bool getValue(const KigsID attributeLabel, type value) const;
	EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, DECLARE_GET_VALUE);
	EXPAND_MACRO_FOR_STRING_TYPES(NOQUALIFIER, &, DECLARE_GET_VALUE);
	EXPAND_MACRO_FOR_EXTRA_TYPES(NOQUALIFIER, &, DECLARE_GET_VALUE);
	

	#define DECLARE_SETARRAY_VALUE2(valuetype) bool setArrayValue(KigsID attributeLabel, valuetype value1, valuetype value2);
	EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SETARRAY_VALUE2);

	#define DECLARE_SETARRAY_VALUE3(valuetype) bool setArrayValue(KigsID attributeLabel, valuetype value1,valuetype value2,valuetype value3);
	EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SETARRAY_VALUE3);

	#define DECLARE_SETARRAY_VALUE4(valuetype) bool setArrayValue(KigsID attributeLabel, valuetype value1,valuetype value2,valuetype value3,valuetype value4);
	EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SETARRAY_VALUE4);


	#define DECLARE_SET_ARRAY_VALUE(type) bool setArrayValue(KigsID attributeLabel, type value, u32 nbElements);
	#define DECLARE_GET_ARRAY_VALUE(type) bool getArrayValue(const KigsID labelid, type value, u32 nbElements) const;

	#define DECLARE_SET_ARRAY_ELEMENT(type) bool setArrayElementValue(KigsID, type value, u32 line, u32 column);
	#define DECLARE_GET_ARRAY_ELEMENT(type) bool getArrayElementValue(const KigsID, type value, u32 line, u32 column) const;

	EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, *, DECLARE_GET_ARRAY_VALUE);
	EXPAND_MACRO_FOR_BASE_TYPES(const, *, DECLARE_SET_ARRAY_VALUE);

	EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, DECLARE_GET_ARRAY_ELEMENT);
	EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SET_ARRAY_ELEMENT);

	DECLARE_GET_ARRAY_ELEMENT(std::string&);
	DECLARE_SET_ARRAY_ELEMENT(const std::string&);

#define DECLARE_SET_VALUE_VECTOR(type, nb) bool setValue(KigsID attributeLabel, type vec){ return setArrayValue(attributeLabel, &vec[0], nb); }
#define DECLARE_GET_VALUE_VECTOR(type, nb) bool getValue(const KigsID attributeLabel, type& vec) const { return getArrayValue(attributeLabel, &vec[0], nb); }
#define DECLARE_GET_SET_VALUE_VECTOR(type, nb) DECLARE_SET_VALUE_VECTOR(type, nb) DECLARE_GET_VALUE_VECTOR(type, nb)

	DECLARE_GET_SET_VALUE_VECTOR(v2f, 2);
	DECLARE_GET_SET_VALUE_VECTOR(v3f, 3);
	DECLARE_GET_SET_VALUE_VECTOR(v4f, 4);
	DECLARE_GET_SET_VALUE_VECTOR(Quaternion, 4);

	DECLARE_GET_SET_VALUE_VECTOR(v2i, 2);
	DECLARE_GET_SET_VALUE_VECTOR(v3i, 3);
	

	template<typename T>
	T getValue(const KigsID id) const
	{
		T val;
		getValue(id, val);
		return val;
	}

	template<typename T>
	void setValueRecursively(KigsID id, T&& value)
	{
		setValue(id, value);
		for (auto& it : getItems()) 
			it.myItem->setValueRecursively(id, value);
	}

	template<typename T, typename Pred>
	void setValueRecursively(KigsID id, T&& value, Pred&& predicate)
	{
		if(predicate(this))
			setValue(id, value);
		for (auto& it : getItems())
			it.myItem->setValueRecursively(id, value, std::forward<decltype(predicate)>(predicate));
	}

	/// Dynamic attribute management
	template<typename attribute_type, typename value_type>
	attribute_type*	AddDynamicAttribute(KigsID ID, const value_type& value);
	CoreModifiableAttribute*	AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID, s32 defaultval);
	CoreModifiableAttribute*	AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID, u64 defaultval);
	CoreModifiableAttribute*	AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID, kfloat defaultval);
	CoreModifiableAttribute*	AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID, bool defaultval);
	CoreModifiableAttribute*	AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID, const char* defaultval = 0);

	CoreModifiableAttribute*	AddDynamicVectorAttribute(KigsID ID, const int* defaultval, u32 valcount);
	CoreModifiableAttribute*	AddDynamicVectorAttribute(KigsID ID, const kfloat* defaultval, u32 valcount);

	void RemoveDynamicAttribute(KigsID id);


	/// Method management
	// Return the map of private methods of this CoreModifiable
	const kigs::unordered_map <KigsID, ModifiableMethodStruct>* GetMethods();
	// Search in private methods, type methods and aggregate methods
	bool HasMethod(const KigsID& methodNameID) const;
	// Add a new private method
	void InsertMethod(KigsID labelID, CoreModifiable::ModifiableMethod method, const std::string& methodName = "" CONNECT_PARAM_DEFAULT);
	// Add a new private method
	void InsertUpgradeMethod(KigsID labelID, CoreModifiable::ModifiableMethod method, UpgradorBase* up);

	// Add a new lambda
	template<typename F>
	void InsertFunction(KigsID labelID, F&& func);
	template<typename F>
	void InsertFunctionNoUnpack(KigsID labelID, F&& func);
	// Add a new free function
	template<typename Ret, typename ... Args>
	void InsertFunction(KigsID labelID, Ret(*func)(Args...));

	// Remove a private methods
	void RemoveMethod(KigsID labelID);

	void AddWeakRef(WeakRef* ref);
	void RemoveWeakRef(WeakRef* ref);

	/// Aggregate management
	// Adds a new son as aggregate
	bool aggregateWith(CMSP& item, ItemPosition pos = Last);
	// Removes a son as aggregate
	bool removeAggregateWith(CMSP& item);
	// Recursive search all aggregate sons for an aggregate of the given type  
	CoreModifiable*	getAggregateByType(KigsID id);
	// Search parent to see if one of them is aggregate root of this, then recurse
	CoreModifiable*	getAggregateRoot() const;
	// Aggregate cast search in all aggregate if the current type can be found else returns nullptr 
	template<typename T>
	static T* aggregate_cast(CoreModifiable* tocast)
	{
		CoreModifiable*	startPoint = tocast->getAggregateRoot();
		CoreModifiable* found = startPoint->getAggregateByType(T::myClassID);
		if (found)
		{
			return (T*)found;
		}
		return nullptr;
	}
	
	/// Flags
	inline void flagAsNotificationCenterRegistered()
	{
		_ModifiableFlag |= (u32)NotificationCenterRegistered;
	}
	inline void unflagAsNotificationCenterRegistered()
	{
		_ModifiableFlag &= 0xFFFFFFFF ^ (u32)NotificationCenterRegistered;
	}
	inline bool isFlagAsNotificationCenterRegistered()
	{
		return (_ModifiableFlag&(u32)NotificationCenterRegistered) != 0;
	}
	inline void flagAsReferenceRegistered()
	{
		_ModifiableFlag |= (u32)ReferenceRegistered;
	}
	inline void unflagAsReferenceRegistered()
	{
		_ModifiableFlag &= 0xFFFFFFFF ^ (u32)ReferenceRegistered;
	}
	inline bool isFlagAsReferenceRegistered()
	{
		return (_ModifiableFlag&(u32)ReferenceRegistered) != 0;
	}
	inline void flagAsAutoUpdateRegistered()
	{
		_ModifiableFlag |= (u32)AutoUpdateRegistered;
	}
	inline void unflagAsAutoUpdateRegistered()
	{
		_ModifiableFlag &= 0xFFFFFFFF ^ (u32)AutoUpdateRegistered;
	}
	inline bool isFlagAsAutoUpdateRegistered()
	{
		return (_ModifiableFlag&(u32)AutoUpdateRegistered) != 0;
	}
	inline void flagAsPostDestroy()
	{
		_ModifiableFlag |= (u32)PostDestroyFlag;
	}
	inline void unflagAsPostDestroy()
	{
		_ModifiableFlag &= 0xFFFFFFFF ^ (u32)PostDestroyFlag;
	}
	inline bool isFlagAsPostDestroy()
	{
		return (_ModifiableFlag & (u32)PostDestroyFlag) != 0;
	}
	inline void setUserFlag(u32 flag)
	{
		_ModifiableFlag |= ((u32)flag) << UserFlagsShift;
	}
	inline void unsetUserFlag(u32 flag)
	{
		_ModifiableFlag &= 0xFFFFFFFF ^ (((u32)flag) << UserFlagsShift);
	}
	inline bool isUserFlagSet(u32 flag)
	{
		return ((_ModifiableFlag&(((u32)flag) << UserFlagsShift)) != 0);
	}
	inline bool isInitFlagSet() { return ((_ModifiableFlag&((u32)InitFlag)) != 0); }

	inline void flagAllowChanges()
	{
		_ModifiableFlag |= (u32)AllowChanges;
	}
	inline void unflagAllowChanges()
	{
		_ModifiableFlag &= 0xFFFFFFFF ^ (u32)AllowChanges;
	}
	inline bool isFlagAllowChanges()
	{
		return (_ModifiableFlag&(u32)AllowChanges) != 0;
	}

	// for some type of classes when we want don't want duplicated instances (textures, shaders...)
	// return an already existing instance equivalent of this
	virtual CMSP	getSharedInstance()
	{
		return CMSP(this, GetRefTag{});
	}

	/// Link management
#ifdef USE_LINK_TYPE
	// List of declared link types
	const std::vector<KigsID>* getDeclaredLinkTypes() const { return myLinkType; }

	// Get the link type for an item by index
	int	getItemLinkTypeIndex(int itemIndex) const
	{
		if ((int)_items.size() > itemIndex)
		{
			return _items[(u32)itemIndex].myLinkType;
		}
		return -1;
	}
	// Get the link type name for an item by index
	KigsID getItemLinkTypeName(int itemIndex) const;
#endif


	/// Utility
	//@TODO check for usage
	void InitParametersFromList(const std::vector<CoreModifiableAttribute*>* params);
	// cast operator, returns the attribute list 
	operator std::vector<CoreModifiableAttribute*>();
	//@TODO check for usage
	bool ImportAttributes(const std::string &filename);
	
	// Init the modifiable and set the _isInit flag if OK. Need to call ParentClassType::InitModifiable() when overriding !
	virtual	void InitModifiable();

	// Called when init has failed. Need to call ParentClassType::UninitModifiable() when overriding !
	virtual	void UninitModifiable();
	
	// Called before the object is deleted. 
	virtual void ProtectedDestroy();
	
	// Update method. Call to ParentClassType::Update is not necessary when overriding
	virtual void Update(const Timer&  timer, void* addParam) {}

	// add the given parent to list. Need to call ParentClassType::addUser(...) when overriding !
	virtual void addUser(CoreModifiable* user);

	// remove the given parent from list. Need to call ParentClassType::removeUser(...) when overriding !
	virtual void removeUser(CoreModifiable* user);
	
	// add a son. Need to call ParentClassType::addItem(...) when overriding !
	virtual bool addItem(const CMSP& item, ItemPosition pos = Last);
	
	// remove a son. Need to call ParentClassType::removeItem(...) when overriding !
	virtual bool removeItem(const CMSP& item);

	// Called when an attribute that has its notification level set to Owner is modified. Need to call ParentClassType::NotifyUpdate(...) when overriding !
	virtual void NotifyUpdate(const u32 labelid);

	// By default Two modifiables are equals if they are the same type and attributes are equal. Free to override as needed
	virtual bool Equal(const CoreModifiable& other);
	virtual void SpecificReInit() {}

	//@TODO check for usage
	virtual void RefreshAllAttributes() {}
	
	// give a chance to the object to add dynamic attribute to its export
	virtual void PrepareExport(ExportSettings* settings) {}

	// called after the object was exported (remove dynamics added before export)
	virtual void EndExport(ExportSettings* settings) {}

	auto& GetMutex() const { return mObjectMutex; }

	// CDATA Export/Import API
	virtual int	HasCDataToExport() { return 0; }
	virtual bool GetCDataToExport(int /*index*/, std::string&/* toexport*/) { return false; }
	virtual void ImportFromCData(const std::string&/* imported*/) {}
	
	// Search an attribute list for a specific attribute
	static CoreModifiableAttribute*	getParameter(const std::vector<CoreModifiableAttribute*>& params, KigsID ID);
	static void Append(std::string &XMLString, const std::list<CoreModifiable*> &toexport, bool recursive, ExportSettings* settings = nullptr);
	
	static void	Export(const std::string &filename, CoreModifiable* toexport, bool recursive = false, ExportSettings* settings = nullptr);
	static XMLNode* ExportToXMLNode(CoreModifiable* toexport, XML* owner_xml_file, bool recursive, ExportSettings* settings = nullptr);
	static std::string ExportToXMLString(CoreModifiable* toexport, bool recursive, ExportSettings* settings=nullptr);
	
	static void	Export(std::string &XMLString, const std::list<CoreModifiable*> &toexport, bool recursive, ExportSettings* settings = nullptr);

public:
	static CMSP	Import(const std::string &filename, bool noInit = false, bool keepImportFileName = false, ImportState* state = nullptr, const std::string& override_name="");
	
	template<typename StringType>
	static CMSP	Import(std::shared_ptr<XMLTemplate<StringType> > xmlfile, const std::string &filename, bool noInit = false, bool keepImportFileName = false, ImportState* state = nullptr, const std::string& override_name = "");

	static CoreModifiable* Find(const std::list<CoreModifiable*> &List, const std::string &Name);
	static CoreModifiable* FindByType(const std::list<CoreModifiable*> &List, const std::string& type);
	
	static CoreModifiable* GetInstanceByGlobalPath(const std::string &path, bool getRef = false);
	static CMSP FindInstanceByGlobalPath(const std::string& path);

	// Search instance with infos (either type:name or a path)
	static CoreModifiable* SearchInstance(const std::string &infos, CoreModifiable* searchStart = nullptr, bool getRef = false);
	static CMSP SearchInstanceSP(const std::string& infos, CoreModifiable* searchStart = nullptr);

	void getRootParentsWithPath(std::string &remainingpath, std::vector<CoreModifiable*>& parents, bool getRef = false);

	// Get the first name in a path (IWantThis/The/Remaining/Path). Return "/" if the path starts a the root
	static  std::string GetFirstNameInPath(const std::string &path, std::string & remainingpath);

	/**
	* \fn			inline returnType		getTypedValueMethod(u32 ID, returnType*) const
	*/
	/*
	template<typename returnType>
	inline returnType		getTypedValueMethod(u32 ID, returnType*) const
	{
	returnType tmp;
	getValue(ID, tmp);
	return tmp;
	}*/

	/// Editor

#ifdef KEEP_NAME_AS_STRING
	void RegisterDecorator(const std::string& name);
	void UnRegisterDecorator(const std::string& name);
#endif
#ifdef KEEP_XML_DOCUMENT
	XMLNodeBase* GetXMLNodeForFile(XML* for_file) 
	{ 
		auto it = mXMLNodes.find(for_file);
		if (it != mXMLNodes.end()) 
			return it->second; 
		return nullptr; 
	}
	void DeleteXMLFiles();
#endif

	/// Debug
	static void debugPrintfFullTree(int maxindent = 3);
	static void debugPrintfClassList(const std::string& className, int maxindent = 3, bool onlyOrphan = false);
	void debugPrintfTree(int maxindent) { debugPrintfTree(0, maxindent); }
	void debugPrintfTree(int indent, int maxindent);

	// upgrador management
	void Upgrade(const std::string& toAdd);
	void Downgrade(const std::string& toRemove);

#ifdef KIGS_TOOLS
	bool myTraceRef = false;
#endif
protected:
	// protected upgrador management
	void Upgrade(UpgradorBase* toAdd);

	void Connect(KigsID signal, CoreModifiable* other, KigsID slot CONNECT_PARAM_DEFAULT);
	void Disconnect(KigsID signal, CoreModifiable* other, KigsID slot);

	// check if we can destroy object
	virtual bool checkDestroy() override;

	/// Internals
	void UpdateAggregates(const Timer&  timer, void* addParam);
	const ModifiableMethodStruct* findMethod(const KigsID& id, const CoreModifiable*& localthis) const;
	const ModifiableMethodStruct* recursivefindMethod(const KigsID& id, const CoreModifiable*& localthis) const;
	CoreModifiableAttribute* findAttribute(const KigsID& id) const;
	CoreModifiableAttribute* recursivefindAttribute(const KigsID& id) const;
	
	CoreModifiable* recursiveGetRootParentByType(const KigsID& ParentClassID, int currentlevel,int &foundLevel) const;

	void recursiveGetRootParentsWithPath(const std::string& searchType, const std::string& searchName, std::vector<CoreModifiable*>& parents, bool getRef = false);

	void flagAsAggregateParent()
	{
		_ModifiableFlag |= (u32)AggregateParentRegistered;
	}
	void unflagAsAggregateParent()
	{
		_ModifiableFlag &= 0xFFFFFFFF ^ (u32)AggregateParentRegistered;
	}
	bool isFlagAsAggregateParent() const
	{
		return (_ModifiableFlag&(u32)AggregateParentRegistered) != 0;
	}
	void flagAsAggregateSon()
	{
		_ModifiableFlag |= (u32)AggregateSonRegistered;
	}
	void unflagAsAggregateSon()
	{
		_ModifiableFlag &= 0xFFFFFFFF ^ (u32)AggregateSonRegistered;
	}
	bool isFlagAsAggregateSon() const
	{
		return (_ModifiableFlag&(u32)AggregateSonRegistered) != 0;
	}
	bool isBelongingToAggregate() const
	{
		return ((_ModifiableFlag&((u32)AggregateSonRegistered | (u32)AggregateParentRegistered)) != 0);
	}
	bool checkIfAggregateSon() const
	{

		std::vector<CoreModifiable*>::const_iterator itpc = _users.begin();
		std::vector<CoreModifiable*>::const_iterator itpe = _users.end();

		while (itpc != itpe)
		{
			if ((*itpc)->isFlagAsAggregateParent()) // this parent is an aggregate root
			{
				CoreModifiable* currentAggregate = (*itpc);

				// just check that returnedVal is the searched aggregate
				std::vector<ModifiableItemStruct>::const_iterator	itc = currentAggregate->_items.begin();
				std::vector<ModifiableItemStruct>::const_iterator	ite = currentAggregate->_items.end();

				while (itc != ite)
				{
					if ((*itc).isAggregate()) // if aggregate 
					{
						if ((*itc).myItem == this) // and aggregate son is this
						{
							return true;
						}
					}
					++itc;
				}
			}
			++itpc;
		}
		return false;

	}

	void unregisterFromNotificationCenter();
	void unregisterFromReferenceMap();
	void unregisterFromAutoUpdate();
	void DeleteDynamicAttributes();


	/// Export
	void	Export(std::vector<CoreModifiable*>& savedList, XMLNode * currentNode, bool recursive
		, ExportSettings* settings
#ifdef USE_LINK_TYPE
		, int LinkIDToParent
#endif
		);

	/// Import

	struct ImportState
	{
		struct ToConnect
		{
			CoreModifiable* currentNode;
			std::string sender;
			std::string signal;
			std::string receiver;
			std::string slot;
			std::string setValue;
			XMLNodeBase* xmlattr;
		};

		std::vector<CMSP> loadedItems;
		std::vector<ToConnect> toConnect;

		bool UTF8Enc;
		bool noInit = false;
		bool keepImportFileName = false;
		std::string override_name;
		bool is_include_unique = false;
		XMLBase* current_xml_file = nullptr;
	};

	template<typename StringType>
	static CMSP Import(XMLNodeTemplate<StringType> * currentNode, CoreModifiable* currentModifiable, ImportState& importState);

	// separated import attributes / import sons, so be sure to import attribute first
	template<typename StringType>
	static void ImportAttributes(XMLNodeTemplate<StringType>* currentNode, CoreModifiable* currentModifiable, ImportState& importState, std::vector<XMLNodeBase *>& sons);

	template<typename StringType>
	static void ImportUpgradors(XMLNodeTemplate<StringType>* currentNode, CoreModifiable* currentModifiable, ImportState& importState);

	template<typename StringType>
	static void ImportSons(const std::vector<XMLNodeBase *>& sons, CoreModifiable* currentModifiable, ImportState& importState);

	template<typename StringType>
	static void	InitAttribute(XMLNodeTemplate<StringType>* currentNode, CoreModifiable* currentModifiable, ImportState& importState);

	template<typename StringType>
	static void	InitLuaScript(XMLNodeTemplate<StringType>* currentNode, CoreModifiable* currentModifiable, ImportState& importState);

	template<typename StringType>
	static AttachedModifierBase* InitAttributeModifier(XMLNodeTemplate<StringType>* modifierNode, CoreModifiableAttribute* attr);

	template<typename StringType>
	static CMSP	InitReference(XMLNodeTemplate<StringType>* currentNode, std::vector<CMSP> &loadedItems, const std::string& name);
	
	static bool AttributeNeedEval(const std::string& attr)
	{
		if (attr.size() > 6)
		{
			if (attr.substr(0, 4) == "eval")
			{
				if (attr[attr.size() - 1] == ')')
				{
					return true;
				}
			}
		}
		return false;
	}
	static void	EvalAttribute(std::string& attr, CoreModifiable* owner, CoreModifiableAttribute* destattr = 0);

	//! create and add dynamic attribute except arrays
	CoreModifiableAttribute*	GenericCreateDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID);

	UpgradorBase* GetUpgrador();


#ifdef USE_LINK_TYPE
	/*! add a new link type to the list, second parameter is used to prevent calling it
	 outside constructor
	*/
	void	AddLinkType(KigsID newtype, CLASS_NAME_TREE_ARG)
	{
		if (myLinkType == 0)
		{
			myLinkType = new std::vector<KigsID>();
		}
		// check that this type is not already in the list
		std::vector<KigsID>::iterator it;
		for (it = myLinkType->begin(); it != myLinkType->end(); ++it)
		{
			if ((*it) == newtype)
			{
				return;
			}
		}
		myLinkType->push_back(newtype);
		// avoid warnig 
		(void)classNameTree;
	}
#endif

#ifdef KEEP_XML_DOCUMENT
public:
	kigs::unordered_map<XMLBase*, XMLNodeBase*> mXMLNodes;
	std::vector<std::shared_ptr<XMLBase>> mXMLFiles;
	u32 myEditorFlag = 0;
	LazyContent* GetLazyContentNoCreate() const { return mLazyContent; }
#endif



private:
	friend class CoreModifiableAttribute;
	friend class IMEditor;
	friend class CoreItemSP;
	friend class KigsCore;

	static void	ReleaseLoadedItems(std::vector<CMSP> &loadedItems);

	unsigned int myUID;
	static std::atomic<unsigned int> myUIDCounter;
	CoreTreeNode* myTypeNode = nullptr;
	std::string	myName;
	KigsID myNameID;

	// attribute map
	kigs::unordered_map<KigsID, CoreModifiableAttribute*> _attributes;
	// sons vector
	std::vector<ModifiableItemStruct>				_items;
	// parent vector
	std::vector<CoreModifiable*>					_users;

	
	mutable std::recursive_mutex mObjectMutex;
	mutable LazyContent* mLazyContent = nullptr;

	LazyContent* GetLazyContent() const;

	// Flags
	int _ModifiableFlag =  AllowChanges;

#ifdef KEEP_NAME_AS_STRING
	// keep track of Decorators only on win32 to be able to export them
	std::vector<std::string>*	_Decorators = nullptr;
#endif

#ifdef USE_LINK_TYPE
	// accepted linkTypes
	std::vector<KigsID>*					myLinkType = nullptr;
#endif


};


template<typename attribute_type, typename value_type>
attribute_type*	CoreModifiable::AddDynamicAttribute(KigsID ID, const value_type& value)
{
	attribute_type*	toadd = 0;
	toadd = (attribute_type*)getAttribute(ID);
	if (toadd != nullptr)
	{
		if (toadd->getType() != attribute_type::type) return nullptr;
		*(attribute_type*)toadd = value;
		return toadd;
	}
	toadd = new attribute_type(*this, false, ID, value);
	toadd->setDynamic(true);
	return toadd;
}


#ifdef USE_LINK_TYPE
//! Macro used to declare new link types : Can only be called in class constructor
#define  DECLARE_LINK_TYPE(newtype) AddLinkType(newtype,classNameTree);
#endif

//! Utility Macro to define methods for XML Parser Delegate
#define Declare_XMLDelegate 	DECLARE_METHOD(XMLElementStartDescription); \
DECLARE_METHOD(XMLElementEndDescription); \
DECLARE_METHOD(XMLDeclHandler); \
DECLARE_METHOD(XMLCharacterHandler);

#define XMLDelegateMethods		JSonObjectStart,JSonObjectEnd,JSonArrayStart,JSonArrayEnd,JSonParamList


class PackCoreModifiableAttributes
{
public:
	PackCoreModifiableAttributes(CoreModifiable* owner) :m_owner(owner)
	{
		m_attributeList.clear();
	}

	template<typename T>
	PackCoreModifiableAttributes& operator<<(T&& V);

	void AddAttribute(CoreModifiableAttribute* attr) { m_attributeList.push_back(attr); }

	operator std::vector<CoreModifiableAttribute*>&()
	{
		return m_attributeList;
	}

	~PackCoreModifiableAttributes();

protected:
	std::vector<CoreModifiableAttribute*>	m_attributeList;
	CoreModifiable*							m_owner;
};


#pragma pack(4)
struct ModifiableMethodStruct
{
	using func_type = std::function<bool(CoreModifiable*, CoreModifiable*, std::vector<CoreModifiableAttribute*>&, void*) >;
	struct Method
	{
		CoreModifiable::ModifiableMethod mMethod;
		std::string mName;
		UpgradorBase* mUpgrador = nullptr;
	};

	ModifiableMethodStruct()
	{
	}

	ModifiableMethodStruct(const func_type& func)
	{
		mVariant = func;
	}

	ModifiableMethodStruct(const std::string& name, CoreModifiable::ModifiableMethod method, UpgradorBase* up = nullptr)
	{
		mVariant = Method{ method, name, up };
	}

	std::variant<func_type, Method> mVariant;

	const func_type& GetFunction() const
	{
		return std::get<0>(mVariant);
	}

	const Method& GetMethod() const
	{
		return std::get<1>(mVariant);
	}

	bool IsMethod() const
	{
		return mVariant.index() == 1;
	}

	CONNECT_FIELD
};

#pragma pack()


struct LazyContent
{
	kigs::unordered_map<KigsID, std::vector<std::pair<KigsID, CoreModifiable*>>> ConnectedTo;
	kigs::unordered_map<CoreModifiable*, std::set<std::pair<KigsID, KigsID>>> ConnectedToMe;
	kigs::unordered_map <KigsID, ModifiableMethodStruct> Methods;
	std::vector<WeakRef*> WeakRefs;
	// Upgrador management
	UpgradorBase* myUpgrador = nullptr;

	~LazyContent();
};




// Mainly used for numeric types
template<CoreModifiable::ATTRIBUTE_TYPE type>
struct EnumToType {};

template<>
struct EnumToType<CoreModifiable::ATTRIBUTE_TYPE::BOOL> { using type = bool; };

template<>
struct EnumToType<CoreModifiable::ATTRIBUTE_TYPE::CHAR> { using type = s8; };
template<>
struct EnumToType<CoreModifiable::ATTRIBUTE_TYPE::SHORT> { using type = s16; };
template<>
struct EnumToType<CoreModifiable::ATTRIBUTE_TYPE::INT> { using type = s32; };
template<>
struct EnumToType<CoreModifiable::ATTRIBUTE_TYPE::LONG> { using type = s64; };

template<>
struct EnumToType<CoreModifiable::ATTRIBUTE_TYPE::UCHAR> { using type = u8; };
template<>
struct EnumToType<CoreModifiable::ATTRIBUTE_TYPE::USHORT> { using type = u16; };
template<>
struct EnumToType<CoreModifiable::ATTRIBUTE_TYPE::UINT> { using type = u32; };
template<>
struct EnumToType<CoreModifiable::ATTRIBUTE_TYPE::ULONG> { using type = u64; };

template<>
struct EnumToType<CoreModifiable::ATTRIBUTE_TYPE::FLOAT> { using type = float; };
template<>
struct EnumToType<CoreModifiable::ATTRIBUTE_TYPE::DOUBLE> { using type = double; };

template<>
struct EnumToType<CoreModifiable::ATTRIBUTE_TYPE::STRING> { using type = std::string; };
template<>
struct EnumToType<CoreModifiable::ATTRIBUTE_TYPE::USSTRING> { using type = usString; };


template<typename type>
struct TypeToEnum {};


template<>
struct TypeToEnum<bool> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::ATTRIBUTE_TYPE::BOOL> {};

template<>
struct TypeToEnum<s8> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::ATTRIBUTE_TYPE::CHAR> {};
template<>
struct TypeToEnum<s16> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::ATTRIBUTE_TYPE::SHORT> {};
template<>
struct TypeToEnum<s32> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::ATTRIBUTE_TYPE::INT> {};
template<>
struct TypeToEnum<s64> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::ATTRIBUTE_TYPE::LONG> {};

template<>
struct TypeToEnum<u8> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::ATTRIBUTE_TYPE::UCHAR> {};
template<>
struct TypeToEnum<u16> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::ATTRIBUTE_TYPE::USHORT> {};
template<>
struct TypeToEnum<u32> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::ATTRIBUTE_TYPE::UINT> {};
template<>
struct TypeToEnum<u64> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::ATTRIBUTE_TYPE::ULONG> {};

template<>
struct TypeToEnum<float> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::ATTRIBUTE_TYPE::FLOAT> {};
template<>
struct TypeToEnum<double> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::ATTRIBUTE_TYPE::DOUBLE> {};

template<>
struct TypeToEnum<std::string> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::ATTRIBUTE_TYPE::STRING> {};
template<>
struct TypeToEnum<usString> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::ATTRIBUTE_TYPE::USSTRING> {};



template<typename TypeOut, typename TypeIn>
TypeOut* KigsDynamicCast(TypeIn* obj)
{
	if (obj->isSubType(TypeOut::myClassID))
		return static_cast<TypeOut*>(obj);
	return nullptr;
}

typedef CoreModifiable* (*createMethod)(const std::string& instancename, std::vector<CoreModifiableAttribute*>* args);
void RegisterClassToInstanceFactory(KigsCore* core, const std::string& moduleName, KigsID classID, createMethod method);


#undef DECLARE_SET_VALUE
#undef DECLARE_GET_VALUE
#undef DECLARE_SETARRAY_VALUE2
#undef DECLARE_SETARRAY_VALUE3
#undef DECLARE_SETARRAY_VALUE4
#undef DECLARE_GET_SET_ARRAY_VALUE
#undef DECLARE_GET_SET_ARRAY_ELEMENT
#undef DECLARE_GET_SET_ARRAY_VALUE_AND_ELEMENT

// Backward compatibility
#define  _isInit	isInitFlagSet()

struct MethodCallingStruct
{
	KigsID myMethodID;
	void* myPrivateParams =  nullptr;
	CoreModifiable* myMethodInstance = nullptr;
};

#endif
