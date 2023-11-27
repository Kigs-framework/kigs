#pragma once

#include "CoreModifiableAttribute.h"

namespace Kigs
{
	namespace Core
	{
		template<typename T>
		using maNumeric = CoreModifiableAttributeData<T,false, false, false, false>;

		template<typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType = TypeToEnum<T>::value>
		using maNumericOrphan = CoreModifiableAttributeData<T,false, false, false, true>;

		template<typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType = TypeToEnum<T>::value>
		using maNumericInit = CoreModifiableAttributeData<T, false, true>;

		using maBool = maNumeric<bool>;
		using maBoolInit = maNumericInit<bool>;
		using maBoolOrphan = maNumericOrphan<bool>;

		using maChar = maNumeric<s8>;
		using maCharOrphan = maNumericOrphan<s8>;
		using maCharInit = maNumericInit<s8>;
		using maShort = maNumeric<s16>;
		using maShortOrphan = maNumericOrphan<s16>;
		using maShortInit = maNumericInit<s16>;
		using maInt = maNumeric<s32>;
		using maIntOrphan = maNumericOrphan<s32>;
		using maIntInit = maNumericInit<s32>;
		using maLong = maNumeric<s64>;
		using maLongOrphan = maNumericOrphan<s64>;
		using maLongInit = maNumericInit<s64>;
		using maUChar = maNumeric<u8>;
		using maUCharOrphan = maNumericOrphan<u8>;
		using maUCharInit = maNumericInit<u8>;
		using maUShort = maNumeric<u16>;
		using maUShortOrphan = maNumericOrphan<u16>;
		using maUShortInit = maNumericInit<u16>;
		using maUInt = maNumeric<u32>;
		using maUIntOrphan = maNumericOrphan<u32>;
		using maUIntInit = maNumericInit<u32>;
		using maULong = maNumeric<u64>;
		using maULongOrphan = maNumericOrphan<u64>;
		using maULongInit = maNumericInit<u64>;
		using maFloat = maNumeric<float>;
		using maFloatOrphan = maNumericOrphan<float>;
		using maFloatInit = maNumericInit<float>;
		using maDouble = maNumeric<double>;
		using maDoubleOrphan = maNumericOrphan<double>;
		using maDoubleInit = maNumericInit<double>;

		// ****************************************
		// * maComputedNumericHeritage class
		// * --------------------------------------
		/**
		* \class	maComputedNumericHeritage
		* \ingroup CoreModifiableAttibute
		* \brief	numeric calling onwer get / set method to compute value
		*/
		// ****************************************
		template<bool notifOwnerT, typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType, bool isInitT = false, bool isReadOnlyT = false, bool isOrphanT = false>
		class maComputedNumericHeritage : public CoreModifiableAttributeTemplated<notifOwnerT, isInitT, isReadOnlyT, isOrphanT>
		{
			template<bool notiflevel, bool isInitTe, bool isReadOnlyTe, bool isOrphanTe>
			using TemplateForPlacementNew = maComputedNumericHeritage<notiflevel, T, attributeType, isInitTe, isReadOnlyTe, isOrphanTe>;


		private:

			maComputedNumericHeritage(CoreModifiable& owner, KigsID ID, const T& value) : CoreModifiableAttributeTemplated<notifOwnerT, isInitT, isReadOnlyT, isOrphanT>(&owner, ID)
			{

			}
			maComputedNumericHeritage(CoreModifiable& owner, KigsID ID) : CoreModifiableAttributeTemplated<notifOwnerT, isInitT, isReadOnlyT, isOrphanT>(&owner, ID)
			{

			}
			maComputedNumericHeritage(KigsID ID, const T& value) : CoreModifiableAttributeTemplated<notifOwnerT, isInitT, isReadOnlyT, isOrphanT>(nullptr, false, ID)
			{
			}

		public:

			explicit maComputedNumericHeritage(InheritanceSwitch tag) : CoreModifiableAttributeTemplated<notifOwnerT, isInitT, isReadOnlyT, isOrphanT>(tag) {}

			maComputedNumericHeritage(CoreModifiable& owner, KigsID ID, KigsID g, KigsID s) : CoreModifiableAttributeTemplated<notifOwnerT, isInitT, isReadOnlyT, isOrphanT>(&owner, ID), mGetter(g), mSetter(s)
			{

			}

			typedef maComputedNumericHeritage CurrentAttributeClass;
			typedef T CurrentAttributeType;
			CoreModifiable::ATTRIBUTE_TYPE getType() const override { return attributeType; }

			auto& operator=(const TemplateForPlacementNew<notifOwnerT, isInitT, isReadOnlyT, isOrphanT>& attribute)
			{
				this->CopyData(attribute);
				return *this;
			}
		protected:

			KigsID	mGetter;
			KigsID	mSetter;

			void doPlacementNew(u8 mask) override
			{
				switch (mask)
				{
				case 0: new (this) TemplateForPlacementNew<false, false, false, false>(InheritanceSwitch{}); break;
				case 1: new (this) TemplateForPlacementNew<true, false, false, false>(InheritanceSwitch{}); break;
				case 2: new (this) TemplateForPlacementNew<false, true, false, false>(InheritanceSwitch{}); break;
				case 3: new (this) TemplateForPlacementNew<true, true, false, false>(InheritanceSwitch{}); break;
				case 4: new (this) TemplateForPlacementNew<false, false, true, false>(InheritanceSwitch{}); break;
				case 5: new (this) TemplateForPlacementNew<true, false, true, false>(InheritanceSwitch{}); break;
				case 6: new (this) TemplateForPlacementNew<false, true, true, false>(InheritanceSwitch{}); break;
				case 7: new (this) TemplateForPlacementNew<true, true, true, false>(InheritanceSwitch{}); break;
				case 16: new (this) TemplateForPlacementNew<false, false, false, true>(InheritanceSwitch{}); break;
				case 20: new (this) TemplateForPlacementNew<false, false, true, true>(InheritanceSwitch{}); break;
				default: assert(false); break;
				}
			}
			virtual void changeInheritance(u8 mask) final
			{
				KigsID old_getter = mGetter;
				mGetter.~KigsID();
				KigsID old_setter = mSetter;
				mSetter.~KigsID();
				doPlacementNew(mask);
				mGetter = old_getter;
				mSetter = old_setter;
			}

		public:

			virtual void* getRawValue(CoreModifiable* owner) final { return nullptr; }
			bool CopyAttribute(const CoreModifiableAttribute& other) override
			{
				return false;
			}

#define DECLARE_SET(type)	bool setValue(type val,CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue; if( CoreConvertValue(val,tmpValue) ) { if (mSetter != "")owner->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notifOwnerT); return true;} return false;  }

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SET);
			DECLARE_SET(const char*);
			DECLARE_SET(const std::string&);
			DECLARE_SET(const unsigned short*);
			DECLARE_SET(const usString&);
			DECLARE_SET(const UTF8Char*);
			DECLARE_SET(const v2f&);
			DECLARE_SET(const p3f&);
			DECLARE_SET(const v4f&);

#define DECLARE_GET(type)	bool getValue(type val,const CoreModifiable* owner) const override { if(mGetter!=""){ T tmpValue = const_cast<CoreModifiable*>(owner)->template SimpleCall<T>(mGetter);  return CoreConvertValue(tmpValue,val);} return false; }

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, DECLARE_GET);
			DECLARE_GET(std::string&);
			DECLARE_GET(usString&);
			DECLARE_GET(v2f&);
			DECLARE_GET(p3f&);
			DECLARE_GET(v4f&);


#define DECLARE_SETARRAYVALUE(type)	bool setArrayValue(type val,CoreModifiable* owner, size_t nbElements ) override {RETURN_ON_READONLY(isReadOnlyT);\
		if constexpr(impl::is_array<std::remove_cv_t<T>>::value) {\
			size_t currentColumnIndex=0, currentLineIndex=0; T tmpValue;\
			for (size_t i = 0; i < nbElements; i++){\
				if constexpr(impl::arrayLineCount<std::remove_cv_t<T>>()>1){\
					if (!CoreConvertValue(val[i], tmpValue[currentColumnIndex][currentLineIndex])) { return false; }\
				}else {\
					if (!CoreConvertValue(val[i], tmpValue[currentColumnIndex])) { return false; }\
				}\
				currentLineIndex++;\
				if (currentLineIndex >= impl::arrayLineCount<std::remove_cv_t<T>>()){\
					currentColumnIndex++; currentLineIndex=0;\
					if (impl::arrayColumnCount<std::remove_cv_t<T>>() <= currentColumnIndex){break;}\
				}}\
			if (mSetter != ""){owner->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notifOwnerT); return true;} }\
		return false; }

			EXPAND_MACRO_FOR_BASE_TYPES(const, *, DECLARE_SETARRAYVALUE);

#define DECLARE_GETARRAYVALUE(type) bool getArrayValue(type * const  val  ,const CoreModifiable* owner, size_t  nbElements ) const override {\
		if constexpr(impl::is_array<std::remove_cv_t<T>>::value) {\
			if(mGetter!=""){ T tmpValue = const_cast<CoreModifiable*>(owner)->template SimpleCall<T>(mGetter); \
			size_t currentColumnIndex=0, currentLineIndex=0;\
			for (size_t i = 0; i < nbElements; i++){\
				if constexpr(impl::arrayLineCount<std::remove_cv_t<T>>()>1){\
					if (!CoreConvertValue(tmpValue[currentColumnIndex][currentLineIndex],val[i])) { return false; }\
				}else {\
					if (!CoreConvertValue(tmpValue[currentColumnIndex],val[i])) { return false; }\
				}\
				currentLineIndex++;\
				if (currentLineIndex >= impl::arrayLineCount<std::remove_cv_t<T>>()){\
					currentColumnIndex++; currentLineIndex=0;\
					if (impl::arrayColumnCount<std::remove_cv_t<T>>() <= currentColumnIndex){break;}\
				}}\
			 return true;}}\
		return false; }

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_GETARRAYVALUE);

#define DECLARE_SETARRAYELEMENTVALUE(type)	bool setArrayElementValue(type val,CoreModifiable* owner, size_t  line , size_t  column ) override {RETURN_ON_READONLY(isReadOnlyT);\
		if constexpr(impl::is_array<std::remove_cv_t<T>>::value) {\
			if( (line >= impl::arrayLineCount<std::remove_cv_t<T>>()) || (column >= impl::arrayColumnCount<std::remove_cv_t<T>>())) {return false;}\
			if(mGetter==""){return false;}\
			T tmpValue = (owner)->template SimpleCall<T>(mGetter);\
			if constexpr(impl::arrayLineCount<std::remove_cv_t<T>>()>1){\
				if (!CoreConvertValue(val, tmpValue(owner)[column][line])) { return false; }\
			}else{\
				if (!CoreConvertValue(val, tmpValue(owner)[column])) { return false; }\
			}\
			if (mSetter != ""){owner->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notifOwnerT); return true;}}\
			return false;}

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SETARRAYELEMENTVALUE);
			DECLARE_SETARRAYELEMENTVALUE(const std::string&);

#define DECLARE_GETARRAYELEMENTVALUE(type)	bool getArrayElementValue(type  val , const CoreModifiable* owner, size_t  line , size_t  column ) const override{\
		if constexpr(impl::is_array<std::remove_cv_t<T>>::value) {\
			if(mGetter!=""){ T tmpValue = const_cast<CoreModifiable*>(owner)->template SimpleCall<T>(mGetter); \
			if( (line >= impl::arrayLineCount<std::remove_cv_t<T>>()) || (column >= impl::arrayColumnCount<std::remove_cv_t<T>>())) {return false;}\
			if constexpr(impl::arrayLineCount<std::remove_cv_t<T>>()>1){\
				if (!CoreConvertValue(tmpValue[column][line],val)) {return false;}\
			}else{\
				if (!CoreConvertValue(tmpValue[column],val)) {return false;}\
			}\
			return true;}}\
		return false; }

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, DECLARE_GETARRAYELEMENTVALUE);
			DECLARE_GETARRAYELEMENTVALUE(std::string&);

#undef DECLARE_SET
#undef DECLARE_GET
#undef DECLARE_SETARRAYVALUE
#undef DECLARE_GETARRAYVALUE
#undef DECLARE_SETARRAYELEMENTVALUE
#undef DECLARE_GETARRAYELEMENTVALUE

		};

		template<typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType = TypeToEnum<T>::value>
		using maComputedNumeric = maComputedNumericHeritage<false, T, attributeType,false,false,false>;

		template<typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType = TypeToEnum<T>::value>
		using maComputedNumericInit = maComputedNumericHeritage<false, T, attributeType, true>;
	}
}