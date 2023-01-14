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
		template<int notificationLevel>
		class maCoreItemHeritage : public CoreModifiableAttributeData<maCoreItemValue>
		{
			DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maCoreItemHeritage, maCoreItemHeritage, maCoreItemValue, CoreModifiable::ATTRIBUTE_TYPE::COREITEM);

		public:

			//! Extra constructor with std::string
			maCoreItemHeritage(CoreModifiable& owner, bool isInitAttribute, KigsID ID, std::string value) : CoreModifiableAttributeData(owner, isInitAttribute, ID)
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
				if (this->isReadOnly())
					return false;

				mValue.InitWithJSON(value, this->getOwner());
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
			virtual bool setValue(const std::string& value, CoreModifiable* owner) override
			{
				if (this->isReadOnly())
					return false;

				mValue.InitWithJSON(value, this->getOwner());
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
			virtual bool setValue(CoreItemSP value, CoreModifiable* owner) override
			{
				if (this->isReadOnly())
					return false;

				if (mValue.item != value)
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

		using maCoreItem = maCoreItemHeritage<0>;

	}
}