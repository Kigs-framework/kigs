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
		template<s32 notificationLevel, typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType> class maNumericHeritage : public CoreModifiableAttributeData<T>
		{
			template<s32 notiflevel>
			using TemplateForPlacementNew = maNumericHeritage<notiflevel, T, attributeType>;

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

			virtual bool setValue(bool value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; } T tmpValue = value ? std::numeric_limits<T>::max() : 0;  this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel); return true; }
			virtual bool setValue(s8 value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; }  T tmpValue = (T)value;  this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(s16 value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value;  this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(s32 value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; }     T tmpValue = (T)value;  this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(s64 value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value;  this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(u8 value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value;  this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(u16 value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value;  this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(u32 value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value;  this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(u64 value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value;  this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(float value, CoreModifiable* owner) override
			{
				if (this->isReadOnly())
				{
					return false;
				}
				T tmpValue = (T)value;

				this->mValue = tmpValue;
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
			virtual bool setValue(double value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value;  this->mValue = tmpValue;  DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(const char* value, CoreModifiable* owner) override { std::string localstr(value); return setValue(localstr,owner); }
			virtual bool setValue(const usString& value, CoreModifiable* owner) override { std::string localstr(value); return setValue(localstr, owner); }
			virtual bool setValue(const std::string& value, CoreModifiable* owner) override
			{
				if (this->isReadOnly()) { return false; }

				if (CoreConvertString2Value<T>(value, this->mValue))
				{
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
		using maNumeric = maNumericHeritage<0, T, attributeType>;


		using maChar = maNumeric<s8>;
		using maShort = maNumeric<s16>;
		using maInt = maNumeric<s32>;
		using maLong = maNumeric<s64>;
		using maUChar = maNumeric<u8>;
		using maUShort = maNumeric<u16>;
		using maUInt = maNumeric<u32>;
		using maULong = maNumeric<u64>;
		using maFloat = maNumeric<float>;
		using maDouble = maNumeric<double>;

		// ****************************************
		// * maComputedNumericHeritage class
		// * --------------------------------------
		/**
		* \class	maComputedNumericHeritage
		* \ingroup CoreModifiableAttibute
		* \brief	numeric calling onwer get / set method to compute value
		*/
		// ****************************************
		template<s32 notificationLevel, typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType> class maComputedNumericHeritage : public CoreModifiableAttribute
		{
			template<s32 notiflevel>
			using TemplateForPlacementNew = maComputedNumericHeritage<notiflevel, T, attributeType>;



		private:

			maComputedNumericHeritage(CoreModifiable& owner, bool isInitParam, KigsID ID, const T& value) : CoreModifiableAttribute(&owner, isInitParam, ID)
			{

			}
			maComputedNumericHeritage(CoreModifiable& owner, bool isInitParam, KigsID ID) : CoreModifiableAttribute(&owner, isInitParam, ID)
			{

			}
			maComputedNumericHeritage(KigsID ID, const T& value) : CoreModifiableAttribute(nullptr, false, ID)
			{
			}

		public:

			explicit maComputedNumericHeritage(InheritanceSwitch tag) : CoreModifiableAttribute(tag) {}

			maComputedNumericHeritage(CoreModifiable& owner, bool isInitParam, KigsID ID, KigsID g, KigsID s) : CoreModifiableAttribute(&owner, isInitParam, ID), mGetter(g), mSetter(s)
			{

			}

			typedef maComputedNumericHeritage CurrentAttributeClass;
			typedef T CurrentAttributeType;
			static constexpr CoreModifiable::ATTRIBUTE_TYPE type = attributeType;
			CoreModifiable::ATTRIBUTE_TYPE getType() const override { return attributeType; }

			auto& operator=(const TemplateForPlacementNew<notificationLevel>& attribute)
			{
				this->CopyData(attribute);
				return *this;
			}
		protected:

			KigsID	mGetter;
			KigsID	mSetter;

			void doPlacementNew(u32 level) override
			{
				switch (level)
				{
				case 0: new (this) TemplateForPlacementNew<0>(InheritanceSwitch{}); break; \
				case 1: new (this) TemplateForPlacementNew<1>(InheritanceSwitch{}); break; \
				case 2: new (this) TemplateForPlacementNew<2>(InheritanceSwitch{}); break; \
				case 3: new (this) TemplateForPlacementNew<3>(InheritanceSwitch{}); break; \
				default: assert(false); break;
				}
			}
			virtual void changeInheritance() final
			{
				KigsID old_id = mID;
				mID.~KigsID();
				KigsID old_getter = mGetter;
				mGetter.~KigsID();
				KigsID old_setter = mSetter;
				mSetter.~KigsID();
				auto owner = mOwner;
				u32 old_flags = mFlags;
				u32 inheritlevel = (mFlags >> INHERIT_LEVEL_SHIFT) & INHERIT_LEVEL_MOD;
				doPlacementNew(inheritlevel);
				mID = old_id;
				mGetter = old_getter;
				mSetter = old_setter;
				mFlags = old_flags;
				mOwner = owner;
			}

		public:
			virtual operator CurrentAttributeType() const
			{
				CurrentAttributeType tmpValue = getOwner()->template SimpleCall<T>(mGetter);
				return tmpValue;
			};
			auto& operator=(const CurrentAttributeType& value)
			{
				if (mSetter != "") getOwner()->SimpleCall(mSetter, value);
				return *this;
			}

		public:

			virtual void* getRawValue() final { return nullptr; }
			bool CopyAttribute(const CoreModifiableAttribute& other) override
			{

				return false;

			}


			virtual bool setValue(bool value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; } T tmpValue = value ? std::numeric_limits<T>::max() : 0;  if (mSetter != "") this->getOwner()->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel); return true; }
			virtual bool setValue(s8 value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; }  T tmpValue = (T)value;  if (mSetter != "") this->getOwner()->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(s16 value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value;  if (mSetter != "") this->getOwner()->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(s32 value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; }     T tmpValue = (T)value;  if (mSetter != "") this->getOwner()->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(s64 value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value;  if (mSetter != "")  this->getOwner()->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(u8 value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value;  if (mSetter != "") this->getOwner()->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(u16 value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value;  if (mSetter != "") this->getOwner()->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(u32 value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value;  if (mSetter != "") this->getOwner()->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(u64 value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value;  if (mSetter != "") this->getOwner()->SimpleCall(mSetter, tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(float value, CoreModifiable* owner) override
			{
				if (this->isReadOnly())
				{
					return false;
				}
				T tmpValue = (T)value;

				if (mSetter != "") this->getOwner()->SimpleCall(mSetter, tmpValue);
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
			virtual bool setValue(double value, CoreModifiable* owner) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value;  this->getOwner()->SimpleCall(mSetter, tmpValue);  DO_NOTIFICATION(notificationLevel);  return true; }
			virtual bool setValue(const char* value, CoreModifiable* owner) override { std::string localstr(value); return setValue(localstr,owner); }
			virtual bool setValue(const usString& value, CoreModifiable* owner) override { std::string localstr(value); return setValue(localstr,owner); }
			virtual bool setValue(const std::string& value, CoreModifiable* owner) override
			{
				if (this->isReadOnly()) { return false; }

				T tmpValue;
				if (CoreConvertString2Value<T>(value, tmpValue))
				{
					if (mSetter != "") this->getOwner()->SimpleCall(mSetter, tmpValue);
					DO_NOTIFICATION(notificationLevel); return true;
				}
				return false;
			}

			virtual bool getValue(bool& value, const CoreModifiable* owner) const override { T tmpValue = this->getOwner()->template SimpleCall<T>(mGetter);  value = (tmpValue != (T)0); return true; }
			virtual bool getValue(s8& value, const CoreModifiable* owner) const override { T tmpValue = this->getOwner()->template SimpleCall<T>(mGetter);  value = (s8)tmpValue; return true; }
			virtual bool getValue(s16& value, const CoreModifiable* owner) const override { T tmpValue = this->getOwner()->template SimpleCall<T>(mGetter);  value = (s16)tmpValue; return true; }
			virtual bool getValue(s32& value, const CoreModifiable* owner) const override { T tmpValue = this->getOwner()->template SimpleCall<T>(mGetter);  value = (s32)tmpValue; return true; }
			virtual bool getValue(s64& value, const CoreModifiable* owner) const override { T tmpValue = this->getOwner()->template SimpleCall<T>(mGetter);  value = (s64)tmpValue; return true; }
			virtual bool getValue(u8& value, const CoreModifiable* owner) const override { T tmpValue = this->getOwner()->template SimpleCall<T>(mGetter);  value = (u8)tmpValue; return true; }
			virtual bool getValue(u16& value, const CoreModifiable* owner) const override { T tmpValue = this->getOwner()->template SimpleCall<T>(mGetter);  value = (u16)tmpValue; return true; }
			virtual bool getValue(u32& value, const CoreModifiable* owner) const override { T tmpValue = this->getOwner()->template SimpleCall<T>(mGetter);  value = (u32)tmpValue; return true; }
			virtual bool getValue(u64& value, const CoreModifiable* owner) const override { T tmpValue = this->getOwner()->template SimpleCall<T>(mGetter);  value = (u64)tmpValue; return true; }
			virtual bool getValue(float& value, const CoreModifiable* owner) const override { T tmpValue = this->getOwner()->template SimpleCall<T>(mGetter);   value = (float)tmpValue; return true; }
			virtual bool getValue(double& value, const CoreModifiable* owner) const override { T tmpValue = this->getOwner()->template SimpleCall<T>(mGetter);   value = (double)tmpValue; return true; }
			virtual bool getValue(std::string& value, const CoreModifiable* owner) const override
			{
				T tmpValue = this->getOwner()->template SimpleCall<T>(mGetter);

				return CoreConvertValue2String<T>(value, tmpValue);
			}
			virtual bool getValue(usString& value, const CoreModifiable* owner) const override
			{
				std::string tmp;
				T tmpValue = this->getOwner()->template SimpleCall<T>(mGetter);

				if (CoreConvertValue2String<T>(tmp, tmpValue))
				{
					value = tmp; return true;
				}
				return false;
			}

		};

		template<typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType = TypeToEnum<T>::value>
		using maComputedNumeric = maComputedNumericHeritage<0, T, attributeType>;

	}
}