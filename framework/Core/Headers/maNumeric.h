#pragma once

#include "CoreModifiableAttribute.h"
#include "AttributeModifier.h"



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
	
	virtual bool setValue(bool value) override { if (this->isReadOnly()) { return false; } T tmpValue = value ? std::numeric_limits<T>::max() : 0; CALL_SETMODIFIER(notificationLevel, tmpValue); this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel); return true; }
	virtual bool setValue(s8 value) override { if (this->isReadOnly()) { return false; }  T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(s16 value) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(s32 value) override { if (this->isReadOnly()) { return false; }     T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(s64 value) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(u8 value) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(u16 value) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(u32 value) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(u64 value) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); this->mValue = tmpValue; DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(kfloat value) override 
	{ 
		if (this->isReadOnly()) 
		{ 
			return false; 
		} 
		T tmpValue = (T)value; 
		CALL_SETMODIFIER(notificationLevel, tmpValue); 
		this->mValue = tmpValue; 
		DO_NOTIFICATION(notificationLevel);  
		return true; 
	}
	virtual bool setValue(kdouble value) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); this->mValue = tmpValue;  DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(const char* value) override { kstl::string localstr(value); return setValue(localstr); }
	virtual bool setValue(const usString& value) override { kstl::string localstr(value); return setValue(localstr); }
	virtual bool setValue(const kstl::string& value) override
	{
		if (this->isReadOnly()) { return false; }

		if (CoreConvertString2Value<T>(value, this->mValue))
		{
			T tmpValue = this->mValue; CALL_SETMODIFIER(notificationLevel, tmpValue); this->mValue = tmpValue;
			DO_NOTIFICATION(notificationLevel); return true;
		}
		return false;
	}

	virtual bool getValue(bool& value) const override { T tmpValue = this->mValue; CALL_GETMODIFIER(notificationLevel, tmpValue); value = (tmpValue != (T)0); return true; }
	virtual bool getValue(s8& value) const override { T tmpValue = this->mValue; CALL_GETMODIFIER(notificationLevel, tmpValue); value = (s8)tmpValue; return true; }
	virtual bool getValue(s16& value) const override { T tmpValue = this->mValue; CALL_GETMODIFIER(notificationLevel, tmpValue); value = (s16)tmpValue; return true; }
	virtual bool getValue(s32& value) const override { T tmpValue = this->mValue; CALL_GETMODIFIER(notificationLevel, tmpValue); value = (s32)tmpValue; return true; }
	virtual bool getValue(s64& value) const override { T tmpValue = this->mValue; CALL_GETMODIFIER(notificationLevel, tmpValue); value = (s64)tmpValue; return true; }
	virtual bool getValue(u8& value) const override { T tmpValue = this->mValue; CALL_GETMODIFIER(notificationLevel, tmpValue); value = (u8)tmpValue; return true; }
	virtual bool getValue(u16& value) const override { T tmpValue = this->mValue; CALL_GETMODIFIER(notificationLevel, tmpValue); value = (u16)tmpValue; return true; }
	virtual bool getValue(u32& value) const override { T tmpValue = this->mValue; CALL_GETMODIFIER(notificationLevel, tmpValue); value = (u32)tmpValue; return true; }
	virtual bool getValue(u64& value) const override { T tmpValue = this->mValue; CALL_GETMODIFIER(notificationLevel, tmpValue); value = (u64)tmpValue; return true; }
	virtual bool getValue(kfloat& value) const override { T tmpValue = this->mValue; CALL_GETMODIFIER(notificationLevel, tmpValue);  value = (kfloat)tmpValue; return true; }
	virtual bool getValue(kdouble& value) const override { T tmpValue = this->mValue; CALL_GETMODIFIER(notificationLevel, tmpValue);  value = (kdouble)tmpValue; return true; }
	virtual bool getValue(kstl::string& value) const override
	{
		T tmpValue = this->mValue;
		CALL_GETMODIFIER(notificationLevel, tmpValue);
		return CoreConvertValue2String<T>(value, tmpValue);
	}
	virtual bool getValue(usString& value) const override
	{
		kstl::string tmp;
		T tmpValue = this->mValue;
		CALL_GETMODIFIER(notificationLevel, tmpValue);
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
		AttachedModifierBase* modifier = mAttachedModifier;
		u32 old_flags = mFlags;
		u32 inheritlevel = (mFlags >> INHERIT_LEVEL_SHIFT)& INHERIT_LEVEL_MOD;
		doPlacementNew(inheritlevel);
		mID = old_id;
		mGetter = old_getter;
		mSetter = old_setter;

		mFlags = old_flags;
		mAttachedModifier = modifier;
	}

public:
	virtual operator CurrentAttributeType() const
	{
		CurrentAttributeType tmpValue = mOwner->template SimpleCall<T>(mGetter);
		
		CALL_GETMODIFIER(notificationLevel, tmpValue); 
		return tmpValue;
	};
	auto& operator=(const CurrentAttributeType& value)
	{
		if (mSetter != "") mOwner->SimpleCall(mSetter,value);
		return *this;
	}

public:

	virtual void* getRawValue() final { return nullptr; }
	bool CopyAttribute(const CoreModifiableAttribute& other) override
	{

		return false;

	}


	virtual bool setValue(bool value) override { if (this->isReadOnly()) { return false; } T tmpValue = value ? std::numeric_limits<T>::max() : 0; CALL_SETMODIFIER(notificationLevel, tmpValue); if (mSetter != "") this->mOwner->SimpleCall(mSetter,tmpValue); DO_NOTIFICATION(notificationLevel); return true; }
	virtual bool setValue(s8 value) override { if (this->isReadOnly()) { return false; }  T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); if (mSetter != "") this->mOwner->SimpleCall(mSetter,tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(s16 value) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); if (mSetter != "") this->mOwner->SimpleCall(mSetter,tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(s32 value) override { if (this->isReadOnly()) { return false; }     T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); if (mSetter != "") this->mOwner->SimpleCall(mSetter,tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(s64 value) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); if (mSetter != "")  this->mOwner->SimpleCall(mSetter,tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(u8 value) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); if (mSetter != "") this->mOwner->SimpleCall(mSetter,tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(u16 value) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); if (mSetter != "") this->mOwner->SimpleCall(mSetter,tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(u32 value) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); if (mSetter != "") this->mOwner->SimpleCall(mSetter,tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(u64 value) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); if (mSetter != "") this->mOwner->SimpleCall(mSetter,tmpValue); DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(kfloat value) override
	{
		if (this->isReadOnly())
		{
			return false;
		}
		T tmpValue = (T)value;
		CALL_SETMODIFIER(notificationLevel, tmpValue);
		if (mSetter != "") this->mOwner->SimpleCall(mSetter,tmpValue);
		DO_NOTIFICATION(notificationLevel);
		return true;
	}
	virtual bool setValue(kdouble value) override { if (this->isReadOnly()) { return false; } T tmpValue = (T)value; CALL_SETMODIFIER(notificationLevel, tmpValue); this->mOwner->SimpleCall(mSetter,tmpValue);  DO_NOTIFICATION(notificationLevel);  return true; }
	virtual bool setValue(const char* value) override { kstl::string localstr(value); return setValue(localstr); }
	virtual bool setValue(const usString& value) override { kstl::string localstr(value); return setValue(localstr); }
	virtual bool setValue(const kstl::string& value) override
	{
		if (this->isReadOnly()) { return false; }

		T tmpValue;
		if (CoreConvertString2Value<T>(value, tmpValue))
		{
			CALL_SETMODIFIER(notificationLevel, tmpValue); if(mSetter != "") this->mOwner->SimpleCall(mSetter,tmpValue);
			DO_NOTIFICATION(notificationLevel); return true;
		}
		return false;
	}

	virtual bool getValue(bool& value) const override { T tmpValue = this->mOwner->template SimpleCall<T>(mGetter); CALL_GETMODIFIER(notificationLevel, tmpValue); value = (tmpValue != (T)0); return true; }
	virtual bool getValue(s8& value) const override { T tmpValue = this->mOwner->template SimpleCall<T>(mGetter); CALL_GETMODIFIER(notificationLevel, tmpValue); value = (s8)tmpValue; return true; }
	virtual bool getValue(s16& value) const override { T tmpValue = this->mOwner->template SimpleCall<T>(mGetter); CALL_GETMODIFIER(notificationLevel, tmpValue); value = (s16)tmpValue; return true; }
	virtual bool getValue(s32& value) const override { T tmpValue = this->mOwner->template SimpleCall<T>(mGetter); CALL_GETMODIFIER(notificationLevel, tmpValue); value = (s32)tmpValue; return true; }
	virtual bool getValue(s64& value) const override { T tmpValue = this->mOwner->template SimpleCall<T>(mGetter); CALL_GETMODIFIER(notificationLevel, tmpValue); value = (s64)tmpValue; return true; }
	virtual bool getValue(u8& value) const override { T tmpValue = this->mOwner->template SimpleCall<T>(mGetter); CALL_GETMODIFIER(notificationLevel, tmpValue); value = (u8)tmpValue; return true; }
	virtual bool getValue(u16& value) const override { T tmpValue = this->mOwner->template SimpleCall<T>(mGetter); CALL_GETMODIFIER(notificationLevel, tmpValue); value = (u16)tmpValue; return true; }
	virtual bool getValue(u32& value) const override { T tmpValue = this->mOwner->template SimpleCall<T>(mGetter); CALL_GETMODIFIER(notificationLevel, tmpValue); value = (u32)tmpValue; return true; }
	virtual bool getValue(u64& value) const override { T tmpValue = this->mOwner->template SimpleCall<T>(mGetter); CALL_GETMODIFIER(notificationLevel, tmpValue); value = (u64)tmpValue; return true; }
	virtual bool getValue(kfloat& value) const override { T tmpValue = this->mOwner->template SimpleCall<T>(mGetter); CALL_GETMODIFIER(notificationLevel, tmpValue);  value = (kfloat)tmpValue; return true; }
	virtual bool getValue(kdouble& value) const override { T tmpValue = this->mOwner->template SimpleCall<T>(mGetter); CALL_GETMODIFIER(notificationLevel, tmpValue);  value = (kdouble)tmpValue; return true; }
	virtual bool getValue(kstl::string& value) const override
	{
		T tmpValue = this->mOwner->template SimpleCall<T>(mGetter);
		CALL_GETMODIFIER(notificationLevel, tmpValue);
		return CoreConvertValue2String<T>(value, tmpValue);
	}
	virtual bool getValue(usString& value) const override
	{
		kstl::string tmp;
		T tmpValue = this->mOwner->template SimpleCall<T>(mGetter);
		CALL_GETMODIFIER(notificationLevel, tmpValue);
		if (CoreConvertValue2String<T>(tmp, tmpValue))
		{
			value = tmp; return true;
		}
		return false;
	}

};

template<typename T, CoreModifiable::ATTRIBUTE_TYPE attributeType = TypeToEnum<T>::value>
using maComputedNumeric = maComputedNumericHeritage<0, T, attributeType>;