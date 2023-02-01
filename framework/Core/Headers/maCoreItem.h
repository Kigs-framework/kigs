#pragma once

#include "CoreModifiableAttribute.h"
#include "CoreItem.h"
#include "SmartPointer.h"

namespace Kigs
{
	namespace Core
	{

		class maCoreItemValue
		{
		public:
			CoreItemSP item = nullptr;
			std::string ref_file = "";

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
	template<bool notificationLevel, bool isInitT = false, bool isReadOnlyT = false, bool isDynamicT = false, bool isOrphanT = false>
	class maCoreItemHeritage : public CoreModifiableAttributeData<maCoreItemValue, notificationLevel, isInitT, isReadOnlyT, isDynamicT, isOrphanT>
		{
			DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maCoreItemHeritage, maCoreItemHeritage, maCoreItemValue, CoreModifiable::ATTRIBUTE_TYPE::COREITEM);

		public:

			//! Extra constructor with std::string
			maCoreItemHeritage(CoreModifiable& owner, KigsID ID, std::string value) : CoreModifiableAttributeData<maCoreItemValue, notificationLevel, isInitT, isReadOnlyT, isDynamicT, isOrphanT>(owner, ID)
			{
				if (value != "")
				{
					mValue.InitWithJSON(value, &owner);
				}
			}

			// getValue overloads

			virtual bool getValue(bool& value, const CoreModifiable* owner) const override { if (mValue.item) { value = (bool)(*mValue.item.get()); return true; }  return false; }
			virtual bool getValue(int& value, const CoreModifiable* owner) const override { if (mValue.item) { value = (int)(*mValue.item.get()); return true; } return false; }
			virtual bool getValue(unsigned int& value, const CoreModifiable* owner) const override { if (mValue.item) { value = (unsigned int)(*mValue.item.get()); return true; }  return false; }
			virtual bool getValue(float& value, const CoreModifiable* owner) const override { if (mValue.item) { value = (float)(*mValue.item.get()); return true; }  return false; }
			virtual bool getValue(std::string& value, const CoreModifiable* owner) const override
			{
				if (mValue.item)
				{
					return mValue.ExportToString(value);
				}
				return false;
			}
			virtual bool getValue(CoreItemSP& value, const CoreModifiable* owner) const override
			{
				value = mValue.item;
				return true;
			}
			virtual bool getValue(void*& value, const CoreModifiable* owner) const override { value = (void*)mValue.item.get(); return true; }

			///

			// setValue overloads
			virtual bool setValue(const char* value, CoreModifiable* owner) override
			{
			RETURN_ON_READONLY(isReadOnlyT);
			
				mValue.InitWithJSON(value, owner);
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
			virtual bool setValue(const std::string& value, CoreModifiable* owner) override
			{
			RETURN_ON_READONLY(isReadOnlyT);

			mValue.InitWithJSON(value, owner);
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
			virtual bool setValue(CoreItemSP value, CoreModifiable* owner) override
			{
			RETURN_ON_READONLY(isReadOnlyT);

			if (mValue.item.get() != value.get())
				{
					mValue.item = value;
					mValue.ref_file = "";
					DO_NOTIFICATION(notificationLevel);
				}
				return true;
			}

			//! cast to CoreModifiable* operator
			operator CoreItem* () { return mValue.item.get(); }
			//! cast to CoreModifiable& operator
			operator CoreItem& () { return (*mValue.item.get()); }

			operator CoreItemSP() { return mValue.item; }

			//! return a reference on internal value
			CoreItem& ref() { return (*mValue.item.get()); }
			//! return a const reference on internal value
			const CoreItem& const_ref() { return (*mValue.item.get()); }
			const std::string& getRefFile() { return mValue.ref_file; }


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

		using maCoreItem = maCoreItemHeritage<false>;
		using maCoreItemDynamic = maCoreItemHeritage<false,false,false,true>;

	}
}