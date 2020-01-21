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
	kstl::string ref_file="";

	void InitWithJSON(const kstl::string& currentval, CoreModifiable* owner);
	bool ExportToString(kstl::string& value) const;

};


// ****************************************
// * maCoreItemHeritage class
// * --------------------------------------
/**
* \class	maCoreItemHeritage
* \ingroup	CoreModifiableAttibute
* \brief	CoreModifiableAttributeData of core item with different notification level
*/
// ****************************************
template<int notificationLevel>
class maCoreItemHeritage : public CoreModifiableAttributeData<maCoreItemValue>
{
	DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maCoreItemHeritage, maCoreItemHeritage, maCoreItemValue, CoreModifiable::COREITEM);

public:
	
	//! Extra constructor with kstl::string
	maCoreItemHeritage(CoreModifiable& owner, bool isInitAttribute, KigsID ID, kstl::string value) : CoreModifiableAttributeData(owner, isInitAttribute, ID ) 
	{
		if (value != "")
		{
			_value.InitWithJSON(value, &owner);
		}
	}
	
	// getValue overloads

	virtual bool getValue(bool&  value) const override { if (!_value.item.isNil()) { value = (bool)(*_value.item.get()); return true; }  return false; }
	virtual bool getValue(int&  value) const override { if (!_value.item.isNil()) { value = (int)(*_value.item.get()); return true; } return false; }
	virtual bool getValue(unsigned int&  value) const override { if (!_value.item.isNil()) { value = (unsigned int)(*_value.item.get()); return true; }  return false; }
	virtual bool getValue(kfloat&  value) const override { if (!_value.item.isNil()) { value = (kfloat)(*_value.item.get()); return true; }  return false; }
	virtual bool getValue(kstl::string& value) const override
	{
		if (!_value.item.isNil())
		{
			return _value.ExportToString(value);
		}
		return false;
	}
	virtual bool getValue(CoreItem*&  value) const override
	{
		value = (CoreItem*)_value.item.get();
		return true;
	}
	virtual bool getValue(void*& value) const override { value = (void*)_value.item.get(); return true; }

	///

	// setValue overloads
	virtual bool setValue(const char* value) override
	{
		if (this->isReadOnly())
			return false;

		_value.InitWithJSON(value, this->_owner);
		DO_NOTIFICATION(notificationLevel);
		return true;
	}
	virtual bool setValue(const kstl::string& value) override
	{
		if (this->isReadOnly())
			return false;

		_value.InitWithJSON(value, this->_owner);
		DO_NOTIFICATION(notificationLevel);
		return true;
	}
	virtual bool setValue(CoreItem*  value) override
	{
		if (this->isReadOnly())
			return false;
		
		if (_value.item != value)
		{
			_value.item = CoreItemSP(value, GetRefTag{});
			_value.ref_file = "";
			DO_NOTIFICATION(notificationLevel);
		}
		return true;
	}

	//! cast to CoreModifiable* operator
	operator CoreItem*() { return _value.item.get(); }
	//! cast to CoreModifiable& operator
	operator CoreItem&() { return (*_value.item.get()); }
	//! return a reference on internal value
	CoreItem& ref() { return (*_value.item.get()); }
	//! return a const reference on internal value
	const CoreItem& const_ref() { return (*_value.item.get()); }
	const kstl::string&	getRefFile() { return _value.ref_file; }


};

using maCoreItem = maCoreItemHeritage<0>;

#endif //_MACOREITEM_H