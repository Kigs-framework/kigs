#ifndef _COREMODIFIABLE_H
#define _COREMODIFIABLE_H

#include "RefCountedClass.h"
#include "robin_hood.h"
#include "usString.h"
#include "TecLibs/Tec3D.h"

#include <mutex>
#include <memory>
#include <type_traits>

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
FUNC(CVQUALIFIER CheckUniqueObject REFQUALIFIER)\
FUNC(CVQUALIFIER CoreItem* REFQUALIFIER)\
FUNC(CVQUALIFIER void* REFQUALIFIER)

class CoreModifiableAttribute;
class AttachedModifierBase;
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


struct LazyContent
{
	std::unordered_map<KigsID, std::vector<std::pair<KigsID, CoreModifiable*>>> ConnectedTo;
	std::unordered_map<CoreModifiable*, std::set<std::pair<KigsID, KigsID>>> ConnectedToMe;
	std::unordered_map<KigsID, ModifiableMethodStruct> Methods;
	std::vector<WeakRef*> WeakRefs;
};

class ModifiableItemStruct
{
protected:

	enum
	{
		EmptyFlag = 0,
		AgreggateFlag = 1,
	};

public:
	ModifiableItemStruct(CoreModifiable* item) : myItem(item), myItemFlag(0) {}

	operator CoreModifiable*() { return myItem; }
	CoreModifiable* operator->() { return myItem; }
	CoreModifiable& operator*() { return *myItem; }

	CoreModifiable* myItem;

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


#define TEST_ALLOWCHANGES_FLAG

// generic flag
#define InitFlag							(1UL)
#define PostDestroyFlag						(2UL)
#define RecursiveUpdateFlag					(4UL)
#define NotificationCenterRegistered		(8UL)
#define ReferenceRegistered					(16UL)
#define AutoUpdateRegistered				(32UL)
#define AggregateParentRegistered			(64UL)
#define AggregateSonRegistered				(128UL)
#ifdef TEST_ALLOWCHANGES_FLAG
#define AllowChanges						(256UL)
#endif
// user flag is set at 0xFF000000
constexpr u64 UserFlags = 0xFF000000UL;

#define SIGNAL_ARRAY_CONTENT(a) #a, 
#define SIGNAL_ENUM_CONTENT(a) a, 
#define SIGNAL_PUSH_BACK(a) signals.push_back(#a);

#define SIGNAL_CASE(a) case Signals::a: return Emit(#a, std::forward<T>(params)...); break;

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
class CoreModifiable : public RefCountedClass
{
	DECLARE_ABSTRACT_CLASS_INFO(CoreModifiable, RefCountedClass, KigsCore);

protected:
	explicit CoreModifiable(std::string name = DefaultName(), DECLARE_CLASS_NAME_TREE_ARG) : RefCountedClass(name, PASS_CLASS_NAME_TREE_ARG) {}
	virtual ~CoreModifiable();

	struct ImportState;
public:	
	SIGNALS(PreInit, 
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
	enum ATTRIBUTE_TYPE
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
	T* as()
	{
		//KIGS_ASSERT(isSubType(T::myClassID));
		static_assert(std::is_base_of<CoreModifiable, T>::value, "using as but type is not a coremodifiable");
		return static_cast<T*>(this);
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

	// Decrement ref count and delete the object if it's no longer used
	void Destroy() final;

	// Call Update on aggregate sons then call Update
	void CallUpdate(const Timer& timer, void* addParam);

	// Call Update recursively
	void RecursiveUpdate(const Timer&  timer, CoreModifiable* a_parent = NULL);

	// Clear the item list
	void EmptyItemList();

	// Get the item list
	const std::vector<ModifiableItemStruct>& getItems() const { return _items; }

	// Get the attribute list
	const robin_hood::unordered_map<KigsID, CoreModifiableAttribute* , KigsIDHash>& getAttributes() const { return _attributes; }
	
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
	void GetSonInstancesByName(KigsID cid, const std::string &name, std::set<CoreModifiable*>& instances, bool recursive = false);

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
	CoreModifiable* GetInstanceByPath(const std::string &path);

	// Update attributes with another modifiable attributes
	void UpdateAttributes(const CoreModifiable& tocopy);

	// Call a method.
	bool SimpleCall(KigsID methodNameID);
	// Call a method with a set of arguement. Need to include AttributePacking.h
	template<typename Ret, typename... T>
	Ret SimpleCall(KigsID methodName, T&&... params);

	template<typename Ret, typename... T>
	Ret InvokeReturn(KigsID methodName, T&&... params)
	{
		return SimpleCall<Ret, T...>(methodName, std::forward<T>(params)...);
	}

	template<typename... T>
	bool SimpleCall(KigsID methodName, T&&... params);

	void Connect(KigsID signal, CoreModifiable* other, KigsID slot CONNECT_PARAM_DEFAULT);
	void Disconnect(KigsID signal, CoreModifiable* other, KigsID slot);

	// Emit a signal
	bool Emit(KigsID methodNameID);
	// Emit a signal with a set of arguement. Need to include AttributePacking.h
	template<typename... T>
	bool Emit(KigsID methodName, T&&... params);

	// Avoid using ! call a method, with a list of CoreModifiableAttribute as parameters
	bool CallMethod(KigsID methodNameID, std::vector<CoreModifiableAttribute*>& params, void* privateParams = 0, CoreModifiable* sender = 0);
	// Avoid using ! call a meethod with the list of CoreModifiableAttribute of the given CoreModifiable as parameter
	bool CallMethod(KigsID methodNameID, CoreModifiable* params, void* privateParams = 0, CoreModifiable* sender = 0) {	return CallMethod(methodNameID, ((std::vector<CoreModifiableAttribute*>&)(*params)), privateParams, sender); }
	// Avoid using ! call a method, with the list of CoreModifiableAttribute of the given CoreModifiable as parameter
	bool CallMethod(KigsID methodNameID, CoreModifiable& params, void* privateParams = 0, CoreModifiable* sender = 0) { return CallMethod(methodNameID, ((std::vector<CoreModifiableAttribute*>&)(params)), privateParams, sender); }
	// Avoid using ! emit a signal, with a list of CoreModifiableAttribute as parameters
	bool CallEmit(KigsID methodNameID, std::vector<CoreModifiableAttribute*>& params, void* privateParams = 0);


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
	DECLARE_SET_VALUE(CoreModifiable*);

	#define DECLARE_GET_VALUE(type) bool getValue(const KigsID attributeLabel, type value) const;
	EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, DECLARE_GET_VALUE);
	EXPAND_MACRO_FOR_STRING_TYPES(NOQUALIFIER, &, DECLARE_GET_VALUE);
	EXPAND_MACRO_FOR_EXTRA_TYPES(NOQUALIFIER, &, DECLARE_GET_VALUE);
	DECLARE_GET_VALUE(CoreModifiable*&);

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

#define DECLARE_SET_VALUE_VECTOR(type, nb) bool setValue(KigsID attributeLabel, type vec){ return setArrayValue(attributeLabel, &vec.x, nb); }
#define DECLARE_GET_VALUE_VECTOR(type, nb) bool getValue(const KigsID attributeLabel, type& vec) const { return getArrayValue(attributeLabel, &vec.x, nb); }
#define DECLARE_GET_SET_VALUE_VECTOR(type, nb) DECLARE_SET_VALUE_VECTOR(type, nb) DECLARE_GET_VALUE_VECTOR(type, nb)

	DECLARE_GET_SET_VALUE_VECTOR(v2f, 2);
	DECLARE_GET_SET_VALUE_VECTOR(v3f, 3);
	DECLARE_GET_SET_VALUE_VECTOR(v4f, 4);

	DECLARE_GET_SET_VALUE_VECTOR(v2i, 2);
	DECLARE_GET_SET_VALUE_VECTOR(v3i, 3);
	

	template<typename T>
	T getValue(const KigsID id) const
	{
		T val;
		getValue(id, val);
		return val;
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
	const std::unordered_map<KigsID, ModifiableMethodStruct>* GetMethods() { if (!mLazyContent) return nullptr; return &GetLazyContent()->Methods; }
	// Search in private methods, type methods and aggregate methods
	bool HasMethod(const KigsID& methodNameID) const;
	// Add a new private method
	void InsertMethod(KigsID labelID, RefCountedClass::ModifiableMethod method, const std::string& methodName = "" CONNECT_PARAM_DEFAULT);
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
	bool aggregateWith(CoreModifiable* item, ItemPosition pos = Last);
	// Removes a son as aggregate
	bool removeAggregateWith(CoreModifiable* item);
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
	void flagAsNotificationCenterRegistered()
	{
		_ModifiableFlag |= (u32)NotificationCenterRegistered;
	}
	void unflagAsNotificationCenterRegistered()
	{
		_ModifiableFlag &= 0xFFFFFFFF ^ (u32)NotificationCenterRegistered;
	}
	bool isFlagAsNotificationCenterRegistered()
	{
		return (_ModifiableFlag&(u32)NotificationCenterRegistered) != 0;
	}
	void flagAsReferenceRegistered()
	{
		_ModifiableFlag |= (u32)ReferenceRegistered;
	}
	void unflagAsReferenceRegistered()
	{
		_ModifiableFlag &= 0xFFFFFFFF ^ (u32)ReferenceRegistered;
	}
	bool isFlagAsReferenceRegistered()
	{
		return (_ModifiableFlag&(u32)ReferenceRegistered) != 0;
	}
	void flagAsAutoUpdateRegistered()
	{
		_ModifiableFlag |= (u32)AutoUpdateRegistered;
	}
	void unflagAsAutoUpdateRegistered()
	{
		_ModifiableFlag &= 0xFFFFFFFF ^ (u32)AutoUpdateRegistered;
	}
	bool isFlagAsAutoUpdateRegistered()
	{
		return (_ModifiableFlag&(u32)AutoUpdateRegistered) != 0;
	}
	void flagAsPostDestroy()
	{
		_ModifiableFlag |= (u32)PostDestroyFlag;
	}
	void setUserFlag(u8 flag)
	{
		_ModifiableFlag |= ((u32)flag) << 24;
	}
	void unsetUserFlag(u8 flag)
	{
		_ModifiableFlag &= 0xFFFFFFFF ^ (((u32)flag) << 24);
	}
	bool isUserFlagSet(u8 flag)
	{
		return ((_ModifiableFlag&(((u32)flag) << 24)) != 0);
	}
	bool isInitFlagSet() { return ((_ModifiableFlag&((u32)InitFlag)) != 0); }

#ifndef TEST_ALLOWCHANGES_FLAG
	inline void flagAllowChanges()
	{
		mAllowChanges = true;
	}
	inline void unflagAllowChanges()
	{
		mAllowChanges = false;
	}
	inline bool isFlagAllowChanges()
	{
		return  mAllowChanges;
	}
#else
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
#endif
	// for some type of classes when we want don't want duplicated instances (textures, shaders...)
	// return an already existing instance equivalent of this
	virtual CoreModifiable*	getSharedInstance()
	{
		return this;
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
	/// Overloadable Methods 
	// Init the modifiable and set the _isInit flag if OK. Need to call base::InitModifiable() when overriding !
	virtual	void InitModifiable();

	// Called when init has failed or when "closing" modifiable. Need to call base::UninitModifiable() when overriding !
	virtual	void UninitModifiable();
	
	// Called before the object in deleted. Need to call base::ProtectedDestroy() when overriding !
	void ProtectedDestroy() override;
	
	// Update method. Call to base::Update is not necessary when overriding
	virtual void Update(const Timer&  timer, void* addParam) {}

	// add the given parent to list. Need to call base::addUser(...) when overriding !
	virtual void addUser(CoreModifiable* user);

	// remove the given parent from list. Need to call base::. Need to call base::addItem(...) when overriding !
	virtual void removeUser(CoreModifiable* user);
	
	// add a son. Need to call base::addItem(...) when overriding !
	virtual bool addItem(CoreModifiable* item, ItemPosition pos = Last);
	
	// remove a son. Need to call base::removeItem(...) when overriding !
	virtual bool removeItem(CoreModifiable* item);

	// Called when an attribute that has its notification level set to Owner is modified. Need to call base::NotifyUpdate(...) when overriding !
	virtual void NotifyUpdate(const u32 labelid);

	// By default Two modifiables are equals if they are the same type and attributes are equal. Free to override as needed
	virtual bool Equal(CoreModifiable& other);
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
	
	/// Static Methods
	// Search an attribute list for a specific attribute
	static CoreModifiableAttribute*	getParameter(const std::vector<CoreModifiableAttribute*>& params, KigsID ID);
	static void Append(std::string &XMLString, const std::list<CoreModifiable*> &toexport, bool recursive, ExportSettings* settings = nullptr);
	
	static void	Export(const std::string &filename, CoreModifiable* toexport, bool recursive = false, ExportSettings* settings = nullptr);
	static XMLNode* ExportToXMLNode(CoreModifiable* toexport, XML* owner_xml_file, bool recursive, ExportSettings* settings = nullptr);
	static std::string ExportToXMLString(CoreModifiable* toexport, bool recursive, ExportSettings* settings=nullptr);
	
	static void	Export(std::string &XMLString, const std::list<CoreModifiable*> &toexport, bool recursive, ExportSettings* settings = nullptr);

public:
	static CoreModifiable*	Import(const std::string &filename, bool noInit = false, bool keepImportFileName = false, ImportState* state = nullptr, const std::string& override_name="");
	
	template<typename StringType>
	static CoreModifiable*	Import(std::shared_ptr<XMLTemplate<StringType> > xmlfile, const std::string &filename, bool noInit = false, bool keepImportFileName = false, ImportState* state = nullptr, const std::string& override_name = "");

	static CoreModifiable* Find(const std::list<CoreModifiable*> &List, const std::string &Name);
	static CoreModifiable* FindByType(const std::list<CoreModifiable*> &List, const std::string& type);
	
	static  CoreModifiable* GetInstanceByGlobalPath(const std::string &path);

	// Search instance with infos (either type:name or a path)
	static CoreModifiable* SearchInstance(const std::string &infos, CoreModifiable* searchStart = 0);

	void getRootParentsWithPath(std::string &remainingpath, std::vector<CoreModifiable*>& parents);

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


protected:
	/// Internals
	void UpdateAggregates(const Timer&  timer, void* addParam);
	const ModifiableMethodStruct* findMethod(const KigsID& id, const CoreModifiable*& localthis) const;
	const ModifiableMethodStruct* recursivefindMethod(const KigsID& id, const CoreModifiable*& localthis) const;
	CoreModifiableAttribute* findAttribute(const KigsID& id) const;
	CoreModifiableAttribute* recursivefindAttribute(const KigsID& id) const;
	
	CoreModifiable* recursiveGetRootParentByType(const KigsID& ParentClassID, int currentlevel,int &foundLevel) const;

	void recursiveGetRootParentsWithPath(const std::string& searchType, const std::string& searchName, std::vector<CoreModifiable*>& parents);

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

		std::vector<CoreModifiable*> loadedItems;
		std::vector<ToConnect> toConnect;

		bool UTF8Enc;
		bool noInit = false;
		bool keepImportFileName = false;
		std::string override_name;
		bool is_include_unique = false;
		XMLBase* current_xml_file = nullptr;
	};

	template<typename StringType>
	static CoreModifiable* Import(XMLNodeTemplate<StringType> * currentNode, CoreModifiable* currentModifiable, ImportState& importState);

	// separated import attributes / import sons, so be sure to import attribute first
	template<typename StringType>
	static void ImportAttributes(XMLNodeTemplate<StringType>* currentNode, CoreModifiable* currentModifiable, ImportState& importState, std::vector<XMLNodeBase *>& sons);
	template<typename StringType>
	static void ImportSons(const std::vector<XMLNodeBase *>& sons, CoreModifiable* currentModifiable, ImportState& importState);

	template<typename StringType>
	static void	InitAttribute(XMLNodeTemplate<StringType>* currentNode, CoreModifiable* currentModifiable, ImportState& importState);

	template<typename StringType>
	static void	InitLuaScript(XMLNodeTemplate<StringType>* currentNode, CoreModifiable* currentModifiable, ImportState& importState);

	template<typename StringType>
	static AttachedModifierBase* InitAttributeModifier(XMLNodeTemplate<StringType>* modifierNode, CoreModifiableAttribute* attr);

	template<typename StringType>
	static CoreModifiable*	InitReference(XMLNodeTemplate<StringType>* currentNode, std::vector<CoreModifiable*> &loadedItems, const std::string& name);
	
	static bool AttributeNeedEval(const std::string& attr)
	{
		if (attr.size() > 6)
		{
			if (attr.substr(0, 5) == "eval(")
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
	std::unordered_map<XMLBase*, XMLNodeBase*> mXMLNodes;
	std::vector<std::shared_ptr<XMLBase>> mXMLFiles;
	u32 myEditorFlag = 0;
	LazyContent* GetLazyContentNoCreate() const { return mLazyContent; }
#endif

private:
	friend class CoreModifiableAttribute;
	friend class IMEditor;

	static void	ReleaseLoadedItems(std::vector<CoreModifiable*> &loadedItems);

	// attribute map
	robin_hood::unordered_map<KigsID, CoreModifiableAttribute* , KigsIDHash> _attributes;
	// sons vector
	std::vector<ModifiableItemStruct>				_items;
	// parent vector
	std::vector<CoreModifiable*>					_users;

	
	mutable std::recursive_mutex mObjectMutex;
	mutable LazyContent* mLazyContent = nullptr;

	LazyContent* GetLazyContent() const
	{
		if (mLazyContent) return mLazyContent;
		std::lock_guard<std::recursive_mutex> lk{ GetMutex() };
		if (mLazyContent) return mLazyContent;
		mLazyContent = new LazyContent;
		return mLazyContent;
	}

#ifndef TEST_ALLOWCHANGES_FLAG
	std::atomic_bool mAllowChanges = { true };
	int _ModifiableFlag = 0;
#else
	// Flags
	int _ModifiableFlag =  AllowChanges;
#endif

	
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


class MethodConstructor
{
public:
	MethodConstructor(CoreModifiable* obj, KigsID id, RefCountedClass::ModifiableMethod method)
	{
		obj->InsertMethod(id, method);
	}
};


class MethodsConstructor
{
public:
	struct MethodCtorParams
	{
		MethodCtorParams(CoreModifiable* obj, KigsID id, RefCountedClass::ModifiableMethod method)
		{
			obj->InsertMethod(id, method);
		}
	};

	MethodsConstructor(std::initializer_list<MethodCtorParams> params)
	{

	}
};

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


// Mainly used for numeric types
template<CoreModifiable::ATTRIBUTE_TYPE type>
struct EnumToType {};

template<>
struct EnumToType<CoreModifiable::BOOL> { using type = bool; };

template<>
struct EnumToType<CoreModifiable::CHAR> { using type = s8; };
template<>
struct EnumToType<CoreModifiable::SHORT> { using type = s16; };
template<>
struct EnumToType<CoreModifiable::INT> { using type = s32; };
template<>
struct EnumToType<CoreModifiable::LONG> { using type = s64; };

template<>
struct EnumToType<CoreModifiable::UCHAR> { using type = u8; };
template<>
struct EnumToType<CoreModifiable::USHORT> { using type = u16; };
template<>
struct EnumToType<CoreModifiable::UINT> { using type = u32; };
template<>
struct EnumToType<CoreModifiable::ULONG> { using type = u64; };

template<>
struct EnumToType<CoreModifiable::FLOAT> { using type = float; };
template<>
struct EnumToType<CoreModifiable::DOUBLE> { using type = double; };

template<>
struct EnumToType<CoreModifiable::STRING> { using type = std::string; };
template<>
struct EnumToType<CoreModifiable::USSTRING> { using type = usString; };


template<typename type>
struct TypeToEnum {};


template<>
struct TypeToEnum<bool> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::BOOL> {};

template<>
struct TypeToEnum<s8> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::CHAR> {};
template<>
struct TypeToEnum<s16> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::SHORT> {};
template<>
struct TypeToEnum<s32> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::INT> {};
template<>
struct TypeToEnum<s64> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::LONG> {};

template<>
struct TypeToEnum<u8> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::UCHAR> {};
template<>
struct TypeToEnum<u16> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::USHORT> {};
template<>
struct TypeToEnum<u32> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::UINT> {};
template<>
struct TypeToEnum<u64> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::ULONG> {};

template<>
struct TypeToEnum<float> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::FLOAT> {};
template<>
struct TypeToEnum<double> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::DOUBLE> {};

template<>
struct TypeToEnum<std::string> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::STRING> {};
template<>
struct TypeToEnum<usString> : std::integral_constant<CoreModifiable::ATTRIBUTE_TYPE, CoreModifiable::USSTRING> {};



template<typename TypeOut, typename TypeIn>
TypeOut* KigsDynamicCast(TypeIn* obj)
{
	if (obj->isSubType(TypeOut::myClassID))
		return static_cast<TypeOut*>(obj);
	return nullptr;
}



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

#endif
