#pragma once

#ifdef STRING
#undef STRING
#endif

#include "CoreModifiable.h"
#include "CoreItem.h"
#include "usString.h"
#include "CoreAttributeTypeTemplateConverter.h"
#include "TecLibs/Tec3D.h"
#include <any>
#include <type_traits>

namespace Kigs
{
	namespace Core
	{

		// Tag type for inheritance switch constructor
		struct InheritanceSwitch {};


#define STATIC_ASSERT_NOTIF_LEVEL_SIZES(classname) static_assert(sizeof(classname<false>) == sizeof(classname<true>), "Size mismatch between notification levels");


#define DECLARE_ATTRIBUTE_USINGS(underlying_type)\
protected:\
	using CoreModifiableAttributeData<underlying_type,notificationLevel,isInitT,isReadOnlyT,isOrphanT>::mValue;\
public:\
	using CoreModifiableAttributeData<underlying_type,notificationLevel,isInitT,isReadOnlyT,isOrphanT>::isReadOnly;

#define DECLARE_ATTRIBUTE_HERITAGE_IMPL(name, template_name, underlying_type, enum_type)\
public:\
using CoreModifiableAttributeData<underlying_type,notificationLevel,isInitT,isReadOnlyT,isOrphanT>::CoreModifiableAttributeData;\
typedef name CurrentAttributeClass;\
typedef underlying_type CurrentAttributeType;\
static constexpr CoreModifiable::ATTRIBUTE_TYPE type = enum_type;\
CoreModifiable::ATTRIBUTE_TYPE getType() const override { return enum_type; }\
DECLARE_ATTRIBUTE_USINGS(underlying_type)\
auto& operator=(const template_name<notificationLevel,isInitT,isReadOnlyT,isOrphanT>& attribute)\
{\
	this->CopyData(attribute);\
	return *this; \
}\
protected:\
void doPlacementNew(u8 mask) override\
{\
	switch (mask)\
	{\
		case 0: new (this) template_name<false,false,false,false>(InheritanceSwitch{}); break; \
		case 1: new (this) template_name<true,false,false,false>(InheritanceSwitch{}); break; \
		case 2: new (this) template_name<false,true,false,false>(InheritanceSwitch{}); break; \
		case 3: new (this) template_name<true,true,false,false>(InheritanceSwitch{}); break; \
		case 4: new (this) template_name<false,false,true,false>(InheritanceSwitch{}); break; \
		case 5: new (this) template_name<true,false,true,false>(InheritanceSwitch{}); break; \
		case 6: new (this) template_name<false,true,true,false>(InheritanceSwitch{}); break; \
		case 7: new (this) template_name<true,true,true,false>(InheritanceSwitch{}); break; \
		case 16: new (this) template_name<false,false,false,true>(InheritanceSwitch{}); break; \
		case 20: new (this) template_name<false,false,true,true>(InheritanceSwitch{}); break; \
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

			template<bool notifOwnerTe, bool isInitParamTe, bool isReadOnlyTe, bool isOrphanTe>
			static u8 getMask()
			{
				u8 mask = (notifOwnerTe ? 1 : 0) | (isInitParamTe ? 2 : 0) | (isReadOnlyTe ? 4 : 0) | (isOrphanTe ? 8 : 0);
				return mask;
			}

			//virtual void setDynamic(bool val, CoreModifiable* owner, const KigsID& ID) = 0;
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
			virtual bool isOrphan() const = 0;
			virtual void setReadOnly(bool val) = 0;
			virtual void setInitParam(bool val) = 0;

			virtual void* getRawValue(CoreModifiable* owner) = 0;

			virtual size_t	size() const { return 1; };
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
		template<bool notifOwnerT = false, bool isInitParamT = false, bool isReadOnlyT = false, bool isOrphanT = false>
		class CoreModifiableAttributeTemplated : public CoreModifiableAttribute
		{

		protected:

			CoreModifiableAttributeTemplated(CoreModifiable* owner, KigsID ID) : CoreModifiableAttribute(owner, ID)
			{
				if (owner)
					owner->addAttribute(this, ID, false);
			}

			explicit CoreModifiableAttributeTemplated(CoreModifiable* owner, KigsID ID, DynamicSwitch tag) : CoreModifiableAttribute(owner, ID)
			{
				if (owner)
					owner->addAttribute(this, ID, true);
			}

			u8 getCurrentMask()
			{
				u8 mask = (notifOwnerT ? 1 : 0) | (isInitParamT ? 2 : 0) | (isReadOnlyT ? 4 : 0) | (isOrphanT ? 8 : 0);
				return mask;
			}

			friend class CoreModifiable;

			virtual void setNotifyOwner(bool val) final
			{
				if (val != notifOwnerT)
				{
					if (val)
					{
						changeInheritance(CoreModifiableAttribute::getMask<true, isInitParamT, isReadOnlyT, isOrphanT>());
					}
					else
					{
						changeInheritance(CoreModifiableAttribute::getMask<false, isInitParamT, isReadOnlyT, isOrphanT>());
					}
				}
			}

		public:
			explicit CoreModifiableAttributeTemplated(InheritanceSwitch tag) : CoreModifiableAttribute(tag) {}


			//! Read only attributes cannot be modified with setValue
			virtual bool isReadOnly() const override { return isReadOnlyT; }
			//! \brief  return true if attribute is an init attribute (necessary for the CoreModifiable Init to be done)
			virtual bool isInitParam() const override { return isInitParamT; }
			virtual bool isOrphan() const override { return isOrphanT; }

			// only readOnly & notif level can be changed
			virtual void setReadOnly(bool val) final
			{
				if (val != isReadOnlyT)
				{
					if (val)
					{
						changeInheritance(getMask<notifOwnerT, isInitParamT, true, isOrphanT>());
					}
					else
					{
						changeInheritance(getMask<notifOwnerT, isInitParamT, false, isOrphanT>());
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
						changeInheritance(getMask<notifOwnerT, true, isReadOnlyT, isOrphanT>());
					}
					else
					{
						changeInheritance(getMask<notifOwnerT, false, isReadOnlyT, isOrphanT>());
					}
				}
			}
		};

#define DO_NOTIFICATION(level)				if constexpr(level){owner->NotifyUpdate(CoreModifiableAttribute::mID);}
#define RETURN_ON_READONLY(readonly)		if constexpr(readonly) {return false;}

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

		template<u32 valueAccessor,typename T, bool notifOwnerT = false, bool isInitT = false, bool isReadOnlyT = false, bool isOrphanT = false>
		class CoreModifiableAttributeDataInterface : public CoreModifiableAttributeTemplated< notifOwnerT, isInitT, isReadOnlyT, isOrphanT>
		{
			using CoreModifiableAttribute::mID;
			using CoreModifiableAttribute::mPlaceHolderForSonClasses;

		protected:

			inline T& valueProtectedAccess(const void* instance);

			inline const T& valueProtectedAccess(const void* instance) const;

		public:
			
			CoreModifiableAttributeDataInterface(CoreModifiable* owner, KigsID ID) : CoreModifiableAttributeTemplated<notifOwnerT, isInitT, isReadOnlyT, isOrphanT>(owner, ID)
			{

			}

			explicit CoreModifiableAttributeDataInterface(InheritanceSwitch tag) : CoreModifiableAttributeTemplated<notifOwnerT, isInitT, isReadOnlyT, isOrphanT>(tag) {}


			inline T& ref() { return valueProtectedAccess(nullptr); }
			inline const T& const_ref() const { return valueProtectedAccess(nullptr); }

			void* getRawValue(CoreModifiable* owner) override { return static_cast<void*>(&valueProtectedAccess(owner)); }

			friend class CoreModifiable;

			size_t MemorySize() const override { return sizeof(T); };

			CoreModifiableAttributeDataInterface<valueAccessor,T>& operator=(const CoreModifiableAttributeDataInterface<valueAccessor,T>& other)
			{
				CopyData(other);
				return *this;
			}

			size_t getNbArrayElements() const override {
				if constexpr (impl::is_array<std::remove_cv_t<T>>::value)
				{
					return impl::arraySize<T>();
				}
				return 0;
			}
			size_t getNbArrayColumns() const override {
				if constexpr (impl::is_array<std::remove_cv_t<T>>::value)
				{
					return impl::arrayColumnCount<T>();
				}
				return 0;
			}
			size_t getNbArrayLines() const override {
				if constexpr (impl::is_array<std::remove_cv_t<T>>::value)
				{
					return impl::arrayLineCount<T>();
				}
				return 0;
			}

#define DECLARE_SET(type)	virtual bool setValue(type val,CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); if( CoreConvertValue(val,valueProtectedAccess(owner)) ) { DO_NOTIFICATION(notifOwnerT); return true;} return false;  }

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SET);
			DECLARE_SET(const char*);
			DECLARE_SET(const std::string&);
			DECLARE_SET(const unsigned short*);
			DECLARE_SET(const usString&);
			DECLARE_SET(const UTF8Char*);
			DECLARE_SET(const v2f&);
			DECLARE_SET(const v3f&);
			DECLARE_SET(const v4f&);

#define DECLARE_GET(type)	virtual bool getValue(type val,const CoreModifiable* owner) const override { return CoreConvertValue(valueProtectedAccess(owner),val); }

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, DECLARE_GET);
			DECLARE_GET(std::string&);
			DECLARE_GET(usString&);
			DECLARE_GET(v2f&);
			DECLARE_GET(v3f&);
			DECLARE_GET(v4f&);


#define DECLARE_SETARRAYVALUE(type)	virtual bool setArrayValue(type val,CoreModifiable* owner, size_t nbElements ) override {RETURN_ON_READONLY(isReadOnlyT);\
		if constexpr(impl::is_array<std::remove_cv_t<T>>::value) {\
			size_t currentColumnIndex=0, currentLineIndex=0;\
			for (size_t i = 0; i < nbElements; i++){\
				if constexpr(impl::arrayLineCount<std::remove_cv_t<T>>()>1){\
					if (!CoreConvertValue(val[i], valueProtectedAccess(owner)[currentColumnIndex][currentLineIndex])) { return false; }\
				}else {\
					if (!CoreConvertValue(val[i], valueProtectedAccess(owner)[currentColumnIndex])) { return false; }\
				}\
				currentLineIndex++;\
				if (currentLineIndex >= impl::arrayLineCount<std::remove_cv_t<T>>()){\
					currentColumnIndex++; currentLineIndex=0;\
					if (impl::arrayColumnCount<std::remove_cv_t<T>>() <= currentColumnIndex){break;}\
				}}\
			DO_NOTIFICATION(notifOwnerT); return true;}\
		return false; }

			EXPAND_MACRO_FOR_BASE_TYPES(const, *, DECLARE_SETARRAYVALUE);

#define DECLARE_GETARRAYVALUE(type) virtual bool getArrayValue(type * const  val  ,const CoreModifiable* owner, size_t  nbElements ) const override {\
		if constexpr(impl::is_array<std::remove_cv_t<T>>::value) {\
			size_t currentColumnIndex=0, currentLineIndex=0;\
			for (size_t i = 0; i < nbElements; i++){\
				if constexpr(impl::arrayLineCount<std::remove_cv_t<T>>()>1){\
					if (!CoreConvertValue(valueProtectedAccess(owner)[currentColumnIndex][currentLineIndex],val[i])) { return false; }\
				}else {\
					if (!CoreConvertValue(valueProtectedAccess(owner)[currentColumnIndex],val[i])) { return false; }\
				}\
				currentLineIndex++;\
				if (currentLineIndex >= impl::arrayLineCount<std::remove_cv_t<T>>()){\
					currentColumnIndex++; currentLineIndex=0;\
					if (impl::arrayColumnCount<std::remove_cv_t<T>>() <= currentColumnIndex){break;}\
				}}\
			 return true;}\
		return false; }

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_GETARRAYVALUE);

#define DECLARE_SETARRAYELEMENTVALUE(type)	virtual bool setArrayElementValue(type val,CoreModifiable* owner, size_t  line , size_t  column ) override {RETURN_ON_READONLY(isReadOnlyT);\
		if constexpr(impl::is_array<std::remove_cv_t<T>>::value) {\
			if( (line >= impl::arrayLineCount<std::remove_cv_t<T>>()) || (column >= impl::arrayColumnCount<std::remove_cv_t<T>>())) {return false;}\
			if constexpr(impl::arrayLineCount<std::remove_cv_t<T>>()>1){\
				if (!CoreConvertValue(val, valueProtectedAccess(owner)[column][line])) { return false; }\
			}else{\
				if (!CoreConvertValue(val, valueProtectedAccess(owner)[column])) { return false; }\
			}\
			DO_NOTIFICATION(notifOwnerT);\
			return true;}\
		return false; }

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SETARRAYELEMENTVALUE);
			DECLARE_SETARRAYELEMENTVALUE(const std::string&);

#define DECLARE_GETARRAYELEMENTVALUE(type)	virtual bool getArrayElementValue(type  val , const CoreModifiable* owner, size_t  line , size_t  column ) const override{\
		if constexpr(impl::is_array<std::remove_cv_t<T>>::value) {\
			if( (line >= impl::arrayLineCount<std::remove_cv_t<T>>()) || (column >= impl::arrayColumnCount<std::remove_cv_t<T>>())) {return false;}\
			if constexpr(impl::arrayLineCount<std::remove_cv_t<T>>()>1){\
				if (!CoreConvertValue(valueProtectedAccess(owner)[column][line],val)) {return false;}\
			}else{\
				if (!CoreConvertValue(valueProtectedAccess(owner)[column],val)) {return false;}\
			}\
			return true;}\
		return false; }

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, DECLARE_GETARRAYELEMENTVALUE);
			DECLARE_GETARRAYELEMENTVALUE(std::string&);

#undef DECLARE_SET
#undef DECLARE_GET
#undef DECLARE_SETARRAYVALUE
#undef DECLARE_GETARRAYVALUE
#undef DECLARE_SETARRAYELEMENTVALUE
#undef DECLARE_GETARRAYELEMENTVALUE

			template<typename U>
			T operator+=(U value)
			{
				if constexpr(std::is_arithmetic<U>::value && std::is_arithmetic<T>::value)
				{
					valueProtectedAccess(nullptr) += (T)value;
				}
				else if constexpr ((std::is_same<U, std::string>::value || std::is_same<U, usString>::value || std::is_same<U, const char*>::value) && (std::is_same<T, std::string>::value || std::is_same<T, usString>::value))
				{
					valueProtectedAccess(nullptr) += (T)value;
				}
				return valueProtectedAccess(nullptr);
			}
			template<typename U>
			T operator-=(U value)
			{
				if constexpr (std::is_arithmetic<U>::value && std::is_arithmetic<T>::value)
				{
					valueProtectedAccess(nullptr) -= (T)value;
				}
				return valueProtectedAccess(nullptr);
			}
			template<typename U>
			T operator*=(U value)
			{
				if constexpr (std::is_arithmetic<U>::value && std::is_arithmetic<T>::value)
				{
					valueProtectedAccess(nullptr) *= (T)value;
				}
				return valueProtectedAccess(nullptr);
			}
			template<typename U>
			T operator/=(U value)
			{
				if constexpr (std::is_arithmetic<U>::value && std::is_arithmetic<T>::value)
				{
					valueProtectedAccess(nullptr) /= (T)value;
				}
				return valueProtectedAccess(nullptr);
			}
			template<typename U>
			T operator|=(U value)
			{
				if constexpr (std::is_integral<U>::value && std::is_integral<T>::value)
				{
					valueProtectedAccess(nullptr) |= (T)value;
				}
				return valueProtectedAccess(nullptr);
			}
			template<typename U>
			T operator^=(U value)
			{
				if constexpr (std::is_integral<U>::value && std::is_integral<T>::value)
				{
					valueProtectedAccess(nullptr) ^= (T)value;
				}
				return valueProtectedAccess(nullptr);
			}
			template<typename U>
			T operator&=(U value)
			{
				if constexpr (std::is_integral<U>::value && std::is_integral<T>::value)
				{
					valueProtectedAccess(nullptr) &= (T)value;
				}
				return valueProtectedAccess(nullptr);
			}

			CoreModifiable::ATTRIBUTE_TYPE getType() const override { return TypeToEnum<T>::value; }
			CoreModifiable::ATTRIBUTE_TYPE getArrayElementType() const override { return AraryTypeToEnum<T>::value; }

		protected:

			explicit CoreModifiableAttributeDataInterface(CoreModifiable& owner, KigsID ID, DynamicSwitch tag) : CoreModifiableAttributeTemplated<notifOwnerT, isInitT, isReadOnlyT, isOrphanT >(&owner, ID, tag)
			{
			}

			// Uses RTTI dynamic cast to check for compatibility by default
			// If we ever add attributes that are not CoreModifiableAttributeData this would need to be changed/overloaded for each type
			bool CopyAttribute(const CoreModifiableAttribute& other) override
			{
				if (this->getType() == other.getType() &&
				this->getArrayElementType() == other.getArrayElementType() &&
				this->getNbArrayColumns() == other.getNbArrayColumns() &&
				this->getNbArrayLines() == other.getNbArrayLines())
				{
					CopyData(other);
					return true;
				}
				return false;
			}

			virtual void CopyData(const CoreModifiableAttribute& other) = 0;
			
		};

		template<typename T, bool notifOwnerT = false, bool isInitT = false, bool isReadOnlyT = false, bool isOrphanT = false>
		class CoreModifiableAttributeData : public CoreModifiableAttributeDataInterface<0, T, notifOwnerT, isInitT, isReadOnlyT, isOrphanT>
		{
			using CoreModifiableAttribute::mID;
			using CoreModifiableAttribute::mPlaceHolderForSonClasses;

		public:
			CoreModifiableAttributeData(CoreModifiable& owner, KigsID ID, const T& value) : CoreModifiableAttributeDataInterface<0,T,notifOwnerT, isInitT, isReadOnlyT, isOrphanT >(&owner, ID), mValue(value)
			{

			}
			CoreModifiableAttributeData(CoreModifiable& owner, KigsID ID) : CoreModifiableAttributeDataInterface<0, T, notifOwnerT, isInitT, isReadOnlyT, isOrphanT>(&owner, ID), mValue{}
			{

			}
			CoreModifiableAttributeData(KigsID ID, const T& value) : CoreModifiableAttributeDataInterface<0, T, notifOwnerT, isInitT, isReadOnlyT, isOrphanT>(nullptr, ID), mValue{ value }
			{
				static_assert(isOrphanT == true, "CoreModifiableAttribute with no owner must be constructed with isOrphanT == true");
			}

			explicit CoreModifiableAttributeData(InheritanceSwitch tag) : CoreModifiableAttributeDataInterface<0, T, notifOwnerT, isInitT, isReadOnlyT, isOrphanT>(tag) {}

			operator T& ()
			{
				return mValue;
			}

			operator const T& () const
			{
				return mValue;
			}

			auto& operator=(const T& value)
			{
				mValue = value;
				return *this;
			}

			friend class CoreModifiable;

		protected:
			T	mValue;

			void doPlacementNew(u8 mask) override
			{
				switch (mask)
				{
				case 0: new (this) CoreModifiableAttributeData< T, false, false, false, false>(InheritanceSwitch{}); break;
				case 1: new (this) CoreModifiableAttributeData< T, true, false, false, false>(InheritanceSwitch{}); break;
				case 2: new (this) CoreModifiableAttributeData< T, false, true, false, false>(InheritanceSwitch{}); break;
				case 3: new (this) CoreModifiableAttributeData< T, true, true, false, false>(InheritanceSwitch{}); break;
				case 4: new (this) CoreModifiableAttributeData< T, false, false, true, false>(InheritanceSwitch{}); break;
				case 5: new (this) CoreModifiableAttributeData< T, true, false, true, false>(InheritanceSwitch{}); break;
				case 6: new (this) CoreModifiableAttributeData< T, false, true, true, false>(InheritanceSwitch{}); break;
				case 7: new (this) CoreModifiableAttributeData< T, true, true, true, false>(InheritanceSwitch{}); break;
				case 16: new (this) CoreModifiableAttributeData< T, false, false, false, true>(InheritanceSwitch{}); break;
				case 20: new (this) CoreModifiableAttributeData< T, false, false, true, true>(InheritanceSwitch{}); break;
				default: assert(false); break;
				}
			}

			void CopyData(const CoreModifiableAttribute& other) override
			{
				CopyData(static_cast<const CoreModifiableAttributeData&>(other));
			}

			virtual void CopyData(const CoreModifiableAttributeData& other)
			{
				mValue = other.mValue;
			}

			explicit CoreModifiableAttributeData(CoreModifiable& owner, KigsID ID, DynamicSwitch tag) : CoreModifiableAttributeDataInterface<0,T,notifOwnerT, isInitT, isReadOnlyT, isOrphanT >(owner, ID, tag)
			{
			}

			virtual void changeInheritance(u8 mask) override
			{
				u32	keepID = mID;
				u32 keepPlaceHolder = mPlaceHolderForSonClasses;

				T old_value = mValue;
				mValue.~T();
				this->doPlacementNew(mask);
				mValue = old_value;

				mID = keepID;
				mPlaceHolderForSonClasses = keepPlaceHolder;

			}
		};

		template<typename T, bool notifOwnerT = false, bool isInitT = false, bool isReadOnlyT = false, bool isOrphanT = false>
		class CoreModifiableAttributeInherited : public CoreModifiableAttributeDataInterface<1, T, notifOwnerT, isInitT, isReadOnlyT, isOrphanT> , public T
		{
			using CoreModifiableAttribute::mID;
			using CoreModifiableAttribute::mPlaceHolderForSonClasses;

		public:
			CoreModifiableAttributeInherited(CoreModifiable& owner, KigsID ID, const T& value) : CoreModifiableAttributeDataInterface<1, T, notifOwnerT, isInitT, isReadOnlyT, isOrphanT >(&owner, ID), T(value)
			{

			}
			CoreModifiableAttributeInherited(CoreModifiable& owner, KigsID ID) : CoreModifiableAttributeDataInterface<1, T, notifOwnerT, isInitT, isReadOnlyT, isOrphanT>(&owner, ID), T{}
			{

			}
			CoreModifiableAttributeInherited(KigsID ID, const T& value) : CoreModifiableAttributeDataInterface<1, T, notifOwnerT, isInitT, isReadOnlyT, isOrphanT>(nullptr, ID), T{ value }
			{
				static_assert(isOrphanT == true, "CoreModifiableAttribute with no owner must be constructed with isOrphanT == true");
			}

			explicit CoreModifiableAttributeInherited(InheritanceSwitch tag) : CoreModifiableAttributeDataInterface<1, T, notifOwnerT, isInitT, isReadOnlyT, isOrphanT>(tag) {}

			using T::T;
			using T::operator =;

			friend class CoreModifiable;

		protected:

			void doPlacementNew(u8 mask) override
			{
				switch (mask)
				{
				case 0: new (this) CoreModifiableAttributeInherited< T, false, false, false, false>(InheritanceSwitch{}); break;
				case 1: new (this) CoreModifiableAttributeInherited< T, true, false, false, false>(InheritanceSwitch{}); break;
				case 2: new (this) CoreModifiableAttributeInherited< T, false, true, false, false>(InheritanceSwitch{}); break;
				case 3: new (this) CoreModifiableAttributeInherited< T, true, true, false, false>(InheritanceSwitch{}); break;
				case 4: new (this) CoreModifiableAttributeInherited< T, false, false, true, false>(InheritanceSwitch{}); break;
				case 5: new (this) CoreModifiableAttributeInherited< T, true, false, true, false>(InheritanceSwitch{}); break;
				case 6: new (this) CoreModifiableAttributeInherited< T, false, true, true, false>(InheritanceSwitch{}); break;
				case 7: new (this) CoreModifiableAttributeInherited< T, true, true, true, false>(InheritanceSwitch{}); break;
				case 16: new (this) CoreModifiableAttributeInherited< T, false, false, false, true>(InheritanceSwitch{}); break;
				case 20: new (this) CoreModifiableAttributeInherited< T, false, false, true, true>(InheritanceSwitch{}); break;
				default: assert(false); break;
				}
			}

			void CopyData(const CoreModifiableAttribute& other) override
			{
				CopyData(static_cast<const CoreModifiableAttributeInherited&>(other));
			}

			virtual void CopyData(const CoreModifiableAttributeInherited& other)
			{
				(T&)(*this) = (const T&)other;
			}

			explicit CoreModifiableAttributeInherited(CoreModifiable& owner, KigsID ID, DynamicSwitch tag) : CoreModifiableAttributeDataInterface<1, T, notifOwnerT, isInitT, isReadOnlyT, isOrphanT >(owner, ID, tag)
			{
			}

			virtual void changeInheritance(u8 mask) override
			{
				u32	keepID = mID;
				u32 keepPlaceHolder = mPlaceHolderForSonClasses;

				T old_value = *this;
				(*((T*)this)).~T();
				this->doPlacementNew(mask);
				(*((T*)this)) = old_value;

				mID = keepID;
				mPlaceHolderForSonClasses = keepPlaceHolder;
			}
		};



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

		template<bool notificationLevel, bool isInitT = false, bool isReadOnlyT = false, bool isOrphanT = false>
		class maRawPtrHeritage : public CoreModifiableAttributeData<void*, notificationLevel, isInitT, isReadOnlyT, isOrphanT>
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


		using maRawPtr = maRawPtrHeritage<false, false, false, false>;
		using maRawPtrOrphan = maRawPtrHeritage<false, false, false, true>;

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
				mAttr->getValue(tmp, mOwner);
			}
			return tmp;
		}

		template<typename T>
		inline const CMSP::AttributeHolder& CMSP::AttributeHolder::operator =(T toset) const {
			if (mAttr)
			{
				mAttr->setValue(toset, mOwner);
			}
			return *this;
		}

		template<typename T>
		inline const bool CMSP::AttributeHolder::operator ==(T totest) const {
			if (mAttr)
			{
				T	tmp{};
				if (mAttr->getValue(tmp, mOwner))
				{
					return tmp == totest;
				}
			}
			return false;
		}
	}
}

// mapped attributes
#include "CoreModifiableAttributeMap.h"

namespace Kigs
{
	namespace Core
	{
		template<u32 valueAccessor, typename T, bool notifOwnerT, bool isInitT, bool isReadOnlyT, bool isOrphanT >
		inline T& CoreModifiableAttributeDataInterface<valueAccessor, T, notifOwnerT, isInitT, isReadOnlyT, isOrphanT>::valueProtectedAccess(const void* instance)
		{
			if constexpr (valueAccessor == 0) // classic access with embedded value
			{
				return (T&)*((CoreModifiableAttributeData<T, notifOwnerT, isInitT, isReadOnlyT, isOrphanT>*)this);
			}
			else if constexpr (valueAccessor == 1) // inherited value
			{
				return (T&)*((CoreModifiableAttributeInherited<T, notifOwnerT, isInitT, isReadOnlyT, isOrphanT>*)this);
			}
			else if constexpr (valueAccessor == 2) // mapped value
			{
				uintptr_t pos((uintptr_t)instance);
				pos += mPlaceHolderForSonClasses;
				return *((T*)pos);
			}
		}

		template<u32 valueAccessor, typename T, bool notifOwnerT, bool isInitT, bool isReadOnlyT, bool isOrphanT >
		inline const T& CoreModifiableAttributeDataInterface<valueAccessor, T, notifOwnerT, isInitT, isReadOnlyT, isOrphanT>::valueProtectedAccess(const void* instance) const
		{
			if constexpr (valueAccessor == 0) // classic access with embedded value
			{
				return (const T&)*((const CoreModifiableAttributeData<T, notifOwnerT, isInitT, isReadOnlyT, isOrphanT>*)this);
			}
			else if constexpr (valueAccessor == 1) // inherited value
			{
				return (const T&)*((const CoreModifiableAttributeInherited<T, notifOwnerT, isInitT, isReadOnlyT, isOrphanT>*)this);
			}
			else if constexpr (valueAccessor == 2) // mapped value
			{
				uintptr_t pos((uintptr_t)instance);
				pos += mPlaceHolderForSonClasses;
				return *((const T*)pos);
			}
		}

		template<typename valT>
		void	CoreModifiable::addMappedAttribute(uintptr_t offset, const std::string& c, std::vector<std::pair<KigsID, CoreModifiableAttribute*>>* parent, CoreModifiableAttribute* toAdd)
		{
			// remove m
			std::string attrname = c.substr(1);
			// do placement new 
			new (toAdd) CoreModifiableAttributeMap<valT,false,false,false>(offset, attrname);
			parent->push_back({ attrname, toAdd });
		}
	}
}