#ifndef _MAREFERENCE_H
#define _MAREFERENCE_H

#include "CoreModifiableAttribute.h"
#include "AttributeModifier.h"



struct maReferenceObject
{
	maReferenceObject() = default;

	maReferenceObject(CoreModifiable* lobj)
	{
		mObj = lobj;
	}
	maReferenceObject(const kstl::string& nametype) : mObj(nullptr)
	{
		mSearchString = nametype;
	}

	CoreModifiable* mObj;
	kstl::string	mSearchString;
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
class maReferenceHeritage : public CoreModifiableAttributeData<maReferenceObject>
{
	DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maReferenceHeritage, maReferenceHeritage, maReferenceObject, CoreModifiable::ATTRIBUTE_TYPE::REFERENCE);


public:

	maReferenceHeritage(CoreModifiable& owner, bool isInitAttribute, KigsID ID, kstl::string value) : CoreModifiableAttributeData<maReferenceObject>(owner, isInitAttribute, ID)
	{
		mValue = maReferenceObject{ value };
		Search();
	}

	maReferenceHeritage() : CoreModifiableAttributeData<maReferenceObject>(KigsID{0u}, maReferenceObject{}) {}

	virtual ~maReferenceHeritage()
	{
		if (mValue.mObj)
		{
			UnreferenceModifiable(mValue.mObj);
		}
	}


	// Sets the internal pointer to null, forcing to search again next time we query the reference
	void	ResetFoundModifiable()
	{
		mValue.mObj = nullptr;
	}
	virtual void CopyData(const CoreModifiableAttributeData<maReferenceObject>& toCopy) override
	{
		const auto& toCopyValue = toCopy.const_ref();
		if (toCopyValue.mObj)
		{
			// check if value has changed
			if (mValue.mObj != toCopyValue.mObj)
			{
				if (mValue.mObj)
					UnreferenceModifiable(mValue.mObj);

				mValue = toCopyValue;

				if (mValue.mObj)
					ReferenceModifiable(mValue.mObj);
			}
		}
		else
		{
			if (mValue.mObj)
				UnreferenceModifiable(mValue.mObj);

			mValue = toCopyValue;
		}
	}

	operator CoreModifiable*() { return SearchRef(); }
	//! cast to CoreModifiable* operator
	//operator CoreModifiable*() { return (CoreModifiable*)SearchRef(); }

	template<typename T>
	operator T*(){ return static_cast<T*>(SearchRef()); }

	operator const std::string& () const { return mValue.mSearchString; }

	CoreModifiable*	operator->()
	{
		return SearchRef();
	}

	operator CoreModifiable&() { return (*SearchRef()); }

	//! return a reference on internal value
	CoreModifiable& ref() { return (*SearchRef()); }
	//! return a const reference on internal value
	const CoreModifiable& const_ref() { return (*SearchRef()); }


	/// getValue overloads
	virtual bool getValue(kstl::string& value) const override
	{

		((maReferenceHeritage*)this)->SearchRef();
		if (mValue.mObj)
		{
#ifdef KEEP_NAME_AS_STRING
			value = mValue.mObj->GetRuntimeType();
#else
			value = std::to_string(mValue.mObj->GetRuntimeType().toUInt());
#endif

			value += ":";
			value += mValue.mObj->getName();
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
	virtual bool getValue(CoreModifiable*&  value) const override
	{
		value = (CoreModifiable*)((maReferenceHeritage*)this)->SearchRef();
		return true;
	}
	virtual bool getValue(void*& value) const override
	{
		value = (void*)((maReferenceHeritage*)this)->SearchRef();
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

		if (mValue.mObj != value)
		{
			if (mValue.mObj)
				UnreferenceModifiable(mValue.mObj);

			mValue.mObj = value;

			if (mValue.mObj)
				ReferenceModifiable(mValue.mObj);
		}

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

	/// Internals;
	void	UnreferenceModifiable(CoreModifiable* current)
	{
		auto& coremodigiablemap = KigsCore::Instance()->getReferenceMap();
		auto found = coremodigiablemap.find(current);

		// ok, the CoreModifiable is here
		if (found != coremodigiablemap.end())
		{
			kstl::vector<CoreModifiableAttribute*>& referencevector = (*found).second;

			kstl::vector<CoreModifiableAttribute*>::iterator	itcurrent = referencevector.begin();
			kstl::vector<CoreModifiableAttribute*>::iterator	itend = referencevector.end();

			while (itcurrent != itend)
			{
				if ((*itcurrent) == this)
				{
					referencevector.erase(itcurrent);
					break;
				}
				++itcurrent;
			}

			if (referencevector.size() == 0)
			{
				current->unflagAsReferenceRegistered();
				coremodigiablemap.erase(found);
			}
		}
	}
	void	ReferenceModifiable(CoreModifiable* current)
	{
		auto& coremodigiablemap = KigsCore::Instance()->getReferenceMap();
		auto found = coremodigiablemap.find(current);

		// ok, the CoreModifiable is already there, add a vector mEntry
		if (found != coremodigiablemap.end())
		{
			kstl::vector<CoreModifiableAttribute*>& referencevector = (*found).second;
			referencevector.push_back(this);
		}
		else
		{
			// create a new map mEntry
			kstl::vector<CoreModifiableAttribute*> toAdd;
			toAdd.push_back(this);
			coremodigiablemap[current] = toAdd;

			current->flagAsReferenceRegistered();
		}
	}

	void	Search()
	{
		CMSP obj = nullptr;
		if (!mValue.mSearchString.empty())
		{
			obj = CoreModifiable::SearchInstance(mValue.mSearchString, getOwner());
		}
		else
		{
			if (mValue.mObj)
			{
				UnreferenceModifiable(mValue.mObj);
				mValue.mObj = nullptr;
			}
			return;
		}
		if (obj)
		{
			if (mValue.mObj != obj.get())
			{
				if (mValue.mObj)
					UnreferenceModifiable(mValue.mObj);

				mValue.mObj = obj.get();
				ReferenceModifiable(mValue.mObj);
			}
		}
		else
		{
			if (mValue.mObj)
				UnreferenceModifiable(mValue.mObj);
			mValue.mObj = nullptr;
		}
	}
	void InitAndSearch(const kstl::string& nametype)
	{
		CoreModifiable* old_obj = mValue.mObj;
		mValue = maReferenceObject{ nametype };
		mValue.mObj = old_obj;
		Search();
	}
	CoreModifiable*	SearchRef()
	{
		if (mValue.mObj)
		{
			return mValue.mObj;
		}
		Search();
		return mValue.mObj;
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







#endif //_MAREFERENCE_H