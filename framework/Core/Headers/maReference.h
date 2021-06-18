#ifndef _MAREFERENCE_H
#define _MAREFERENCE_H

#include "CoreModifiableAttribute.h"
#include "AttributeModifier.h"



struct maWeakReferenceObject
{
	maWeakReferenceObject() = default;

	maWeakReferenceObject(CoreModifiable* lobj)
	{
		if (lobj)
			mObj = lobj->SharedFromThis();
	}
	maWeakReferenceObject(const kstl::string& nametype)
	{
		mSearchString = nametype;
	}
	std::weak_ptr<CoreModifiable> mObj;
	kstl::string mSearchString;
};

struct maStrongReferenceObject
{
	maStrongReferenceObject() = default;
	maStrongReferenceObject(GenericRefCountedBaseClass* lobj)
	{
		if(lobj)
			mObj = lobj->shared_from_this();
	}
	maStrongReferenceObject(SP<GenericRefCountedBaseClass> lobj)
	{
		mObj = lobj;
	}
	maStrongReferenceObject(const kstl::string& nametype)
	{
		mSearchString = nametype;
	}
	SP<GenericRefCountedBaseClass> mObj;
	kstl::string mSearchString;
};

// ****************************************
// * maReferenceHeritage class
// * --------------------------------------
/**
* \class	maReferenceHeritage
* \ingroup CoreModifiableAttibute
* \brief	CoreModifiableAttributeData for reference on CoreModifiable
*/
// ****************************************

template<int notificationLevel>
class maReferenceHeritage : public CoreModifiableAttributeData<maWeakReferenceObject>
{
	DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maReferenceHeritage, maReferenceHeritage, maWeakReferenceObject, CoreModifiable::ATTRIBUTE_TYPE::WEAK_REFERENCE);


public:

	maReferenceHeritage(CoreModifiable& owner, bool isInitAttribute, KigsID ID, kstl::string value) : CoreModifiableAttributeData<maWeakReferenceObject>(owner, isInitAttribute, ID)
	{
		mValue = maWeakReferenceObject{ value };
		Search();
	}

	maReferenceHeritage() : CoreModifiableAttributeData<maWeakReferenceObject>(KigsID{0u}, maWeakReferenceObject{}) {}

	virtual ~maReferenceHeritage()
	{
	}

	// Sets the internal pointer to null, forcing to search again next time we query the reference
	void	ResetFoundModifiable()
	{
		mValue.mObj.reset();
	}
	virtual void CopyData(const CoreModifiableAttributeData<maWeakReferenceObject>& toCopy) override
	{
		const auto& toCopyValue = toCopy.const_ref();
		mValue = toCopyValue;
	}

	operator CMSP() { return SearchRef(); }

	template<typename T>
	operator SP<T>(){ return SearchRef(); }

	// NOTE(antoine) Thread unsafe, need to remove method
	operator CoreModifiable*() { return SearchRef().get(); }
	// NOTE(antoine) Thread unsafe, need to remove method
	template<typename T>
	operator T* () { return static_cast<T*>(SearchRef().get()); }

	std::string RefString() const
	{
		std::string result;
		getValue(result);
		return result;
	}

	CoreModifiable*	operator->()
	{
		return SearchRef().get();
	}

	// NOTE(antoine) null references ?????
	// Thread unsafe, need to remove method
	operator CoreModifiable&() { return (*SearchRef()); }
	CoreModifiable& ref() { return (*SearchRef()); }
	const CoreModifiable& const_ref() { return (*SearchRef()); }


	/// getValue overloads
	virtual bool getValue(kstl::string& value) const override
	{

		((maReferenceHeritage*)this)->SearchRef();
		if (auto ptr = mValue.mObj.lock())
		{
#ifdef KEEP_NAME_AS_STRING
			value = ptr->GetRuntimeType();
#else
			value = std::to_string(ptr->GetRuntimeType().toUInt());
#endif

			value += ":";
			value += ptr->getName();
			return true;
		}
		else
		{
			value = mValue.mSearchString;
			return true;
		}

		return false;
	}
	virtual bool getValue(usString& value) const override
	{
		// TODO ?
		return false;
	}

	// Thread unsafe, need to add getValue method with CMSP
	virtual bool getValue(CoreModifiable*&  value) const override
	{
		value = (CoreModifiable*) const_cast<maReferenceHeritage*>(this)->SearchRef().get();
		return true;
	}
	
	// Thread unsafe, this one is really bad...
	virtual bool getValue(void*& value) const override
	{
		value = (void*) const_cast<maReferenceHeritage*>(this)->SearchRef().get();
		return true;
	}

	/// setValue overloads
	virtual bool setValue(const char* value) override
	{
		if (this->isReadOnly())
			return false;

		InitAndSearch(value);
		DO_NOTIFICATION(notificationLevel);
		return true;
	}
	virtual bool setValue(const kstl::string& value) override
	{
		if (this->isReadOnly())
			return false;

		InitAndSearch(value);
		DO_NOTIFICATION(notificationLevel);
		return true;
	}
	virtual bool setValue(CoreModifiable* value) override
	{
		if (this->isReadOnly())
			return false;
		mValue.mObj = value ? value->SharedFromThis() : nullptr;
		DO_NOTIFICATION(notificationLevel);
		return true;
	}

	/// operators
	auto& operator=(const kstl::string &value)
	{
		InitAndSearch(value);
		DO_NOTIFICATION(notificationLevel);
		return *this;
	}
	auto& operator=(CoreModifiable* value)
	{
		setValue(value);
		return *this;
	}

protected:
	CMSP Search()
	{
		CMSP obj;
		if (!mValue.mSearchString.empty())
		{
			obj = CoreModifiable::SearchInstance(mValue.mSearchString, getOwner());
		}
		mValue.mObj = obj;
		return obj;
	}
	void InitAndSearch(const kstl::string& nametype)
	{
		auto old_obj = mValue.mObj;
		mValue = maWeakReferenceObject{ nametype };
		mValue.mObj = old_obj;
		Search();
	}
	CMSP SearchRef()
	{
		if (auto ptr = mValue.mObj.lock())
			return ptr;
		return Search();
	}
};

// ****************************************
// * maReference class
// * --------------------------------------
/**
* \class	maReference
* \ingroup CoreModifiableAttibute
* \brief	CoreModifiableAttribute managing a reference
*/
// ****************************************

using maReference = maReferenceHeritage<0>;



template<int notificationLevel>
class maStrongReferenceHeritage : public CoreModifiableAttributeData<maStrongReferenceObject>
{
	DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maStrongReferenceHeritage, maStrongReferenceHeritage, maStrongReferenceObject, CoreModifiable::ATTRIBUTE_TYPE::WEAK_REFERENCE);


public:

	maStrongReferenceHeritage(CoreModifiable& owner, bool isInitAttribute, KigsID ID, kstl::string value) : CoreModifiableAttributeData<maStrongReferenceObject>(owner, isInitAttribute, ID)
	{
		mValue = maStrongReferenceObject{ value };
		Search();
	}

	maStrongReferenceHeritage() : CoreModifiableAttributeData<maStrongReferenceObject>(KigsID{ 0u }, maStrongReferenceObject{}) {}

	virtual ~maStrongReferenceHeritage()
	{
	}

	// Sets the internal pointer to null, forcing to search again next time we query the reference
	void	ResetFoundModifiable()
	{
		mValue.mObj.reset();
	}
	virtual void CopyData(const CoreModifiableAttributeData<maStrongReferenceObject>& toCopy) override
	{
		const auto& toCopyValue = toCopy.const_ref();
		mValue = toCopyValue;
	}

	operator CMSP() { return SearchRef(); }

	template<typename T>
	operator SP<T>() { return SearchRef(); }

	operator CoreModifiable* () { return SearchRef().get(); }
	
	template<typename T>
	operator T* () { return static_cast<T*>(SearchRef().get()); }

	std::string RefString() const
	{
		std::string result;
		getValue(result);
		return result;
	}

	CoreModifiable* operator->()
	{
		return SearchRef().get();
	}

	// NOTE(antoine) null references ?????
	operator CoreModifiable& () { return (*SearchRef()); }
	CoreModifiable& ref() { return (*SearchRef()); }
	const CoreModifiable& const_ref() { return (*SearchRef()); }

	/// getValue overloads
	virtual bool getValue(kstl::string& value) const override
	{
		((maStrongReferenceHeritage*)this)->SearchRef();
		if (auto ptr = std::dynamic_pointer_cast<CoreModifiable>(mValue.mObj).get())
		{
#ifdef KEEP_NAME_AS_STRING
			value = ptr->GetRuntimeType();
#else
			value = std::to_string(ptr->GetRuntimeType().toUInt());
#endif

			value += ":";
			value += ptr->getName();
			return true;
		}
		else
		{
			value = mValue.mSearchString;
			return true;
		}

		return false;
	}
	virtual bool getValue(usString& value) const override
	{
		// TODO ?
		return false;
	}
	
	virtual bool getValue(CoreModifiable*& value) const override
	{
		value = (CoreModifiable*) const_cast<maStrongReferenceHeritage*>(this)->SearchRef().get();
		return true;
	}
	
	virtual bool getValue(void*& value) const override
	{
		value = (void*) const_cast<maStrongReferenceHeritage*>(this)->SearchRef().get();
		return true;
	}

	/// setValue overloads
	virtual bool setValue(const char* value) override
	{
		if (this->isReadOnly())
			return false;

		InitAndSearch(value);
		DO_NOTIFICATION(notificationLevel);
		return true;
	}
	virtual bool setValue(const kstl::string& value) override
	{
		if (this->isReadOnly())
			return false;

		InitAndSearch(value);
		DO_NOTIFICATION(notificationLevel);
		return true;
	}
	virtual bool setValue(CoreModifiable* value) override
	{
		if (this->isReadOnly())
			return false;
		mValue.mObj = value->SharedFromThis();
		DO_NOTIFICATION(notificationLevel);
		return true;
	}

	/// operators
	auto& operator=(const kstl::string& value)
	{
		InitAndSearch(value);
		DO_NOTIFICATION(notificationLevel);
		return *this;
	}
	auto& operator=(CoreModifiable* value)
	{
		setValue(value);
		return *this;
	}

protected:
	CMSP Search()
	{
		CMSP obj;
		if (!mValue.mSearchString.empty())
		{
			obj = CoreModifiable::SearchInstance(mValue.mSearchString, getOwner());
		}
		mValue.mObj = obj;
		return obj;
	}
	void InitAndSearch(const kstl::string& nametype)
	{
		auto old_obj = mValue.mObj;
		mValue = maStrongReferenceObject{ nametype };
		mValue.mObj = old_obj;
		Search();
	}
	CMSP SearchRef()
	{
		if (mValue.mObj)
			return mValue.mObj;
		return Search();
	}
};



using maStrongReference = maStrongReferenceHeritage<0>;





#endif //_MAREFERENCE_H