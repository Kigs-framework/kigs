#pragma once

#ifdef STRING
#undef STRING
#endif

#include "CoreModifiable.h"
#include "CoreItem.h"
#include "usString.h"
#include <any>

namespace Kigs
{
	namespace Core
	{

		// ****************************************
		// * AttributeNotificationLevel enum
		// * --------------------------------------
		/*!
		* \enum	AttributeNotificationLevel
		* CoreModifiableAttribute can notify their owner that they have changed
		* Do do this, some dynamic type change is done with a placement new, changing the virtual
		* table of the objects. But placement new can in some cases reinit members (ie :for strings).
		* So placement new constructor must have some parameters as copy.
		* The notification occurs only when parameter is changed with a setValue (or array equivalent) methods
		*
		* \ingroup Core
		*/
		// ****************************************
		enum	AttributeNotificationLevel
		{
			None = 1,
			Owner = 2,
		};

		// Tag type for inheritance switch constructor
		struct InheritanceSwitch {};;

#define INHERIT_LEVEL_SHIFT 3
#define INHERIT_LEVEL_MOD 3


#define STATIC_ASSERT_NOTIF_LEVEL_SIZES(classname) static_assert(sizeof(classname<0>) == sizeof(classname<1>), "Size mismatch between notification levels");\
static_assert(sizeof(classname<0>) == sizeof(classname<2>), "Size mismatch between notification levels");\
static_assert(sizeof(classname<0>) == sizeof(classname<3>), "Size mismatch between notification levels");\
static_assert(sizeof(classname<2>) == sizeof(classname<3>), "Size mismatch between notification levels");


#define DECLARE_ATTRIBUTE_USINGS(underlying_type)\
protected:\
	using CoreModifiableAttributeData<underlying_type>::mValue;\
public:\
	using CoreModifiableAttributeData<underlying_type>::isReadOnly;

#define DECLARE_ATTRIBUTE_HERITAGE_IMPL(name, template_name, underlying_type, enum_type)\
public:\
using CoreModifiableAttributeData<underlying_type>::CoreModifiableAttributeData;\
typedef name CurrentAttributeClass;\
typedef underlying_type CurrentAttributeType;\
static constexpr CoreModifiable::ATTRIBUTE_TYPE type = enum_type;\
CoreModifiable::ATTRIBUTE_TYPE getType() const override { return enum_type; }\
DECLARE_ATTRIBUTE_USINGS(underlying_type)\
auto& operator=(const template_name<notificationLevel>& attribute)\
{\
	this->CopyData(attribute);\
return *this; \
}\
protected:\
void doPlacementNew(u32 level) override\
{\
	switch (level)\
	{\
		case 0: new (this) template_name<0>(InheritanceSwitch{}); break; \
		case 1: new (this) template_name<1>(InheritanceSwitch{}); break; \
		case 2: new (this) template_name<2>(InheritanceSwitch{}); break; \
		case 3: new (this) template_name<3>(InheritanceSwitch{}); break; \
		default: assert(false); break; \
	}\
}

		// Use this one if the underlying_type is not something used anywhere (like an internal structure)
#define DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(name, template_name, underlying_type, enum_type) DECLARE_ATTRIBUTE_HERITAGE_IMPL(name, template_name, underlying_type, enum_type)

// cast operator does not call modifiers
#define DECLARE_ATTRIBUTE_HERITAGE(name, template_name, underlying_type, enum_type) DECLARE_ATTRIBUTE_HERITAGE_IMPL(name, template_name, underlying_type, enum_type)\
public:\
virtual operator CurrentAttributeType() const\
{\
	return mValue;\
};\
auto& operator=(const CurrentAttributeType& value)\
{\
	mValue = value;\
	return *this;\
}






/*! \defgroup CoreModifiableAttibute CoreModifiableAttibute
*	\ingroup Core
*  \brief group of coreModifiableAttribute
*
* A CoreModifiableAttribute defines a type as an object.<br>
* the management of attribute is transparent :<br>
* This allow to export and import a CoreModifiable with its attributes.<br>
* Each CoreModifiableAttribute can be used and modified by anyone using generic function as getValue() and setValue().<br>
* it IS possible to add and remove a dynamic attribute to a CoreModifiable.<br>
* a CoreModifiableAttribute is accessed by an unique id. See the class documentation for the list of available CoreModifiableAttribute.<br>
*
*/


// ****************************************
// * CoreModifiableAttribute class
// * --------------------------------------
/**
* \file	CoreModifiableAttribute.h
* \class	CoreModifiableAttribute
* \ingroup Core
* \brief	 Base class for all CoreModifiableAttribute . This class is just composed of virtual methods
*/
// ****************************************
		class CoreModifiableAttribute
		{
			CoreModifiableAttribute(const CoreModifiableAttribute& attribute) = delete;

		protected:
			explicit CoreModifiableAttribute(InheritanceSwitch tag) {}


			CoreModifiableAttribute(CoreModifiable* owner, bool isInitParam, KigsID ID) : mOwner(owner), mID(ID), mFlags(0)
			{
				setIsInitParam(isInitParam);
				if (owner)
					owner->mAttributes[ID] = this;
			}

		public:


			virtual ~CoreModifiableAttribute();



			CoreModifiableAttribute& operator=(const CoreModifiableAttribute& other)
			{
				CopyAttribute(other);
				return *this;
			}




			//! retreive attribute label and coremodifiable path in a full path. Format is corepath->attributelabel
			static void ParseAttributePath(const std::string& path, std::string& CoreModifiablePath, std::string& CoreModifiableAttributeLabel);
			static std::string typeToString(CoreModifiable::ATTRIBUTE_TYPE typ);
			static CoreModifiable::ATTRIBUTE_TYPE stringToType(const std::string_view& typ);


			virtual CoreModifiable::ATTRIBUTE_TYPE getType() const = 0;
			virtual CoreModifiable::ATTRIBUTE_TYPE getArrayElementType() const { return CoreModifiable::ATTRIBUTE_TYPE::UNKNOWN; }

			virtual u32 getNbArrayElements() const { return 0; }
			virtual u32 getNbArrayColumns() const { return 0; }
			virtual u32 getNbArrayLines() const { return 0; }


			virtual std::vector<std::string> getEnumElements() const { return {}; }

			KigsID getID() const { return mID; };

			// For Backward compatibility:
			KigsID getLabel() const { return mID; };
			KigsID getLabelID() const { return mID; };


			enum ModifiableAttributeFlags
			{
				isReadOnlyFlag = 1,
				isInitFlag = 2,
				isDynamicFlag = 4,
				notifyOwnerFlag = 8,
				haveAttachedModifier = 16
			};


			virtual void changeNotificationLevel(AttributeNotificationLevel level);


#define DECLARE_SET(type)	virtual bool setValue(type value,CoreModifiable* owner){ return false; }
			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SET);
			DECLARE_SET(const char*);
			DECLARE_SET(const std::string&);
			DECLARE_SET(const unsigned short*);
			DECLARE_SET(const usString&);
			DECLARE_SET(CoreItemSP);
			DECLARE_SET(CMSP);
			DECLARE_SET(void*);
			DECLARE_SET(const UTF8Char*); // Only for usstring
			DECLARE_SET(const Point2D&);
			DECLARE_SET(const Point3D&);
			DECLARE_SET(const Vector4D&);

#define DECLARE_GET(type) virtual bool getValue(type value,const CoreModifiable* owner) const { return false; }
			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, DECLARE_GET);
			DECLARE_GET(std::string&);
			DECLARE_GET(usString&);
			DECLARE_GET(CoreItemSP&);
			DECLARE_GET(CMSP&);
			DECLARE_GET(void*&);
			DECLARE_GET(Point2D&);
			DECLARE_GET(Point3D&);
			DECLARE_GET(Vector4D&);

#define DECLARE_SETARRAYVALUE(type)	virtual bool setArrayValue(type /*value*/,CoreModifiable* owner, u32 /* nbElements */){return false;};
			EXPAND_MACRO_FOR_BASE_TYPES(const, *, DECLARE_SETARRAYVALUE);

#define DECLARE_GETARRAYVALUE(type) virtual bool getArrayValue(type * const /* value */ ,const CoreModifiable* owner, u32 /* nbElements */) const {return false;};
			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_GETARRAYVALUE);

#define DECLARE_SETARRAYELEMENTVALUE(type)	virtual bool setArrayElementValue(type /*value*/,CoreModifiable* owner, u32 /* line */, u32 /* column */){ return false; };
			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SETARRAYELEMENTVALUE);
			DECLARE_SETARRAYELEMENTVALUE(const std::string&);

#define DECLARE_GETARRAYELEMENTVALUE(type) virtual bool getArrayElementValue(type /* value */,const CoreModifiable* owner, u32 /* line */, u32 /* column */) const {return false;};
			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, DECLARE_GETARRAYELEMENTVALUE);
			DECLARE_GETARRAYELEMENTVALUE(std::string&);


#undef DECLARE_SET
#undef DECLARE_GET
#undef DECLARE_SETARRAYVALUE
#undef DECLARE_GETARRAYVALUE
#undef DECLARE_SETARRAYELEMENTVALUE
#undef DECLARE_GETARRAYELEMENTVALUE

			CoreModifiable* getOwner() const;

			//! Read only attributes cannot be modified with setValue
			virtual bool isReadOnly() { return (bool)((((u8)isReadOnlyFlag) & this->mFlags) != 0); }
			//! \brief  return true if attribute is an init attribute (necessary for the CoreModifiable Init to be done)
			virtual bool isInitParam() { return (bool)((((u8)isInitFlag) & this->mFlags) != 0); }
			virtual bool isDynamic() { return (bool)((((u8)isDynamicFlag) & this->mFlags) != 0); }
			virtual void setReadOnly(bool val)
			{
				if (val)
				{
					this->mFlags |= (u8)isReadOnlyFlag;
				}
				else
				{
					this->mFlags &= ~(u8)isReadOnlyFlag;
				}
			}
			virtual void setDynamic(bool dyn)
			{
				if (dyn)
				{
					this->mFlags |= (u8)isDynamicFlag;
				}
				else
				{
					this->mFlags &= ~(u8)isDynamicFlag;
				}
			}
			virtual void setIsInitParam(bool init)
			{
				if (init)
				{
					this->mFlags |= (u8)isInitFlag;
				}
				else
				{
					this->mFlags &= ~(u8)isInitFlag;
				}
			}


			virtual void* getRawValue() = 0;

			// return element number for array or one for other attributes
			virtual int		size() const { return 1; };
			virtual size_t 	MemorySize() const { return 0; };

			virtual void	changeInheritance() = 0;
			virtual void	doPlacementNew(u32 level) = 0;

			// Return true if the copy was done correctly
			virtual bool	CopyAttribute(const CoreModifiableAttribute& other) = 0;

		protected:
			CoreModifiable* mOwner;
			KigsID					mID;
			u8						mFlags;
		};

		// ****************************************
		// * CoreModifiableAttributeData class
		// * --------------------------------------
		/**
		* \class	CoreModifiableAttributeData
		* \file		CoreModifiableAttribute.h
		* \ingroup CoreModifiableAttibute
		* \brief	Base template class for CoreModifiable attributes managing data
		*/
		// ****************************************

		template<typename T>
		class CoreModifiableAttributeData : public CoreModifiableAttribute
		{

		public:
			CoreModifiableAttributeData(CoreModifiable& owner, bool isInitParam, KigsID ID, const T& value) : CoreModifiableAttribute(&owner, isInitParam, ID), mValue(value)
			{

			}
			CoreModifiableAttributeData(CoreModifiable& owner, bool isInitParam, KigsID ID) : CoreModifiableAttribute(&owner, isInitParam, ID), mValue{}
			{

			}
			CoreModifiableAttributeData(KigsID ID, const T& value) : CoreModifiableAttribute(nullptr, false, ID), mValue{ value }
			{
			}

			explicit CoreModifiableAttributeData(InheritanceSwitch tag) : CoreModifiableAttribute(tag) {}


			virtual void changeNotificationLevel(AttributeNotificationLevel level) final
			{
				CoreModifiableAttribute::changeNotificationLevel(level);
				changeInheritance();
			}

			inline T& ref() { return mValue; }
			inline const T& const_ref() const { return mValue; }

			void* getRawValue() override { return static_cast<void*>(&mValue); }

			friend class CoreModifiable;


			size_t MemorySize() const override { return sizeof(T); };

			CoreModifiableAttributeData<T>& operator=(const CoreModifiableAttributeData<T>& other)
			{
				CopyData(other);
				return *this;
			}

		protected:

			//! Underlying value
			T mValue;

			virtual void changeInheritance() final
			{
				KigsID old_id = mID;
				mID.~KigsID();
				T old_value = mValue;
				mValue.~T();
				CoreModifiable* owner = mOwner;
				u32 old_flags = mFlags;
				u32 inheritlevel = (mFlags >> INHERIT_LEVEL_SHIFT) & INHERIT_LEVEL_MOD;
				doPlacementNew(inheritlevel);
				mID = old_id;
				mValue = old_value;
				mFlags = old_flags;
				mOwner = owner;
			}


			// Uses RTTI dynamic cast to check for compatibility by default
			// If we ever add attributes that are not CoreModifiableAttributeData this would need to be changed/overloaded for each type
			bool CopyAttribute(const CoreModifiableAttribute& other) override
			{
#if 0
				const CoreModifiableAttributeData<T>* casted = dynamic_cast<const CoreModifiableAttributeData<T>*>(&other);
				if (casted)
				{
					CopyData(*casted);
					return true;
				}
				return false;
#else
				if (getType() == other.getType() &&
					getArrayElementType() == other.getArrayElementType() &&
					getNbArrayColumns() == other.getNbArrayColumns() &&
					getNbArrayLines() == getNbArrayLines())
				{
					CopyData(static_cast<const CoreModifiableAttributeData<T>&>(other));
					return true;
				}
				return false;
#endif
			}

			virtual void CopyData(const CoreModifiableAttributeData<T>& other)
			{
				mValue = other.mValue;
			}
		};


#define DO_NOTIFICATION(level)	if(level&1){this->getOwner()->NotifyUpdate(CoreModifiableAttribute::getLabel().toUInt());}


		// ****************************************
		// * maRawPtrHeritage class
		// * --------------------------------------
		/**
		* \class	maRawPtrHeritage
		* \file		CoreModifiableAttribute.h
		* \ingroup CoreModifiableAttibute
		* \brief	Base class for attributes managing a single ptr ( void* )
		*/
		// ****************************************

		template<int notificationLevel>
		class maRawPtrHeritage : public CoreModifiableAttributeData<void*>
		{
			DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maRawPtrHeritage, maRawPtrHeritage, void*, CoreModifiable::ATTRIBUTE_TYPE::RAWPTR);
		public:

			bool getValue(CMSP& value, const CoreModifiable* owner) const override
			{
				value = CMSP(static_cast<CoreModifiable*>(mValue));
				return true;
			}
			bool getValue(void*& value, const CoreModifiable* owner) const override { value = mValue; return true; }
			bool setValue(void* value, CoreModifiable* owner) override
			{
				if (isReadOnly()) return false;
				mValue = value;
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
			bool setValue(CMSP value, CoreModifiable* owner) override
			{
				if (isReadOnly()) return false;
				mValue = static_cast<void*>(value.get());
				DO_NOTIFICATION(notificationLevel);
				return true;
			}

		};
		using maRawPtr = maRawPtrHeritage<0>;


		// Template specializations defined in CoreModifiable.cpp
		//! \brief methods to convert a string to a value (numeric, boolean...)
		template<typename T> bool CoreConvertString2Value(const std::string& stringValue, T& value);
		//! \brief methods to convert a value (numeric, boolean...) to a string
		template<typename T> bool CoreConvertValue2String(std::string& stringValue, T value);

	}
}

		//@REFACTOR: Ideally those includes would be removed and each .cpp only includes the ma****.h that it needs
#include "maString.h"
#include "maNumeric.h"
#include "maBool.h"
#include "maEnum.h"
#include "maArray.h"
#include "maAny.h"

namespace Kigs
{
	namespace Core
	{
// AttributeHolder methods

		template<typename T>
		inline CMSP::AttributeHolder::operator T() const {
			T	tmp{};
			if (mAttr)
			{
				mAttr->getValue(tmp);
			}
			return tmp;
		}

		template<typename T>
		inline const CMSP::AttributeHolder& CMSP::AttributeHolder::operator =(T toset) const {
			if (mAttr)
			{
				mAttr->setValue(toset);
			}
			return *this;
		}

		template<typename T>
		inline const bool CMSP::AttributeHolder::operator ==(T totest) const {
			if (mAttr)
			{
				T	tmp{};
				if (mAttr->getValue(tmp))
				{
					return tmp == totest;
				}
			}
			return false;
		}


	}
}
