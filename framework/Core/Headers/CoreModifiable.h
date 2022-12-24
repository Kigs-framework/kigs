#pragma once

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
#include <list>
#include <any>
#include <shared_mutex>


#include "XML.h"
#include "XMLNode.h"


namespace Kigs
{
	namespace Time
	{
		class Timer;
	}
	namespace File
	{
		class CorePackage;
	}
	namespace Core
	{
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
		class CoreTreeNode;
		class CoreItem;
		class CoreItemSP;
		class KigsCore;

		// utility class to pack variable argument list in std::vector<CoreModifiableAttribute*>

		#if defined(KIGS_TOOLS) || defined(_DEBUG)
		#ifndef KEEP_XML_DOCUMENT
		#define KEEP_XML_DOCUMENT
		#endif
		#endif

		#define DECLARE_DEFAULT_LINK_NAME
		#define DECLARE_LINK_NAME
		#define PASS_LINK_NAME(a)


		struct ExportSettings
		{
			Kigs::File::CorePackage* current_package = nullptr;
			bool export_directly_in_package = true;
			std::set<std::string> external_files_exported;
			std::string current_xml_file;
			std::vector<std::string> current_unique_id_path;
			std::string working_directory;
			size_t export_buffer_attribute_as_external_file_size_threshold = 0;

		#ifdef KEEP_XML_DOCUMENT
			Xml::XML* current_xml_file_node = nullptr;
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

		struct LazyContent;
		struct ModifiableMethodStruct;
		class UpgradorBase;
		class StructLinkedListBase;

		struct LazyContentLinkedListItemStruct
		{
		protected:

			// hack use first two unused bits of address (aligned on 4 bytes at least)
			// to give a type
			enum class ItemType
			{
				UpgradorType = 0,
				ForwardSmartPtrType = 1,
				// available for future usage = 2,
				// available for future usage = 3
			};

			LazyContentLinkedListItemStruct(uintptr_t init) :mAddressAndType(init)
			{

			}

			static LazyContentLinkedListItemStruct	FromAddressAndType(StructLinkedListBase* realaddress, const LazyContentLinkedListItemStruct::ItemType settype)
			{
				LazyContentLinkedListItemStruct	result((uintptr_t)realaddress);
				result.mAddressAndType |= (uintptr_t)settype;
				return result;
			}


			friend class CoreModifiable;
			friend class StructLinkedListBase;
			friend struct LazyContent;
			uintptr_t	mAddressAndType;

		public:
			operator StructLinkedListBase* () const
			{
				StructLinkedListBase* realaddress = (StructLinkedListBase*)(mAddressAndType & (((uintptr_t)-1) ^ (uintptr_t)3));
				return realaddress;
			}
			operator uintptr_t () const
			{
				return mAddressAndType;
			}

			LazyContentLinkedListItemStruct::ItemType	getType() const
			{
				return (LazyContentLinkedListItemStruct::ItemType)(mAddressAndType & 3);
			}
		};

		class StructLinkedListBase
		{
		protected:

			LazyContentLinkedListItemStruct mNextItem = 0; // store address + type 
		public:
			StructLinkedListBase* getNext(const LazyContentLinkedListItemStruct::ItemType searchtype) const
			{
				LazyContentLinkedListItemStruct next = mNextItem;
				while ((uintptr_t)next)
				{
					StructLinkedListBase* realaddress = (StructLinkedListBase*)next;
					if ((next & 3) == (u32)searchtype)
					{
						return realaddress;
					}
					next = realaddress->mNextItem;
				}
				return nullptr;
			}

			LazyContentLinkedListItemStruct getNext() const
			{
				return mNextItem;
			}

		};


		class CMSP : public SmartPointer<CoreModifiable>
		{
		public:
			using SmartPointer<CoreModifiable>::SmartPointer;

			// Auto casts
			template<typename U>
			CMSP(const std::shared_ptr<U>& other) : SmartPointer<CoreModifiable>(debug_checked_pointer_cast<CoreModifiable>(other))
			{
			}
			template<typename U>
			CMSP(std::shared_ptr<U>&& other) : SmartPointer<CoreModifiable>(debug_checked_pointer_cast<CoreModifiable>(other))
			{
			}
			/*template<typename U>
			CMSP& operator=(const std::shared_ptr<U>& other)
			{
				std::shared_ptr<CoreModifiable>::operator=(debug_checked_pointer_cast<CoreModifiable>(other));
				return *this;
			}
			template<typename U>
			CMSP& operator=(std::shared_ptr<U>&& other)
			{
				std::shared_ptr<CoreModifiable>::operator=(debug_checked_pointer_cast<CoreModifiable>(other));
				return *this;
			}*/
			/*template<typename U>
			operator SmartPointer<U>()
			{
				return debug_checked_pointer_cast<U>(*this);
			}*/
			template<typename U>
			bool operator==(const U* other) const
			{
				return this->get() == other;
			}
			template<typename U>
			bool operator!=(const U* other) const
			{
				return this->get() != other;
			}

			inline CMSP operator [](const std::string& son) const;

			template<size_t _Size>
			inline CMSP operator [](const char(&son)[_Size]) const;

			// CoreModifiable Attributes management
			class AttributeHolder
			{

			protected:
				CoreModifiableAttribute* mAttr = nullptr;
			public:

				AttributeHolder() : mAttr(nullptr) {};
				AttributeHolder(CoreModifiableAttribute* attr) : mAttr(attr) {};
				AttributeHolder(const AttributeHolder& other) : mAttr(other.mAttr) {};

				template<typename T>
				inline operator T() const;

				template<typename T>
				inline const AttributeHolder& operator =(T toset) const;

				template<typename T>
				inline const bool operator ==(T totest) const;
			};


			inline AttributeHolder operator()(const std::string& son) const;
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
			ModifiableItemStruct(const CMSP& item) : mItem(item), mItemFlag(0) {}

			operator CMSP() { return mItem; }
			CoreModifiable* operator->() const { return (CoreModifiable*)mItem.get(); }
			CoreModifiable& operator*() const { return *((CoreModifiable*)mItem.get()); }

			CMSP mItem;

			inline bool	isAggregate() const
			{
				return mItemFlag&(u32)AgreggateFlag;
			}
			inline void	setAggregate()
			{
				mItemFlag |= (u32)AgreggateFlag;
			}
			inline void	unsetAggregate()
			{
				mItemFlag &= 0xFFFFFFFF ^ (u32)AgreggateFlag;
			}

		private:
			// flag used to store item or link type (item, aggregate...)
			u32	mItemFlag;
		};

		typedef	const int	CoreMessageType;


		class CoreClassNameTree
		{
		public:
			struct	TwoNames
			{
				KigsID mClassName;
				KigsID mRuntimeName;
			};

			// constructors
			CoreClassNameTree(KigsID className, KigsID runtimeName) : mClassNames{} { addClassName(className, runtimeName); }
			CoreClassNameTree() : mClassNames() {}

			CoreClassNameTree(const CoreClassNameTree& tree) = delete;
			CoreClassNameTree& operator=(const CoreClassNameTree& tree) = delete;

			// push class name
			void addClassName(KigsID className, KigsID runtimeName)
			{
				for (const auto& names : mClassNames)
				{
					if (names.mClassName == className)
						return;
				}
				mClassNames.push_back({ className, runtimeName });
			}

			// return classname vector
			const std::vector<TwoNames>& classNames() const { return mClassNames; }

		protected:
			std::vector<TwoNames> mClassNames;
		};



		// generic flag
		#define InitFlag							(1U)
		//#define PostDestroyFlag					(2U) // Free slot, feel free to use for something else
		#define RecursiveUpdateFlag					(4U)
		#define NotificationCenterRegistered		(8U)
		//#define ReferenceRegistered				(16U) // Free slot, feel free to use for something else
		#define AutoUpdateRegistered				(32U)
		#define AggregateParentRegistered			(64U)
		#define AggregateSonRegistered				(128U)
		//#define AllowChanges						(256U) // Free slot, feel free to use for something else
		#define AutoCreateAttributes				(512U) // when loading from xml, add not found CoreModifiableAttributes even when not tagget as dynamic 

		// more generic UserFlags
		constexpr u32 UserFlagsBitSize = 16;
		constexpr u32 UserFlagsShift = (32 - UserFlagsBitSize);
		// user flag is set at 0xFFFF0000
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



		#pragma pack(4)
		struct ModifiableMethodStruct
		{
			using func_type = std::function<bool(CoreModifiable*, CoreModifiable*, std::vector<CoreModifiableAttribute*>&, void*) >;
			struct Method
			{
				GenericRefCountedBaseClass::ModifiableMethod mMethod;
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

			ModifiableMethodStruct(const std::string& name, GenericRefCountedBaseClass::ModifiableMethod method, UpgradorBase* up = nullptr)
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


		// ****************************************
		// * CoreModifiable class
		// * --------------------------------------
		/**
		 * \file	CoreModifiable.h
		 * \class	CoreModifiable
		 * \ingroup Core
		 * \brief	Base class for Kigs framework objects.
		 * CoreModifiable class manage a list of attributes supporting reflexion and serialization. 
		 * CoreModifiable also support reference counting, auto update, instance factory... 
		 */
		 // ****************************************
		class CoreModifiable : public GenericRefCountedBaseClass
		{
			static_assert(sizeof(CMSP) == sizeof(SmartPointer<CoreModifiable>));
			static_assert(sizeof(std::shared_ptr<CoreModifiable>) == sizeof(SmartPointer<CoreModifiable>));
		public:
			//DECLARE_ABSTRACT_CLASS_INFO(CoreModifiable, RefCountedClass, KigsCore);
			static const KigsID mClassID;
			static KigsID mRuntimeType;

			static constexpr unsigned int UserFlagNode3D = 1 << 0;
			static constexpr unsigned int UserFlagDrawable = 1 << 1;
			static constexpr unsigned int usedUserFlags = 2;
	
			typedef CoreModifiable CurrentClassType;
			typedef GenericRefCountedBaseClass ParentClassType;
			virtual bool Call(CoreModifiable::ModifiableMethod method, CoreModifiable* sender, std::vector<CoreModifiableAttribute*>& attr, void* privateParams)
			{
				CoreModifiable::ModifiableMethod currentmethod = static_cast<CoreModifiable::ModifiableMethod>(method);
				return (this->*(currentmethod))(sender, attr, privateParams);
			}
			static CMSP Get()
			{
				return GetFirstInstance("CoreModifiable", false);
			}
			static CMSP Get(const std::string& name)
			{
				return GetFirstInstanceByName("CoreModifiable", name, false);
			}

			DECLARE_GetRuntimeTypeBase(CoreModifiable);
			DECLARE_getExactTypeBase(CoreModifiable);

			virtual void ConstructClassNameTree(CoreClassNameTree& classNameTree) { classNameTree.addClassName("CoreModifiable", "CoreModifiable"); }
	
			CMSP SharedFromThis() 
			{
		#ifdef _DEBUG
				try
				{
					return debug_checked_pointer_cast<CoreModifiable>(shared_from_this());
				}
				catch (const std::bad_weak_ptr&)
				{
					// NOTE(antoine)
					// It's forbidden to call shared_from_this inside the constructor or the destructor 
					// Connect and Upgrade are common functions that will do so, move them out of the constructor (InitModidiable most likely)
					// Another possibility is that this object was new'ed manually, DON'T!
		#ifdef WIN32			
					__debugbreak();
		#endif	
				}
				return nullptr;
		#else
				return debug_checked_pointer_cast<CoreModifiable>(shared_from_this());
		#endif
			}

			virtual KigsID getExactTypeID() const { return CoreModifiable::mClassID; }
			virtual bool isSubType(const KigsID& cid) const { return CoreModifiable::mClassID == cid; }
			static void GetClassNameTree(CoreClassNameTree& classNameTree);

			static void GetMethodTable(std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& method_table) {}
			static void GetNotWrappedMethodTable(std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& method_table) {}


			static std::vector<CMSP> GetInstances(const KigsID& id, bool exact_type_only = false);
			static std::vector<CMSP> GetInstancesByName(const KigsID& id, const std::string& name, bool exact_type_only = false);
			static CMSP GetFirstInstance(const KigsID& id, bool exact_type_only = false);
			static CMSP GetFirstInstanceByName(const KigsID& id, const std::string& name, bool exact_type_only = false);
			static std::vector<CMSP> GetRootInstances(const KigsID& cid, bool exactTypeOnly = false);

			const std::string& getName() const { return mName; }
			void setName(const std::string& name);
			const KigsID& getNameID() const { return mNameID; }
			inline unsigned int getUID() { return mUID; }
			CoreTreeNode* GetTypeNode() const { return mTypeNode; }

			// Don't call this manually!
			void RegisterToCore();
			virtual ~CoreModifiable();

		protected:
			explicit CoreModifiable(std::string name, DECLARE_CLASS_NAME_TREE_ARG) : GenericRefCountedBaseClass() 
			{
				mUID = mUIDCounter.fetch_add(1);
				if (name.empty())
					name = "nobody" + std::to_string(mUID);
				mName = name;
				mNameID = name;
			}
	
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
				WEAK_REFERENCE,
				STRONG_REFERENCE,
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
				//KIGS_ASSERT(isSubType(T::mClassID));
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
			void CallUpdate(const Time::Timer& timer, void* addParam);

			// Call Update recursively
			void RecursiveUpdate(const Time::Timer&  timer, CoreModifiable* a_parent = NULL);

			// Clear the item list
			void EmptyItemList();

			// Get the item list
			const std::vector<ModifiableItemStruct>& getItems() const { return mItems; }

			// Get the attribute list
			const unordered_map<KigsID, CoreModifiableAttribute*>& getAttributes() const { return mAttributes; }
	
			// Number of attributes
			size_t getAttributeCount() const { return mAttributes.size(); }
	
			// Get parents/users
			const std::vector<CoreModifiable*>& GetParents() const { return mUsers; }
	
			// Get parents/users count
			size_t GetParentCount() const { return mUsers.size(); }

			void RemoveFromAllParents();

			// Get the first parent matching the ID
			CoreModifiable* getFirstParent(KigsID ParentClassID) const;

			CoreModifiable* getRootParentByType(KigsID ParentClassID) const
			{
				int foundLevel = -1;
				return recursiveGetRootParentByType(ParentClassID, 0, foundLevel);
			}

			// Return true if the modifiable has a least one parent/user
			bool isUsed() const { return !mUsers.empty(); }

			// Search in sons for by id and name
			void GetSonInstancesByName(KigsID cid, const std::string &name, std::vector<CMSP>& instances,bool recursive = false);

			// Search in sons for by id and name, return the first one
			CMSP GetFirstSonByName(KigsID cid, const std::string &name, bool recursive = false);

			template<typename T>
			T* GetSon(const std::string& name, bool recursive = false)
			{
				return GetFirstSonByName(T::mClassID, name, recursive)->template as<T>();
			}

			template<typename T>
			T* GetSon(const char* name, bool recursive = false)
			{
				return GetFirstSonByName(T::mClassID, name, recursive)->template as<T>();
			}

			// Search in sons for by id
			void GetSonInstancesByType(KigsID cid, std::vector<CMSP>& instances, bool recursive = false);

			// Search in sons for by id, return the first one
			CMSP GetFirstSonByType(KigsID cid, bool recursive = false);

			template<typename T>
			T* GetSon(bool recursive = false)
			{
				return GetFirstSonByType(T::mClassID, recursive)->template as<T>();
			}

			// Search in sons for by path
			CMSP	GetInstanceByPath(const std::string& path);

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
			DECLARE_SET_VALUE(const UTF8Char*);
			DECLARE_SET_VALUE(const v2f&);
			DECLARE_SET_VALUE(const v3f&);
			DECLARE_SET_VALUE(const v4f&);

			#define DECLARE_GET_VALUE(type) bool getValue(const KigsID attributeLabel, type value) const;
			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, DECLARE_GET_VALUE);
			EXPAND_MACRO_FOR_STRING_TYPES(NOQUALIFIER, &, DECLARE_GET_VALUE);
			EXPAND_MACRO_FOR_EXTRA_TYPES(NOQUALIFIER, &, DECLARE_GET_VALUE);

			DECLARE_GET_VALUE(v2f&);
			DECLARE_GET_VALUE(v3f&);
			DECLARE_GET_VALUE(v4f&);

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
			T* getAny(const KigsID id);

			template<typename T>
			void setValueRecursively(KigsID id, T&& value)
			{
				setValue(id, value);
				for (auto& it : getItems()) 
					it.mItem->setValueRecursively(id, value);
			}

			template<typename T, typename Pred>
			void setValueRecursively(KigsID id, T&& value, Pred&& predicate)
			{
				if(predicate(this))
					setValue(id, value);
				for (auto& it : getItems())
					it.mItem->setValueRecursively(id, value, std::forward<decltype(predicate)>(predicate));
			}

			/// Dynamic attribute management
			template<typename attribute_type, typename value_type>
			attribute_type*	AddDynamicAttribute(KigsID ID, const value_type& value);
			CoreModifiableAttribute*	AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID, s32 defaultval);
			CoreModifiableAttribute*	AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID, u64 defaultval);
			CoreModifiableAttribute*	AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID, float defaultval);
			CoreModifiableAttribute*	AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID, bool defaultval);
			CoreModifiableAttribute*	AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID, const char* defaultval = 0);

			CoreModifiableAttribute*	AddDynamicVectorAttribute(KigsID ID, const int* defaultval, u32 valcount);
			CoreModifiableAttribute*	AddDynamicVectorAttribute(KigsID ID, const float* defaultval, u32 valcount);

			void RemoveDynamicAttribute(KigsID id);


			/// Method management
			// Return the map of private methods of this CoreModifiable
			const unordered_map <KigsID, ModifiableMethodStruct>* GetMethods();
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

			/// Aggregate management
			// Adds a new son as aggregate
			bool aggregateWith(const CMSP& item, ItemPosition pos = Last);
			// Removes a son as aggregate
			bool removeAggregateWith(const CMSP& item);
			// Recursive search all aggregate sons for an aggregate of the given type  
			CoreModifiable*	getAggregateByType(KigsID id);
			// Search parent to see if one of them is aggregate root of this, then recurse
			CoreModifiable*	getAggregateRoot() const;
			// Aggregate cast search in all aggregate if the current type can be found else returns nullptr 
			template<typename T>
			static T* aggregate_cast(CoreModifiable* tocast)
			{
				CoreModifiable*	startPoint = tocast->getAggregateRoot();
				CoreModifiable* found = startPoint->getAggregateByType(T::mClassID);
				if (found)
				{
					return (T*)found;
				}
				return nullptr;
			}
	
			/// Flags
			inline void flagAsNotificationCenterRegistered()
			{
				mModifiableFlag |= (u32)NotificationCenterRegistered;
			}
			inline void unflagAsNotificationCenterRegistered()
			{
				mModifiableFlag &= 0xFFFFFFFF ^ (u32)NotificationCenterRegistered;
			}
			inline bool isFlagAsNotificationCenterRegistered() const
			{
				return (mModifiableFlag&(u32)NotificationCenterRegistered) != 0;
			}
			inline void flagAsAutoUpdateRegistered()
			{
				mModifiableFlag |= (u32)AutoUpdateRegistered;
			}
			inline void unflagAsAutoUpdateRegistered()
			{
				mModifiableFlag &= 0xFFFFFFFF ^ (u32)AutoUpdateRegistered;
			}
			inline bool isFlagAsAutoUpdateRegistered() const
			{
				return (mModifiableFlag&(u32)AutoUpdateRegistered) != 0;
			}
			void flagAsPostDestroy();
			void unflagAsPostDestroy();
			inline void setUserFlag(u32 flag)
			{
				mModifiableFlag |= ((u32)flag) << UserFlagsShift;
			}
			inline void unsetUserFlag(u32 flag)
			{
				mModifiableFlag &= 0xFFFFFFFF ^ (((u32)flag) << UserFlagsShift);
			}

			inline void changeUserFlag(u32 flag, bool setit)
			{
				setit?setUserFlag(flag):unsetUserFlag(flag);
			}

			inline bool isUserFlagSet(u32 flag) const
			{
				return ((mModifiableFlag&(flag << UserFlagsShift)) != 0);
			}

			u32	getUserFlags(u32 mask) const
			{
				return ((mModifiableFlag & (mask << UserFlagsShift)) >> UserFlagsShift);
			}

			inline bool isInitFlagSet() const { return ((mModifiableFlag&((u32)InitFlag)) != 0); }

			inline void flagAutoCreateAttributes()
			{
				mModifiableFlag |= (u32)AutoCreateAttributes;
			}
			inline void unflagAutoCreateAttributes()
			{
				mModifiableFlag &= 0xFFFFFFFF ^ (u32)AutoCreateAttributes;
			}
			inline bool isFlagAutoCreateAttributes() const
			{
				return (mModifiableFlag & (u32)AutoCreateAttributes) != 0;
			}

			// for some type of classes when we want don't want duplicated instances (textures, shaders...)
			// return an already existing instance equivalent of this
			virtual CMSP getSharedInstance()
			{
				return SharedFromThis();
			}



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
			virtual void ProtectedDestroy() final;
	
			// Update method. Call to ParentClassType::Update is not necessary when overriding
			virtual void Update(const Time::Timer&  timer, void* addParam) {}

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

			// CDATA Export/Import API
			virtual int	HasCDataToExport() { return 0; }
			virtual bool GetCDataToExport(int /*index*/, std::string&/* toexport*/) { return false; }
			virtual void ImportFromCData(const std::string&/* imported*/) {}
	
			// Search an attribute list for a specific attribute
			static CoreModifiableAttribute*	getParameter(const std::vector<CoreModifiableAttribute*>& params, KigsID ID);
			static void Append(std::string &XMLString, const std::list<CoreModifiable*> &toexport, bool recursive, ExportSettings* settings = nullptr);
	
			static void	Export(const std::string &filename, CoreModifiable* toexport, bool recursive = false, ExportSettings* settings = nullptr);
			static Xml::XMLNode* ExportToXMLNode(CoreModifiable* toexport, Xml::XML* owner_xml_file, bool recursive, ExportSettings* settings = nullptr);
			static std::string ExportToXMLString(CoreModifiable* toexport, bool recursive, ExportSettings* settings=nullptr);
	
			static void	Export(std::string &XMLString, const std::list<CoreModifiable*> &toexport, bool recursive, ExportSettings* settings = nullptr);

		public:
			static CMSP	Import(const std::string &filename, bool noInit = false, bool keepImportFileName = false, ImportState* state = nullptr, const std::string& override_name="");
	
			template<typename StringType>
			static CMSP	Import(std::shared_ptr<Xml::XMLTemplate<StringType> > xmlfile, const std::string &filename, bool noInit = false, bool keepImportFileName = false, ImportState* state = nullptr, const std::string& override_name = "");

			static CoreModifiable* Find(const std::list<CoreModifiable*> &List, const std::string &Name);
			static CoreModifiable* FindByType(const std::list<CoreModifiable*> &List, const std::string& type);
	
			static CMSP GetInstanceByGlobalPath(const std::string& path);

			// Search instance with infos (either type:name or a path)
			static CMSP SearchInstance(const std::string& infos, CoreModifiable* searchStart = nullptr);

			std::vector<CMSP> getRootParentsWithPath(std::string &remainingpath);

			// Get the first name in a path (IWantThis/The/Remaining/Path). Return "/" if the path starts a the root
			static  std::string GetFirstNameInPath(const std::string &path, std::string & remainingpath);

			/// Editor

		#ifdef KEEP_NAME_AS_STRING
			void RegisterDecorator(const std::string& name);
			void UnRegisterDecorator(const std::string& name);
		#endif
		#ifdef KEEP_XML_DOCUMENT
			Xml::XMLNodeBase* GetXMLNodeForFile(Xml::XMLBase* for_file)
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
			// upgrador management
			void Upgrade(UpgradorBase* toAdd, bool doInit = true,bool attachMethods=true);
			void Downgrade(const KigsID& toRemove,bool doDestroy=true, bool detachMethods = true);
			// insert forward ptr
			LazyContentLinkedListItemStruct InsertForwardPtr(StructLinkedListBase* address);


		#ifdef KIGS_TOOLS
			bool mTraceRef = false;
		#endif
		protected:



			// used in import
			bool SimpleCallWithCoreItemParams(KigsID methodNameID, const CoreItemSP& params);



			void Connect(KigsID signal, CMSP other, KigsID slot CONNECT_PARAM_DEFAULT);
			void Disconnect(KigsID signal, const CMSP& other, KigsID slot);

			// check if we can destroy object
			// virtual bool checkDestroy() override;

			/// Internals
			void UpdateAggregates(const Time::Timer&  timer, void* addParam);
			const ModifiableMethodStruct* findMethodOnThisOnly(const KigsID& id, const CoreModifiable*& localthis) const;
			const ModifiableMethodStruct* findMethod(const KigsID& id, const CoreModifiable*& localthis) const;
			const ModifiableMethodStruct* recursivefindMethod(const KigsID& id, const CoreModifiable*& localthis) const;
			CoreModifiableAttribute* findAttributeOnThisOnly(const KigsID& id) const;
			CoreModifiableAttribute* findAttribute(const KigsID& id) const;
			CoreModifiableAttribute* recursivefindAttribute(const KigsID& id) const;
	
			CoreModifiable* recursiveGetRootParentByType(const KigsID& ParentClassID, int currentlevel,int &foundLevel) const;

			void recursiveGetRootParentsWithPath(const std::string& searchType, const std::string& searchName, std::vector<CMSP>& parents);

			void flagAsAggregateParent()
			{
				mModifiableFlag |= (u32)AggregateParentRegistered;
			}
			void unflagAsAggregateParent()
			{
				mModifiableFlag &= 0xFFFFFFFF ^ (u32)AggregateParentRegistered;
			}
			bool isFlagAsAggregateParent() const
			{
				return (mModifiableFlag&(u32)AggregateParentRegistered) != 0;
			}
			void flagAsAggregateSon()
			{
				mModifiableFlag |= (u32)AggregateSonRegistered;
			}
			void unflagAsAggregateSon()
			{
				mModifiableFlag &= 0xFFFFFFFF ^ (u32)AggregateSonRegistered;
			}
			bool isFlagAsAggregateSon() const
			{
				return (mModifiableFlag&(u32)AggregateSonRegistered) != 0;
			}
			bool isBelongingToAggregate() const
			{
				return ((mModifiableFlag&((u32)AggregateSonRegistered | (u32)AggregateParentRegistered)) != 0);
			}
			bool checkIfAggregateSon() const
			{

				std::vector<CoreModifiable*>::const_iterator itpc = mUsers.begin();
				std::vector<CoreModifiable*>::const_iterator itpe = mUsers.end();

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
							if ((*itc).isAggregate()) // if aggregate 
							{
								if ((*itc).mItem.get() == this) // and aggregate son is this
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
			void unregisterFromAutoUpdate();
			void DeleteDynamicAttributes();


			/// Export
			void	Export(std::vector<CoreModifiable*>& savedList, Xml::XMLNode * currentNode, bool recursive
				, ExportSettings* settings
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
					Xml::XMLNodeBase* xmlattr;
				};

				struct ToCall
				{
					CoreModifiable* currentNode;
					std::string methodName;
					std::string paramList;
				};

				std::vector<CMSP>		loadedItems;
				// manage items to connect after load is completed
				std::vector<ToConnect>	toConnect;
				// manage methods to call after load is completed
				std::vector<ToCall>		toCall;

				bool UTF8Enc;
				bool noInit = false;
				bool keepImportFileName = false;
				std::string override_name;
				bool is_include_unique = false;
				Xml::XMLBase* current_xml_file = nullptr;
			};

			static void ManageToCall(ImportState::ToCall& c);

			template<typename StringType>
			static CMSP Import(Xml::XMLNodeTemplate<StringType> * currentNode, CoreModifiable* currentModifiable, ImportState& importState);

			// separated import attributes / import sons, so be sure to import attribute first
			template<typename StringType>
			static void ImportAttributes(Xml::XMLNodeBase* currentNode, CoreModifiable* currentModifiable, ImportState& importState, std::vector<Xml::XMLNodeBase *>& sons);

			template<typename StringType>
			static void ImportUpgradors(Xml::XMLNodeTemplate<StringType>* currentNode, CoreModifiable* currentModifiable, ImportState& importState);

			template<typename StringType>
			static void ImportSons(const std::vector<Xml::XMLNodeBase *>& sons, CoreModifiable* currentModifiable, ImportState& importState);

			template<typename StringType>
			static void	InitAttribute(Xml::XMLNodeTemplate<StringType>* currentNode, CoreModifiable* currentModifiable, ImportState& importState);

			static void	InitLuaScript(Xml::XMLNodeBase* currentNode, CoreModifiable* currentModifiable, ImportState& importState);

			template<typename StringType>
			static CMSP	InitReference(Xml::XMLNodeTemplate<StringType>* currentNode, std::vector<CMSP> &loadedItems, const std::string& name);
	
	
			static void	EvalAttribute(std::string& attr, CoreModifiable* owner, CoreModifiableAttribute* destattr = 0);

			//! create and add dynamic attribute except arrays
			CoreModifiableAttribute*	GenericCreateDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE type, KigsID ID);

			UpgradorBase* GetUpgrador(const KigsID& ID="");



		#ifdef KEEP_XML_DOCUMENT
		public:
			unordered_map<Xml::XMLBase*, Xml::XMLNodeBase*> mXMLNodes;
			std::vector<std::shared_ptr<Xml::XMLBase>> mXMLFiles;
			u32 mEditorFlag = 0;
			LazyContent* GetLazyContentNoCreate() const { return mLazyContent; }
		#endif



		private:
			friend class CoreModifiableAttribute;
			friend class IMEditor;
			friend class KigsCore;

			auto& GetMutex() const { return mObjectMutex; }

			static void	ReleaseLoadedItems(std::vector<CMSP> &loadedItems);
			static std::atomic<unsigned int> mUIDCounter;

			unsigned int	mUID;
			KigsID			mNameID;
			// Flags
			unsigned int	mModifiableFlag = 0;
			mutable std::shared_mutex mObjectMutex;
	
			CoreTreeNode* mTypeNode = nullptr;
			mutable std::atomic<LazyContent*> mLazyContent{ nullptr };

			std::string	mName;
	
			// attribute map
			unordered_map<KigsID, CoreModifiableAttribute*> mAttributes;
			// sons vector
			std::vector<ModifiableItemStruct>				mItems;
			// parent vector
			std::vector<CoreModifiable*>					mUsers;


			LazyContent* GetLazyContent() const;



		#ifdef KEEP_NAME_AS_STRING
			// keep track of Decorators only on win32 to be able to export them
			std::vector<std::string>*	mDecorators = nullptr;
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

		//! Utility Macro to define methods for XML Parser Delegate
		#define Declare_XMLDelegate 	DECLARE_METHOD(XMLElementStartDescription); \
		DECLARE_METHOD(XMLElementEndDescription); \
		DECLARE_METHOD(XMLDeclHandler); \
		DECLARE_METHOD(XMLCharacterHandler);

		#define XMLDelegateMethods		JSonObjectStart,JSonObjectEnd,JSonArrayStart,JSonArrayEnd,JSonParamList


		class PackCoreModifiableAttributes
		{
		public:
			PackCoreModifiableAttributes(CoreModifiable* owner) :mOwner(owner)
			{
				mAttributeList.clear();
			}

			template<typename T>
			PackCoreModifiableAttributes& operator<<(T&& V);

			void AddAttribute(CoreModifiableAttribute* attr) { mAttributeList.push_back(attr); }

			operator std::vector<CoreModifiableAttribute*>&()
			{
				return mAttributeList;
			}

			~PackCoreModifiableAttributes();

		protected:
			std::vector<CoreModifiableAttribute*>	mAttributeList;
			CoreModifiable*							mOwner;
		};

		// CMSP methods
		inline CMSP CMSP::operator [](const std::string& son) const
		{
			if (get())
				return get()->GetFirstSonByName("CoreModifiable", son);
			return nullptr;
		}

		template<size_t _Size>
		inline CMSP CMSP::operator [](const char(&son)[_Size]) const
		{
			if (get())
				return get()->GetFirstSonByName("CoreModifiable", son);
			return nullptr;
		}


		inline CMSP::AttributeHolder CMSP::operator()(const std::string& attr) const
		{
			if (get())
			{
				CoreModifiableAttribute* attrib = get()->getAttribute(attr);
				return AttributeHolder(attrib);
			}
			return AttributeHolder(nullptr);
		}

		struct LazyContent
		{
			unordered_map<KigsID, std::vector<std::pair<KigsID, std::weak_ptr<CoreModifiable>>>> mConnectedTo;
			//unordered_map<CoreModifiable*, std::set<std::pair<KigsID, KigsID>>> mConnectedToMe;
			unordered_map <KigsID, ModifiableMethodStruct> mMethods;
			// Upgrador management
			LazyContentLinkedListItemStruct mLinkedListItem = 0;

			StructLinkedListBase* GetLinkedListItem(const LazyContentLinkedListItemStruct::ItemType	searchType)
			{
				LazyContentLinkedListItemStruct current = mLinkedListItem;
				while ((uintptr_t)current)
				{
					if (current.getType() == searchType)
					{
						return (StructLinkedListBase*)current;
					}
					current = ((StructLinkedListBase*)current)->getNext();
				}
				return nullptr;
			}

			~LazyContent();
		};

		// specialize some 
		template<>
		inline bool CoreModifiable::getValue(const KigsID id) const
		{
			bool val=false;
			getValue(id, val);
			return val;
		}

		template<>
		inline CoreModifiable* CoreModifiable::getValue(const KigsID id) const
		{
			CoreModifiable* val = nullptr;
			getValue(id, val);
			return val;
		}

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
			if (obj->isSubType(TypeOut::mClassID))
				return static_cast<TypeOut*>(obj);
			return nullptr;
		}

		typedef CMSP (*createMethod)(const std::string& instancename, std::vector<CoreModifiableAttribute*>* args);
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
			KigsID mMethodID;
			void* mPrivateParams =  nullptr;
			CoreModifiable* mMethodInstance = nullptr;
		};

		template<typename smartPointOn>
		class CoreAttributeAndMethodForwardSmartPointer : public SmartPointer<smartPointOn>, public StructLinkedListBase
		{
		protected:
			CoreAttributeAndMethodForwardSmartPointer() : SmartPointer<smartPointOn>()
			{

			}
		public:

			operator bool() const
			{
				return SmartPointer<smartPointOn>::get();
			}

			inline CoreAttributeAndMethodForwardSmartPointer(CoreModifiable* parent, CoreModifiable* p);

			CoreAttributeAndMethodForwardSmartPointer& operator=(const SmartPointer<smartPointOn>& smcopy)
			{
				SmartPointer<smartPointOn>::operator=(smcopy);
				return *this;
			}
			operator SmartPointer<smartPointOn>()
			{
				return *this;
			}
		};

		inline bool AttributeNeedEval(const std::string& attr)
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

		template<typename smartPointOn>
		using ForwardSP = CoreAttributeAndMethodForwardSmartPointer<smartPointOn>;

		#define INSERT_FORWARDSP(pointOn,name)	ForwardSP<pointOn> name={this,this};


		template<typename smartPointOn>
		inline CoreAttributeAndMethodForwardSmartPointer<smartPointOn>::CoreAttributeAndMethodForwardSmartPointer(CoreModifiable* parent,CoreModifiable* p) : SmartPointer<smartPointOn>(), StructLinkedListBase()
		{
			mNextItem = parent->InsertForwardPtr(this);
		}

	}
}
