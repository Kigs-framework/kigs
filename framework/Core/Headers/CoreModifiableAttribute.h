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

		// Tag type for inheritance switch constructor
		struct InheritanceSwitch {};


#define STATIC_ASSERT_NOTIF_LEVEL_SIZES(classname) static_assert(sizeof(classname<false>) == sizeof(classname<true>), "Size mismatch between notification levels");


#define DECLARE_ATTRIBUTE_USINGS(underlying_type)\
protected:\
	using CoreModifiableAttributeData<underlying_type,notificationLevel,isInitT,isReadOnlyT,isDynamicT,isOrphanT>::mValue;\
public:\
	using CoreModifiableAttributeData<underlying_type,notificationLevel,isInitT,isReadOnlyT,isDynamicT,isOrphanT>::isReadOnly;

#define DECLARE_ATTRIBUTE_HERITAGE_IMPL(name, template_name, underlying_type, enum_type)\
public:\
using CoreModifiableAttributeData<underlying_type,notificationLevel,isInitT,isReadOnlyT,isDynamicT,isOrphanT>::CoreModifiableAttributeData;\
typedef name CurrentAttributeClass;\
typedef underlying_type CurrentAttributeType;\
static constexpr CoreModifiable::ATTRIBUTE_TYPE type = enum_type;\
CoreModifiable::ATTRIBUTE_TYPE getType() const override { return enum_type; }\
DECLARE_ATTRIBUTE_USINGS(underlying_type)\
auto& operator=(const template_name<notificationLevel,isInitT,isReadOnlyT,isDynamicT,isOrphanT>& attribute)\
{\
	this->CopyData(attribute);\
	return *this; \
}\
protected:\
void doPlacementNew(u8 mask) override\
{\
	switch (mask)\
	{\
		case 0: new (this) template_name<false,false,false,false,false>(InheritanceSwitch{}); break; \
		case 1: new (this) template_name<true,false,false,false,false>(InheritanceSwitch{}); break; \
		case 2: new (this) template_name<false,true,false,false,false>(InheritanceSwitch{}); break; \
		case 3: new (this) template_name<true,true,false,false,false>(InheritanceSwitch{}); break; \
		case 4: new (this) template_name<false,false,true,false,false>(InheritanceSwitch{}); break; \
		case 5: new (this) template_name<true,false,true,false,false>(InheritanceSwitch{}); break; \
		case 6: new (this) template_name<false,true,true,false,false>(InheritanceSwitch{}); break; \
		case 7: new (this) template_name<true,true,true,false,false>(InheritanceSwitch{}); break; \
		case 8: new (this) template_name<false,false,false,true,false>(InheritanceSwitch{}); break; \
		case 9: new (this) template_name<true,false,false,true,false>(InheritanceSwitch{}); break; \
		case 12: new (this) template_name<false,false,true,true,false>(InheritanceSwitch{}); break; \
		case 13: new (this) template_name<true,false,true,true,false>(InheritanceSwitch{}); break; \
		case 16: new (this) template_name<false,false,false,false,true>(InheritanceSwitch{}); break; \
		case 20: new (this) template_name<false,false,true,false,true>(InheritanceSwitch{}); break; \
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
		CoreModifiableAttribute(CoreModifiable* owner, KigsID ID) : mID(ID._id)
			{

			}

		friend class CoreModifiable;

			template<bool notifOwnerTe, bool isInitParamTe, bool isReadOnlyTe, bool isDynamicTe, bool isOrphanTe>
			static u8 getMask()
			{
				u8 mask = (notifOwnerTe ? 1 : 0) | (isInitParamTe ? 2 : 0) | (isReadOnlyTe ? 4 : 0) | (isDynamicTe ? 8 : 0) | (isOrphanTe ? 16 : 0);
				return mask;
			}

		virtual void setDynamic(bool val, CoreModifiable* owner, const KigsID& ID) = 0;
		virtual void setNotifyOwner(bool val) = 0;

		u32		mID = 0; // only the u32 part of the KigsID
		u32		mPlaceHolderForSonClasses = 0;	// used to set flags/offsets... in inherited classes

		public:

		u32		id() const
		{
			return mID;
		}
			virtual std::vector<std::string> getEnumElements() const { return { }; }
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

		virtual size_t getNbArrayElements() const { return 0; }
		virtual size_t getNbArrayColumns() const { return 0; }
		virtual size_t getNbArrayLines() const { return 0; }


			//virtual void changeNotificationLevel(AttributeNotificationLevel level);


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

#define DECLARE_SETARRAYVALUE(type)	virtual bool setArrayValue(type /*value*/,CoreModifiable* owner, size_t /* nbElements */){return false;};
			EXPAND_MACRO_FOR_BASE_TYPES(const, *, DECLARE_SETARRAYVALUE);

#define DECLARE_GETARRAYVALUE(type) virtual bool getArrayValue(type * const /* value */ ,const CoreModifiable* owner, size_t /* nbElements */) const {return false;};
			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_GETARRAYVALUE);

#define DECLARE_SETARRAYELEMENTVALUE(type)	virtual bool setArrayElementValue(type /*value*/,CoreModifiable* owner, size_t /* line */, size_t /* column */){ return false; };
			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SETARRAYELEMENTVALUE);
			DECLARE_SETARRAYELEMENTVALUE(const std::string&);

#define DECLARE_GETARRAYELEMENTVALUE(type) virtual bool getArrayElementValue(type /* value */,const CoreModifiable* owner, size_t /* line */, size_t /* column */) const {return false;};
			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, DECLARE_GETARRAYELEMENTVALUE);
			DECLARE_GETARRAYELEMENTVALUE(std::string&);


#undef DECLARE_SET
#undef DECLARE_GET
#undef DECLARE_SETARRAYVALUE
#undef DECLARE_GETARRAYVALUE
#undef DECLARE_SETARRAYELEMENTVALUE
#undef DECLARE_GETARRAYELEMENTVALUE

			//! Read only attributes cannot be modified with setValue
		virtual bool isReadOnly() const = 0;
			//! \brief  return true if attribute is an init attribute (necessary for the CoreModifiable Init to be done)
		virtual bool isInitParam() const = 0;
		virtual bool isDynamic() const = 0;
		virtual bool isOrphan() const = 0;
		virtual void setReadOnly(bool val) = 0;
		virtual void setInitParam(bool val) = 0;

		virtual void* getRawValue(CoreModifiable* owner) = 0;

			// return element number for array or one for other attributes
			virtual int		size() const { return 1; };
			virtual size_t 	MemorySize() const { return 0; };

			virtual void	changeInheritance(u8 mask) = 0;
			virtual void	doPlacementNew(u8 mask) = 0;

			// Return true if the copy was done correctly
			virtual bool	CopyAttribute(const CoreModifiableAttribute& other) = 0;

		};

		// ****************************************
		// * CoreModifiableAttributeTemplated class
		// * --------------------------------------
		/**
		* \file	CoreModifiableAttribute.h
		* \class	CoreModifiableAttributeTemplated
		* \ingroup Core
		* \brief	 Base class with templated flags attributes
		*/
		// ****************************************
		template<bool notifOwnerT = false, bool isInitParamT = false, bool isReadOnlyT = false, bool isDynamicT = false, bool isOrphanT = false>
		class CoreModifiableAttributeTemplated : public CoreModifiableAttribute
		{

		protected:

			CoreModifiableAttributeTemplated(CoreModifiable* owner, KigsID ID) : CoreModifiableAttribute(owner, ID)
			{
				if (owner)
					owner->addAttribute(this, ID, isDynamicT);
			}

			u8 getCurrentMask()
			{
				u8 mask = (notifOwnerT ? 1 : 0) | (isInitParamT ? 2 : 0) | (isReadOnlyT ? 4 : 0) | (isDynamicT ? 8 : 0) | (isOrphanT ? 16 : 0);
				return mask;
			}

			friend class CoreModifiable;

			// is dynamic can be changed by CoreModifiable
			virtual void setDynamic(bool val, CoreModifiable* owner, const KigsID& ID) override
			{
				if (val != isDynamicT)
				{
					if (val)
					{
						if (owner)
							owner->setDynamicAttributeFlag(true, ID);

						changeInheritance(getMask<notifOwnerT, isInitParamT, isReadOnlyT, true, isOrphanT>());
					}
					else
					{

						if (owner)
							owner->setDynamicAttributeFlag(false, ID);


						changeInheritance(getMask<notifOwnerT, isInitParamT, isReadOnlyT, false, isOrphanT>());
					}
				}
			}

			virtual void setNotifyOwner(bool val) final
			{
				if (val != notifOwnerT)
				{
					if (val)
					{
						changeInheritance(CoreModifiableAttribute::getMask<true, isInitParamT, isReadOnlyT, isDynamicT, isOrphanT>());
					}
					else
					{
						changeInheritance(CoreModifiableAttribute::getMask<false, isInitParamT, isReadOnlyT, isDynamicT, isOrphanT>());
					}
				}
			}

		public:
			explicit CoreModifiableAttributeTemplated(InheritanceSwitch tag) : CoreModifiableAttribute(tag) {}


			//! Read only attributes cannot be modified with setValue
			virtual bool isReadOnly() const override { return isReadOnlyT; }
			//! \brief  return true if attribute is an init attribute (necessary for the CoreModifiable Init to be done)
			virtual bool isInitParam() const override { return isInitParamT; }
			virtual bool isDynamic() const override { return isDynamicT; }
			virtual bool isOrphan() const override { return isOrphanT; }

			// only readOnly & notif level can be changed
			virtual void setReadOnly(bool val) final
			{
				if (val != isReadOnlyT)
				{
					if (val)
					{
						changeInheritance(getMask<notifOwnerT, isInitParamT, true, isDynamicT, isOrphanT>());
					}
					else
					{
						changeInheritance(getMask<notifOwnerT, isInitParamT, false, isDynamicT, isOrphanT>());
					}
				}
			}

			// only readOnly & notif level can be changed
			virtual void setInitParam(bool val) final
			{
				if (val != isInitParamT)
				{
					if (val)
					{
						changeInheritance(getMask<notifOwnerT, true, isReadOnlyT, isDynamicT, isOrphanT>());
					}
					else
					{
						changeInheritance(getMask<notifOwnerT, false, isReadOnlyT, isDynamicT, isOrphanT>());
					}
				}
			}
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

	template<typename T, bool notifOwnerT = false, bool isInitT = false, bool isReadOnlyT = false, bool isDynamicT = false, bool isOrphanT = false>
	class CoreModifiableAttributeData : public CoreModifiableAttributeTemplated< notifOwnerT, isInitT, isReadOnlyT, isDynamicT, isOrphanT>
		{
		using CoreModifiableAttribute::mID;
		using CoreModifiableAttribute::mPlaceHolderForSonClasses;

		public:
		CoreModifiableAttributeData(CoreModifiable& owner, KigsID ID, const T& value) : CoreModifiableAttributeTemplated<notifOwnerT, isInitT, isReadOnlyT, isDynamicT, isOrphanT >(&owner, ID), mValue(value)
			{

			}
		CoreModifiableAttributeData(CoreModifiable& owner, KigsID ID) : CoreModifiableAttributeTemplated<notifOwnerT, isInitT, isReadOnlyT, isDynamicT, isOrphanT>(&owner, ID), mValue{}
			{

			}
		CoreModifiableAttributeData(KigsID ID, const T& value) : CoreModifiableAttributeTemplated<notifOwnerT, isInitT, isReadOnlyT, isDynamicT, isOrphanT>(nullptr, ID), mValue{ value }
			{
			static_assert(isOrphanT == true,"CoreModifiableAttribute with no owner must be constructed with isOrphanT == true");
			}

		explicit CoreModifiableAttributeData(InheritanceSwitch tag) : CoreModifiableAttributeTemplated<notifOwnerT, isInitT, isReadOnlyT, isDynamicT, isOrphanT>(tag) {}


			inline T& ref() { return mValue; }
			inline const T& const_ref() const { return mValue; }

		void* getRawValue(CoreModifiable* owner) override { return static_cast<void*>(&mValue); }

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

		virtual void changeInheritance(u8 mask) override
			{
			u32	keepID = mID;
			u32 keepPlaceHolder = mPlaceHolderForSonClasses;

				T old_value = mValue;
				mValue.~T();
			this->doPlacementNew(mask);
				mValue = old_value;

			mID= keepID;
			mPlaceHolderForSonClasses =keepPlaceHolder;

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
			if (this->getType() == other.getType() &&
				this->getArrayElementType() == other.getArrayElementType() &&
				this->getNbArrayColumns() == other.getNbArrayColumns() &&
				this->getNbArrayLines() == other.getNbArrayLines())
				{
					CopyData(static_cast<const CoreModifiableAttributeData<T>&>(other));
					return true;
				}
				return false;
#endif
			}

			virtual void CopyData(const CoreModifiableAttributeData<T>& other)
			{
			mValue = ((const CoreModifiableAttributeData<T, notifOwnerT, isInitT, isReadOnlyT, isDynamicT, isOrphanT>*)(&other))->mValue;
			}
		};

#define DO_NOTIFICATION(level)				if constexpr(level){owner->NotifyUpdate(CoreModifiableAttribute::mID);}
#define RETURN_ON_READONLY(readonly)		if constexpr(readonly) {return false;}



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

	template<bool notificationLevel, bool isInitT = false, bool isReadOnlyT = false, bool isDynamicT = false, bool isOrphanT = false>
	class maRawPtrHeritage : public CoreModifiableAttributeData<void*, notificationLevel, isInitT, isReadOnlyT, isDynamicT, isOrphanT>
		{
			DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maRawPtrHeritage, maRawPtrHeritage, void*, CoreModifiable::ATTRIBUTE_TYPE::RAWPTR);
		public:

		bool getValue(void*& value, const CoreModifiable* owner) const override { value = mValue; return true; }
			bool setValue(void* value, CoreModifiable* owner) override
			{
			RETURN_ON_READONLY(isReadOnlyT);
				mValue = value;
				DO_NOTIFICATION(notificationLevel);
				return true;
			}

		};


	using maRawPtr = maRawPtrHeritage<false, false, false, false, false>;
	using maRawPtrOrphan = maRawPtrHeritage<false, false, false, false, true>;


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
