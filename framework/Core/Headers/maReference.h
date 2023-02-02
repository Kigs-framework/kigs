#pragma once

#include "CoreModifiableAttribute.h"


namespace Kigs
{
	namespace Core
	{
		struct maWeakReferenceObject
		{
			maWeakReferenceObject() = default;

			maWeakReferenceObject(CoreModifiable* lobj)
			{
				if (lobj)
					mObj = lobj->SharedFromThis();
			}
			maWeakReferenceObject(const std::string& nametype)
			{
				mSearchString = nametype;
			}
			std::weak_ptr<CoreModifiable> mObj;
			std::string mSearchString;
		};

		struct maStrongReferenceObject
		{
			maStrongReferenceObject() = default;
			maStrongReferenceObject(GenericRefCountedBaseClass* lobj)
			{
				if (lobj)
					mObj = lobj->shared_from_this();
			}
			maStrongReferenceObject(SP<GenericRefCountedBaseClass> lobj)
			{
				mObj = lobj;
			}
			maStrongReferenceObject(const std::string& nametype)
			{
				mSearchString = nametype;
			}
			SP<GenericRefCountedBaseClass> mObj;
			std::string mSearchString;
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

	template<bool notificationLevel, bool isInitT = false, bool isReadOnlyT = false, bool isOrphanT = false>
	class maReferenceHeritage : public CoreModifiableAttributeData<maWeakReferenceObject, notificationLevel, isInitT, isReadOnlyT, isOrphanT>
		{
			DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maReferenceHeritage, maReferenceHeritage, maWeakReferenceObject, CoreModifiable::ATTRIBUTE_TYPE::WEAK_REFERENCE);


		public:

		maReferenceHeritage(CoreModifiable& owner, KigsID ID, std::string value) : CoreModifiableAttributeData<maWeakReferenceObject, notificationLevel, isInitT, isReadOnlyT, isOrphanT>(owner, ID)
			, mOwner(&owner)
			, mID(ID)
			{
				mValue = maWeakReferenceObject{ value };
				Search();
			}

		maReferenceHeritage() : CoreModifiableAttributeData<maWeakReferenceObject, notificationLevel, isInitT, isReadOnlyT, isOrphanT>(KigsID{ 0u }, maWeakReferenceObject{}) {}

			virtual ~maReferenceHeritage()
			{
			}

		virtual void changeInheritance(u8 mask) final
		{
			CoreModifiable* prevOwner(mOwner);
			KigsID			prevID(mID);

			mID.~KigsID();
			maWeakReferenceObject old_value = mValue;
			mValue.~maWeakReferenceObject();
			this->doPlacementNew(mask);
			mValue = old_value;

			mID = prevID;
			mOwner = prevOwner;
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
			operator SP<T>() { return SearchRef(); }

			// NOTE(antoine) Thread unsafe, need to remove method
			operator CoreModifiable* () { return SearchRef().get(); }
			// NOTE(antoine) Thread unsafe, need to remove method
			template<typename T>
			operator T* () { return static_cast<T*>(SearchRef().get()); }

			// TODO
			std::string RefString() const
			{
				std::string result;
				getValue(result,nullptr);
				return result;
			}

			CoreModifiable* operator->()
			{
				return SearchRef().get();
			}

			// NOTE(antoine) null references ?????
			// Thread unsafe, need to remove method
			operator CoreModifiable& () { return (*SearchRef()); }
			CoreModifiable& ref() { return (*SearchRef()); }
			const CoreModifiable& const_ref() { return (*SearchRef()); }


			/// getValue overloads
			virtual bool getValue(std::string& value,const CoreModifiable* owner) const override
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
			virtual bool getValue(usString& value, const CoreModifiable* owner) const override
			{
				// TODO ?
				return false;
			}

			// Thread unsafe, need to add getValue method with CMSP
			virtual bool getValue(CMSP& value, const CoreModifiable* owner) const override
			{
				value = const_cast<maReferenceHeritage*>(this)->SearchRef();
				return true;
			}

			// Thread unsafe, this one is really bad...
			virtual bool getValue(void*& value, const CoreModifiable* owner) const override
			{
				value = (void*) const_cast<maReferenceHeritage*>(this)->SearchRef().get();
				return true;
			}

			/// setValue overloads
			virtual bool setValue(const char* value, CoreModifiable* owner) override
			{
			RETURN_ON_READONLY(isReadOnlyT);

				InitAndSearch(value);
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
			virtual bool setValue(const std::string& value, CoreModifiable* owner) override
			{
			RETURN_ON_READONLY(isReadOnlyT);

				InitAndSearch(value);
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
			virtual bool setValue(CMSP value, CoreModifiable* owner) override
			{
			RETURN_ON_READONLY(isReadOnlyT);
			mValue.mObj = value;
				DO_NOTIFICATION(notificationLevel);
				return true;
			}

			/// operators
			auto& operator=(const std::string& value)
			{
				InitAndSearch(value);
			CoreModifiable* owner = mOwner;
			KigsID& id = mID;
				DO_NOTIFICATION(notificationLevel);
				return *this;
			}
			// TODO
			auto& operator=(CMSP value)
			{
				mValue.mObj = value;
				return *this;
			}

		protected:
			CMSP Search()
			{
				CMSP obj;
				if (!mValue.mSearchString.empty())
				{
				obj = CoreModifiable::SearchInstance(mValue.mSearchString, mOwner);
				}
				mValue.mObj = obj;
				return obj;
			}
			void InitAndSearch(const std::string& nametype)
			{
				auto old_obj = mValue.mObj;
				mValue = maWeakReferenceObject{ nametype };
				mValue.mObj = old_obj;
				Search();
			}
			CMSP SearchRef()
			{
				auto ptr = mValue.mObj.lock();
				if (ptr)
					return ptr;
				return Search();
			}

		CoreModifiable* mOwner = nullptr;
		KigsID			mID;
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

		using maReference = maReferenceHeritage<false,false,false,false>;
		using maReferenceOrphan = maReferenceHeritage<false, false, false, true>;
		using maReferenceInit = maReferenceHeritage<false, true, false, false>;


	template<bool notificationLevel, bool isInitT = false, bool isReadOnlyT = false, bool isOrphanT = false>
	class maStrongReferenceHeritage : public CoreModifiableAttributeData<maStrongReferenceObject, notificationLevel, isInitT, isReadOnlyT, isOrphanT>
		{
			DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maStrongReferenceHeritage, maStrongReferenceHeritage, maStrongReferenceObject, CoreModifiable::ATTRIBUTE_TYPE::WEAK_REFERENCE);


		public:

		maStrongReferenceHeritage(CoreModifiable& owner, KigsID ID, std::string value) : CoreModifiableAttributeData<maStrongReferenceObject, notificationLevel, isInitT, isReadOnlyT, isOrphanT>(owner, ID)
			, mOwner(&owner)
			, mID(ID)
			{
				mValue = maStrongReferenceObject{ value };
				Search();
			}

		maStrongReferenceHeritage() : CoreModifiableAttributeData<maStrongReferenceObject, notificationLevel, isInitT, isReadOnlyT, isOrphanT>(KigsID{ 0u }, maStrongReferenceObject{}) {}

			virtual ~maStrongReferenceHeritage()
			{
			}

		virtual void changeInheritance(u8 mask) final
		{
			CoreModifiable* prevOwner(mOwner);
			KigsID			prevID(mID);

			mID.~KigsID();
			maStrongReferenceObject old_value = mValue;
			mValue.~maStrongReferenceObject();
			this->doPlacementNew(mask);
			mValue = old_value;

			mID = prevID;
			mOwner = prevOwner;
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
			virtual bool getValue(std::string& value, const CoreModifiable* owner) const override
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
			virtual bool getValue(usString& value, const CoreModifiable* owner) const override
			{
				// TODO ?
				return false;
			}

			virtual bool getValue(CMSP& value, const CoreModifiable* owner) const override
			{
				value =  const_cast<maStrongReferenceHeritage*>(this)->SearchRef();
				return true;
			}

			virtual bool getValue(void*& value, const CoreModifiable* owner) const override
			{
				value = (void*) const_cast<maStrongReferenceHeritage*>(this)->SearchRef().get();
				return true;
			}

			/// setValue overloads
			virtual bool setValue(const char* value, CoreModifiable* owner) override
			{
			RETURN_ON_READONLY(isReadOnlyT);

				InitAndSearch(value);
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
			virtual bool setValue(const std::string& value,  CoreModifiable* owner) override
			{
			RETURN_ON_READONLY(isReadOnlyT);

				InitAndSearch(value);
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
			virtual bool setValue(CMSP value,  CoreModifiable* owner) override
			{
			RETURN_ON_READONLY(isReadOnlyT);
				mValue.mObj = value;
				DO_NOTIFICATION(notificationLevel);
				return true;
			}

			/// operators
			auto& operator=(const std::string& value)
			{
				InitAndSearch(value);
			CoreModifiable* owner = mOwner;
			KigsID& id = mID;
				DO_NOTIFICATION(notificationLevel);
				return *this;
			}
			auto& operator=(CoreModifiable* value)
			{
			setValue(value, mOwner, mID);
				return *this;
			}

		protected:
			CMSP Search()
			{
				CMSP obj;
				if (!mValue.mSearchString.empty())
				{
				obj = CoreModifiable::SearchInstance(mValue.mSearchString, mOwner);
				}
				mValue.mObj = obj;
				return obj;
			}
			void InitAndSearch(const std::string& nametype)
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

		CoreModifiable* mOwner = nullptr;
		KigsID			mID;
		};



		using maStrongReference = maStrongReferenceHeritage<false,false,false,false>;
		using maStrongReferenceOrphan = maStrongReferenceHeritage<false, false, false, true>;

	}
}