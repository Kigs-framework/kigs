#pragma once

#include "CoreModifiableAttribute.h"

namespace Kigs
{
	namespace Core
	{

		// ****************************************
		// * maNumericHeritage class
		// * --------------------------------------
		/**
		* \class	maNumericHeritage
		* \ingroup CoreModifiableAttibute
		* \brief	CoreModifiableAttributeData for numeric without different level of notification
		*/
		// ****************************************
		template<bool notificationLevel, typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType, bool isInitT = false, bool isReadOnlyT = false, bool isDynamicT = false, bool isOrphanT = false> 
		class maNumericHeritage : public CoreModifiableAttributeData<T, notificationLevel, isInitT, isReadOnlyT, isDynamicT, isOrphanT>
		{
			template<bool notiflevel, bool isInitTe, bool isReadOnlyTe, bool isDynamicTe, bool isOrphanTe>
			using TemplateForPlacementNew = maNumericHeritage<notiflevel, T, attributeType, isInitTe, isReadOnlyTe, isDynamicTe, isOrphanTe>;

			DECLARE_ATTRIBUTE_HERITAGE(maNumericHeritage, TemplateForPlacementNew, T, attributeType);

		public:

			// Shouldn't they return *this instead ?
			T operator+=(T value)
			{
				mValue += value;
				return operator T();
			}
			T operator-=(T value)
			{
				mValue -= value;
				return operator T();
			}
			T operator*=(T value)
			{
				mValue *= value;
				return operator T();
			}
			T operator/=(T value)
			{
				mValue /= value;
				return operator T();
			}
			T operator|=(T value)
			{
				mValue |= value;
				return operator T();
			}
			T operator^=(T value)
			{
				mValue ^= value;
				return operator T();
			}
			T operator&=(T value)
			{
				mValue &= value;
				return operator T();
			}


			//@REFACTOR: use expand macro

			virtual bool setValue(bool value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = value ? ::std::numeric_limits<T>::max() : 0;   this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel); return true; }
			virtual bool setValue(s8 value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT);  T tmpValue = (T)value;   this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(s16 value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = (T)value;   this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(s32 value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT);     T tmpValue = (T)value;   this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(s64 value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = (T)value;   this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(u8 value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = (T)value;   this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(u16 value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = (T)value;   this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(u32 value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = (T)value;   this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(u64 value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = (T)value;   this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(float value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = (T)value;  this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel); return true; }		
			virtual bool setValue(double value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = (T)value;   this->mValue = tmpValue;  DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(const char* value, CoreModifiable* owner) override { std::string localstr(value); return setValue(localstr, owner); }
			virtual bool setValue(const usString& value, CoreModifiable* owner) override { std::string localstr(value); return setValue(localstr, owner); }
			virtual bool setValue(const std::string& value, CoreModifiable* owner) override
				{
			RETURN_ON_READONLY(isReadOnlyT);
			if (CoreConvertString2Value<T>(value, this->mValue)) {
					T tmpValue = this->mValue;  this->mValue = tmpValue;
					DO_NOTIFICATION(notificationLevel); return true;
				}
				return false;
			}

			virtual bool getValue(bool& value, const CoreModifiable* owner) const override { T tmpValue = this->mValue;  value = (tmpValue != (T)0); return true; }
			virtual bool getValue(s8& value, const CoreModifiable* owner) const override { T tmpValue = this->mValue;  value = (s8)tmpValue; return true; }
			virtual bool getValue(s16& value, const CoreModifiable* owner) const override { T tmpValue = this->mValue;  value = (s16)tmpValue; return true; }
			virtual bool getValue(s32& value, const CoreModifiable* owner) const override { T tmpValue = this->mValue;  value = (s32)tmpValue; return true; }
			virtual bool getValue(s64& value, const CoreModifiable* owner) const override { T tmpValue = this->mValue;  value = (s64)tmpValue; return true; }
			virtual bool getValue(u8& value, const CoreModifiable* owner) const override { T tmpValue = this->mValue;  value = (u8)tmpValue; return true; }
			virtual bool getValue(u16& value, const CoreModifiable* owner) const override { T tmpValue = this->mValue;  value = (u16)tmpValue; return true; }
			virtual bool getValue(u32& value, const CoreModifiable* owner) const override { T tmpValue = this->mValue;  value = (u32)tmpValue; return true; }
			virtual bool getValue(u64& value, const CoreModifiable* owner) const override { T tmpValue = this->mValue;  value = (u64)tmpValue; return true; }
			virtual bool getValue(float& value, const CoreModifiable* owner) const override { T tmpValue = this->mValue;   value = (float)tmpValue; return true; }
			virtual bool getValue(double& value, const CoreModifiable* owner) const override { T tmpValue = this->mValue;   value = (double)tmpValue; return true; }
			virtual bool getValue(std::string& value, const CoreModifiable* owner) const override
			{
				T tmpValue = this->mValue;

				return CoreConvertValue2String<T>(value, tmpValue);
			}
			virtual bool getValue(usString& value, const CoreModifiable* owner) const override
			{
				std::string tmp;
				T tmpValue = this->mValue;

				if (CoreConvertValue2String<T>(tmp, tmpValue))
				{
					value = tmp; return true;
				}
				return false;
			}

		};

		template<typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType = TypeToEnum<T>::value>
		using maNumeric = maNumericHeritage<false, T, attributeType,false,false,false,false>;

		template<typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType = TypeToEnum<T>::value>
		using maNumericOrphan = maNumericHeritage<false, T, attributeType, false, false, false, true>;

		template<typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType = TypeToEnum<T>::value>
		using maNumericInit = maNumericHeritage<false, T, attributeType, true>;

		template<typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType = TypeToEnum<T>::value>
		using maNumericDynamic = maNumericHeritage<false, T, attributeType, false,false,true>;

		using maChar = maNumeric<s8>;
		using maCharOrphan = maNumericOrphan<s8>;
		using maCharInit = maNumericInit<s8>;
		using maCharDynamic = maNumericDynamic<s8>;
		using maShort = maNumeric<s16>;
		using maShortOrphan = maNumericOrphan<s16>;
		using maShortInit = maNumericInit<s16>;
		using maShortDynamic = maNumericDynamic<s16>;
		using maInt = maNumeric<s32>;
		using maIntOrphan = maNumericOrphan<s32>;
		using maIntInit = maNumericInit<s32>;
		using maIntDynamic = maNumericDynamic<s32>;
		using maLong = maNumeric<s64>;
		using maLongOrphan = maNumericOrphan<s64>;
		using maLongInit = maNumericInit<s64>;
		using maLongDynamic = maNumericDynamic<s64>;
		using maUChar = maNumeric<u8>;
		using maUCharOrphan = maNumericOrphan<u8>;
		using maUCharInit = maNumericInit<u8>;
		using maUCharDynamic = maNumericDynamic<u8>;
		using maUShort = maNumeric<u16>;
		using maUShortOrphan = maNumericOrphan<u16>;
		using maUShortInit = maNumericInit<u16>;
		using maUShortDynamic = maNumericDynamic<u16>;
		using maUInt = maNumeric<u32>;
		using maUIntOrphan = maNumericOrphan<u32>;
		using maUIntInit = maNumericInit<u32>;
		using maUIntDynamic = maNumericDynamic<u32>;
		using maULong = maNumeric<u64>;
		using maULongOrphan = maNumericOrphan<u64>;
		using maULongInit = maNumericInit<u64>;
		using maULongDynamic = maNumericDynamic<u64>;
		using maFloat = maNumeric<float>;
		using maFloatOrphan = maNumericOrphan<float>;
		using maFloatInit = maNumericInit<float>;
		using maFloatDynamic = maNumericDynamic<float>;
		using maDouble = maNumeric<double>;
		using maDoubleOrphan = maNumericOrphan<double>;
		using maDoubleInit = maNumericInit<double>;
		using maDoubleDynamic = maNumericDynamic<double>;

		// ****************************************
		// * maComputedNumericHeritage class
		// * --------------------------------------
		/**
		* \class	maComputedNumericHeritage
		* \ingroup CoreModifiableAttibute
		* \brief	numeric calling onwer get / set method to compute value
		*/
		// ****************************************
		template<bool notificationLevel, typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType, bool isInitT = false, bool isReadOnlyT = false, bool isDynamicT = false, bool isOrphanT = false>
		class maComputedNumericHeritage : public CoreModifiableAttributeTemplated<notificationLevel, isInitT, isReadOnlyT, isDynamicT, isOrphanT>
		{
		template<bool notiflevel, bool isInitTe, bool isReadOnlyTe, bool isDynamicTe, bool isOrphanTe>
		using TemplateForPlacementNew = maComputedNumericHeritage<notiflevel, T, attributeType, isInitTe, isReadOnlyTe, isDynamicTe, isOrphanTe>;


		private:

		maComputedNumericHeritage(CoreModifiable& owner, KigsID ID, const T& value) : CoreModifiableAttributeTemplated<notificationLevel, isInitT, isReadOnlyT, isDynamicT, isOrphanT>(&owner, ID)
			{

			}
		maComputedNumericHeritage(CoreModifiable& owner, KigsID ID) : CoreModifiableAttributeTemplated<notificationLevel, isInitT, isReadOnlyT, isDynamicT, isOrphanT>(&owner, ID)
			{

			}
		maComputedNumericHeritage(KigsID ID, const T& value) : CoreModifiableAttributeTemplated<notificationLevel, isInitT, isReadOnlyT, isDynamicT, isOrphanT>(nullptr, false, ID)
			{
			}

		public:

		explicit maComputedNumericHeritage(InheritanceSwitch tag) : CoreModifiableAttributeTemplated<notificationLevel, isInitT, isReadOnlyT, isDynamicT, isOrphanT>(tag) {}

		maComputedNumericHeritage(CoreModifiable& owner, KigsID ID, KigsID g, KigsID s) : CoreModifiableAttributeTemplated<notificationLevel, isInitT, isReadOnlyT, isDynamicT, isOrphanT>(&owner, ID), mGetter(g), mSetter(s)
			{

			}

			typedef maComputedNumericHeritage CurrentAttributeClass;
			typedef T CurrentAttributeType;
			static constexpr CoreModifiable::ATTRIBUTE_TYPE type = attributeType;
			CoreModifiable::ATTRIBUTE_TYPE getType() const override { return attributeType; }

		auto& operator=(const TemplateForPlacementNew<notificationLevel, isInitT, isReadOnlyT, isDynamicT, isOrphanT>& attribute)
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
			case 0: new (this) TemplateForPlacementNew<false, false, false, false, false>(InheritanceSwitch{}); break;
			case 1: new (this) TemplateForPlacementNew<true, false, false, false, false>(InheritanceSwitch{}); break;
			case 2: new (this) TemplateForPlacementNew<false, true, false, false, false>(InheritanceSwitch{}); break;
			case 3: new (this) TemplateForPlacementNew<true, true, false, false, false>(InheritanceSwitch{}); break;
			case 4: new (this) TemplateForPlacementNew<false, false, true, false, false>(InheritanceSwitch{}); break;
			case 5: new (this) TemplateForPlacementNew<true, false, true, false, false>(InheritanceSwitch{}); break;
			case 6: new (this) TemplateForPlacementNew<false, true, true, false, false>(InheritanceSwitch{}); break;
			case 7: new (this) TemplateForPlacementNew<true, true, true, false, false>(InheritanceSwitch{}); break;
			case 8: new (this) TemplateForPlacementNew<false, false, false, true, false>(InheritanceSwitch{}); break;
			case 9: new (this) TemplateForPlacementNew<true, false, false, true, false>(InheritanceSwitch{}); break;
			case 12: new (this) TemplateForPlacementNew<false, false, true, true, false>(InheritanceSwitch{}); break;
			case 13: new (this) TemplateForPlacementNew<true, false, true, true, false>(InheritanceSwitch{}); break;
			case 16: new (this) TemplateForPlacementNew<false, false, false, false, true>(InheritanceSwitch{}); break;
			case 20: new (this) TemplateForPlacementNew<false, false, true, false, true>(InheritanceSwitch{}); break;
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
		// TODO ?
		/*virtual operator CurrentAttributeType() const
			{
				CurrentAttributeType tmpValue = getOwner()->template SimpleCall<T>(mGetter);


				return tmpValue;
			};
			auto& operator=(const CurrentAttributeType& value)
			{
			if (mSetter != "") getOwner()->SimpleCall(mSetter,value);
				return *this;
			}
		*/
		public:

		virtual void* getRawValue(CoreModifiable* owner) final { return nullptr; }
			bool CopyAttribute(const CoreModifiableAttribute& other) override
			{

				return false;

			}


		virtual bool setValue(bool value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = value ? std::numeric_limits<T>::max() : 0;   if (mSetter != "")owner->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel); return true; }
		virtual bool setValue(s8 value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT);  T tmpValue = (T)value;   if (mSetter != "")owner->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
		virtual bool setValue(s16 value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = (T)value;   if (mSetter != "")owner->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
		virtual bool setValue(s32 value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT);   T tmpValue = (T)value;   if (mSetter != "")owner->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
		virtual bool setValue(s64 value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = (T)value;   if (mSetter != "") owner->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
		virtual bool setValue(u8 value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = (T)value;   if (mSetter != "")owner->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
		virtual bool setValue(u16 value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = (T)value;   if (mSetter != "")owner->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
		virtual bool setValue(u32 value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = (T)value;   if (mSetter != "")owner->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
		virtual bool setValue(u64 value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = (T)value;   if (mSetter != "")owner->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(float value, CoreModifiable* owner) override
			{
			RETURN_ON_READONLY(isReadOnlyT);
				T tmpValue = (T)value;
			if (mSetter != "")owner->SimpleCall(mSetter, tmpValue);
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
		virtual bool setValue(double value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); T tmpValue = (T)value;   owner->SimpleCall(mSetter, tmpValue);  DO_NOTIFICATION(notificationLevel);  return true; }
		virtual bool setValue(const char* value, CoreModifiable* owner) override { std::string localstr(value); return setValue(localstr, owner); }
		virtual bool setValue(const usString& value, CoreModifiable* owner) override { std::string localstr(value); return setValue(localstr, owner); }
		virtual bool setValue(const std::string& value, CoreModifiable* owner) override
			{
			RETURN_ON_READONLY(isReadOnlyT);

				T tmpValue;
				if (CoreConvertString2Value<T>(value, tmpValue))
				{
					if (mSetter != "") owner->SimpleCall(mSetter, tmpValue);
					DO_NOTIFICATION(notificationLevel); return true;
				}
				return false;
			}

		virtual bool getValue(bool& value, const CoreModifiable* owner) const override { T tmpValue = const_cast<CoreModifiable*>(owner)->template SimpleCall<T>(mGetter);   value = (tmpValue != (T)0); return true; }
		virtual bool getValue(s8& value, const CoreModifiable* owner) const override { T tmpValue = const_cast<CoreModifiable*>(owner)->template SimpleCall<T>(mGetter);   value = (s8)tmpValue; return true; }
		virtual bool getValue(s16& value, const CoreModifiable* owner) const override { T tmpValue = const_cast<CoreModifiable*>(owner)->template SimpleCall<T>(mGetter);   value = (s16)tmpValue; return true; }
		virtual bool getValue(s32& value, const CoreModifiable* owner) const override { T tmpValue = const_cast<CoreModifiable*>(owner)->template SimpleCall<T>(mGetter);   value = (s32)tmpValue; return true; }
		virtual bool getValue(s64& value, const CoreModifiable* owner) const override { T tmpValue = const_cast<CoreModifiable*>(owner)->template SimpleCall<T>(mGetter);   value = (s64)tmpValue; return true; }
		virtual bool getValue(u8& value, const CoreModifiable* owner) const override { T tmpValue = const_cast<CoreModifiable*>(owner)->template SimpleCall<T>(mGetter);   value = (u8)tmpValue; return true; }
		virtual bool getValue(u16& value, const CoreModifiable* owner) const override { T tmpValue = const_cast<CoreModifiable*>(owner)->template SimpleCall<T>(mGetter);   value = (u16)tmpValue; return true; }
		virtual bool getValue(u32& value, const CoreModifiable* owner) const override { T tmpValue = const_cast<CoreModifiable*>(owner)->template SimpleCall<T>(mGetter);   value = (u32)tmpValue; return true; }
		virtual bool getValue(u64& value, const CoreModifiable* owner) const override { T tmpValue = const_cast<CoreModifiable*>(owner)->template SimpleCall<T>(mGetter);   value = (u64)tmpValue; return true; }
		virtual bool getValue(float& value, const CoreModifiable* owner) const override { T tmpValue = const_cast<CoreModifiable*>(owner)->template SimpleCall<T>(mGetter);    value = (float)tmpValue; return true; }
		virtual bool getValue(double& value, const CoreModifiable* owner) const override { T tmpValue = const_cast<CoreModifiable*>(owner)->template SimpleCall<T>(mGetter);    value = (double)tmpValue; return true; }
		virtual bool getValue(std::string& value, const CoreModifiable* owner) const override
			{
			T tmpValue = const_cast<CoreModifiable*>(owner)->template SimpleCall<T>(mGetter);

				return CoreConvertValue2String<T>(value, tmpValue);
			}
		virtual bool getValue(usString& value, const CoreModifiable* owner) const override
			{
			std::string tmp;
			
			T tmpValue = const_cast<CoreModifiable*>(owner)->template SimpleCall<T>(mGetter);

				if (CoreConvertValue2String<T>(tmp, tmpValue))
				{
					value = tmp; return true;
				}
				return false;
			}

		};

		template<typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType = TypeToEnum<T>::value>
		using maComputedNumeric = maComputedNumericHeritage<false, T, attributeType,false,false,false,false>;

		template<typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType = TypeToEnum<T>::value>
		using maComputedNumericInit = maComputedNumericHeritage<false, T, attributeType, true>;
	}
}