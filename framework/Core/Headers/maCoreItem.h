#ifndef _MACOREITEM_H
#define _MACOREITEM_H

#include "CoreModifiableAttribute.h"
#include "CoreItem.h"
#include "AttributeModifier.h"
#include "SmartPointer.h"



class maCoreItemValue
{
public:
	CoreItemSP item=nullptr;
	std::string ref_file="";

	void InitWithJSON(const std::string& currentval, CoreModifiable* owner);
	bool ExportToString(std::string& value) const;

};


// ****************************************
// * maCoreItemHeritage class
// * --------------------------------------
/**
* \class	maCoreItemHeritage
* \file		maCoreItem.h
* \ingroup	CoreModifiableAttibute
* \brief	CoreModifiableAttributeData of core item with different notification level
*/
// ****************************************
template<int notificationLevel>
class maCoreItemHeritage : public CoreModifiableAttributeData<maCoreItemValue>
{
	DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maCoreItemHeritage, maCoreItemHeritage, maCoreItemValue, CoreModifiable::ATTRIBUTE_TYPE::COREITEM);

public:
	
	//! Extra constructor with std::string
	maCoreItemHeritage(CoreModifiable& owner, bool isInitAttribute, KigsID ID, std::string value) : CoreModifiableAttributeData(owner, isInitAttribute, ID ) 
	{
		if (value != "")
		{
			mValue.InitWithJSON(value, &owner);
		}
	}
	
	// getValue overloads

	virtual bool getValue(bool&  value) const override { if (mValue.item) { value = (bool)(*mValue.item.get()); return true; }  return false; }
	virtual bool getValue(int&  value) const override { if (mValue.item) { value = (int)(*mValue.item.get()); return true; } return false; }
	virtual bool getValue(unsigned int&  value) const override { if (mValue.item) { value = (unsigned int)(*mValue.item.get()); return true; }  return false; }
	virtual bool getValue(float&  value) const override { if (mValue.item) { value = (float)(*mValue.item.get()); return true; }  return false; }
	virtual bool getValue(std::string& value) const override
	{
		if (mValue.item)
		{
			return mValue.ExportToString(value);
		}
		return false;
	}
	virtual bool getValue(CoreItem*&  value) const override
	{
		value = (CoreItem*)mValue.item.get();
		return true;
	}
	virtual bool getValue(void*& value) const override { value = (void*)mValue.item.get(); return true; }

	///

	// setValue overloads
	virtual bool setValue(const char* value) override
	{
		if (this->isReadOnly())
			return false;

		mValue.InitWithJSON(value, this->getOwner());
		DO_NOTIFICATION(notificationLevel);
		return true;
	}
	virtual bool setValue(const std::string& value) override
	{
		if (this->isReadOnly())
			return false;

		mValue.InitWithJSON(value, this->getOwner());
		DO_NOTIFICATION(notificationLevel);
		return true;
	}
	virtual bool setValue(CoreItem*  value) override
	{
		if (this->isReadOnly())
			return false;
		
		if (mValue.item.get() != value)
		{
			mValue.item = value->SharedFromThis();
			mValue.ref_file = "";
			DO_NOTIFICATION(notificationLevel);
		}
		return true;
	}

	//! cast to CoreModifiable* operator
	operator CoreItem*() { return mValue.item.get(); }
	//! cast to CoreModifiable& operator
	operator CoreItem&() { return (*mValue.item.get()); }
	
	operator CoreItemSP() { return mValue.item; }

	//! return a reference on internal value
	CoreItem& ref() { return (*mValue.item.get()); }
	//! return a const reference on internal value
	const CoreItem& const_ref() { return (*mValue.item.get()); }
	const std::string&	getRefFile() { return mValue.ref_file; }


};

// ****************************************
// * maCoreItem class
// * --------------------------------------
/**
* \class	maCoreItem
* \file		maCoreItem.h
* \ingroup	CoreModifiableAttibute
* \brief	CoreModifiable attribute managing a CoreItem
*/
// ****************************************

using maCoreItem = maCoreItemHeritage<0>;

#endif //_MACOREITEM_H