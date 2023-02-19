#pragma once

namespace Kigs
{

	namespace Core
	{
		// ****************************************
		// * CoreModifiableAttributeMap class
		// * --------------------------------------
		/**
		* \class	CoreModifiableAttributeMap
		* \file		CoreModifiableAttribute.h
		* \ingroup CoreModifiableAttibute
		* \brief	Base template class for CoreModifiable attributes mapping already existing member value
		*/
		// ****************************************

		template<typename T, bool notifOwnerT = false, bool isInitT = false, bool isReadOnlyT = false>
		class CoreModifiableAttributeMap : public CoreModifiableAttributeDataInterface<2, T, notifOwnerT, isInitT, isReadOnlyT, false>
		{
			using CoreModifiableAttribute::mID;
			using CoreModifiableAttribute::mPlaceHolderForSonClasses;

		private:
			CoreModifiableAttributeMap(CoreModifiable& owner, KigsID ID, const T& value) = delete;
			CoreModifiableAttributeMap(CoreModifiable& owner, KigsID ID) = delete;

		protected:
			friend CoreModifiable;
			// special constructor, the owner will be set afterward
			CoreModifiableAttributeMap(uintptr_t offsetOnValue, const KigsID& ID) : CoreModifiableAttributeDataInterface<2,T,notifOwnerT, isInitT, isReadOnlyT, false >(nullptr, ID)
			{
				mPlaceHolderForSonClasses = (u32)offsetOnValue;
			}
		public:

			explicit CoreModifiableAttributeMap(InheritanceSwitch tag) : CoreModifiableAttributeDataInterface<2,T,notifOwnerT, isInitT, isReadOnlyT, false>(tag) {}

			friend class CoreModifiable;

		protected:

			void doPlacementNew(u8 mask) override
			{
				switch (mask)
				{
					case 0: new (this) CoreModifiableAttributeMap<T, false, false, false>(InheritanceSwitch{}); break;
					case 1: new (this) CoreModifiableAttributeMap<T, true, false, false>(InheritanceSwitch{}); break;
					case 2: new (this) CoreModifiableAttributeMap<T, false, true, false>(InheritanceSwitch{}); break;
					case 3: new (this) CoreModifiableAttributeMap<T, true, true, false>(InheritanceSwitch{}); break;
					case 4: new (this) CoreModifiableAttributeMap<T, false, false, true>(InheritanceSwitch{}); break;
					case 5: new (this) CoreModifiableAttributeMap<T, true, false, true>(InheritanceSwitch{}); break;
					case 6: new (this) CoreModifiableAttributeMap<T, false, true, true>(InheritanceSwitch{}); break;
					case 7: new (this) CoreModifiableAttributeMap<T, true, true, true>(InheritanceSwitch{}); break;
					default: assert(false); break;
				}
			}

			void changeInheritance(u8 mask) override
			{
				u32	keepID = mID;
				u32 keepPlaceHolder = mPlaceHolderForSonClasses;
				this->doPlacementNew(mask);
				mID = keepID;
				mPlaceHolderForSonClasses = keepPlaceHolder;
			}

			void CopyData(const CoreModifiableAttribute& other) override
			{
				// should not be there
				assert(false);
			}

		};

		// specialized

		template<typename pointed, bool notifOwnerT, bool isInitT, bool isReadOnlyT>
		class CoreModifiableAttributeMap<std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT> : public CoreModifiableAttributeDataInterface<2, std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT,false>
		{
			using CoreModifiableAttribute::mID;
			using CoreModifiableAttribute::mPlaceHolderForSonClasses;

			using CoreModifiableAttributeDataInterface<2, std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT, false >::valueProtectedAccess;

		private:
			CoreModifiableAttributeMap<std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>(CoreModifiable& owner, KigsID ID, const std::weak_ptr<pointed>& value) = delete;
			CoreModifiableAttributeMap<std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>(CoreModifiable& owner, KigsID ID) = delete;

		protected:
			friend CoreModifiable;
			// special constructor, the owner will be set afterward
			CoreModifiableAttributeMap<std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>(uintptr_t offsetOnValue, const KigsID& ID) : CoreModifiableAttributeDataInterface<2, std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT, false >(nullptr, ID)
			{
				mPlaceHolderForSonClasses = (u32)offsetOnValue;
			}
		public:

			explicit CoreModifiableAttributeMap<std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>(InheritanceSwitch tag) : CoreModifiableAttributeDataInterface<2, std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT, false>(tag) {}

			friend class CoreModifiable;

		public:

			bool setValue(const std::string& val, CoreModifiable* owner) override
			{
				RETURN_ON_READONLY(isReadOnlyT);
				mSearchString = val;
				::std::weak_ptr<pointed> prev = valueProtectedAccess(owner);
				Search(owner);
				::std::weak_ptr<pointed> newone = valueProtectedAccess(owner);
				if (prev.lock() != newone.lock())
				{
					DO_NOTIFICATION(notifOwnerT);
					return true;
				}
				return false;
			}
			bool setValue(CMSP val, CoreModifiable* owner) override
			{
				RETURN_ON_READONLY(isReadOnlyT);
				mSearchString.clear();
				::std::weak_ptr<pointed> prev = valueProtectedAccess(owner);
				valueProtectedAccess(owner) = val;
				if (prev.lock() != val)
				{
					DO_NOTIFICATION(notifOwnerT);
					return true;
				}
				return false;
			}

			bool setValue(const char* val, CoreModifiable* owner) override
			{
				return setValue(std::string(val), owner);
			}

			bool setValue(const usString& val, CoreModifiable* owner) override
			{
				return setValue(val.ToString(), owner);
			}

			bool setValue(const unsigned short* val, CoreModifiable* owner) override
			{
				return setValue(usString(val), owner);
			}

			bool setValue(const UTF8Char* val, CoreModifiable* owner) override
			{
				return setValue(usString(val), owner);
			}

			virtual bool getValue(std::string& val, const CoreModifiable* owner) const override
			{
				const_cast<CoreModifiableAttributeMap<::std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>*>(this)->Search(owner);
				if (auto ptr = valueProtectedAccess(owner).lock())
				{
#ifdef KEEP_NAME_AS_STRING
					val = ptr->GetRuntimeType();
#else
					val = ::std::to_string(ptr->GetRuntimeType().toUInt());
#endif

					val += ":";
					val += ptr->getName();
					return true;
				}
				else
				{
					val = mSearchString;
					return true;
				}

				return false;
			}
			virtual bool getValue(usString& value, const CoreModifiable* owner) const override
			{
				std::string result;
				if (getValue(result, owner))
				{
					value = result;
					return true;
				}
				return false;
			}

			virtual bool getValue(CMSP& val, const CoreModifiable* owner) const override
			{
				auto localthis = const_cast<CoreModifiableAttributeMap<::std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>*>(this);
				localthis->Search(owner);
				val = valueProtectedAccess(owner).lock();
				return true;
			}

		protected:

			void Search(const CoreModifiable* instance)
			{
				if (!mSearchString.empty())
				{
					::std::weak_ptr<pointed>& val = valueProtectedAccess(instance);

					val = CoreModifiable::SearchInstance(mSearchString, const_cast<CoreModifiable*>(instance));

					if (!val.expired())
					{
						mSearchString.clear();
					}

				}
			}

			void changeInheritance(u8 mask) override
			{
				u32	keepID = CoreModifiableAttribute::mID;
				u32 keepPlaceHolder = CoreModifiableAttribute::mPlaceHolderForSonClasses;

				std::string copyString = mSearchString;
				mSearchString.~basic_string();
				doPlacementNew(mask);
				mSearchString = copyString;
				CoreModifiableAttribute::mID = keepID;
				CoreModifiableAttribute::mPlaceHolderForSonClasses = keepPlaceHolder;
			}

			void doPlacementNew(u8 mask) override
			{
				switch (mask)
				{
				case 0: new (this) CoreModifiableAttributeMap<::std::weak_ptr<pointed>, false, false, false>(InheritanceSwitch{}); break;
				case 1: new (this) CoreModifiableAttributeMap<::std::weak_ptr<pointed>, true, false, false>(InheritanceSwitch{}); break;
				case 2: new (this) CoreModifiableAttributeMap<::std::weak_ptr<pointed>, false, true, false>(InheritanceSwitch{}); break;
				case 3: new (this) CoreModifiableAttributeMap<::std::weak_ptr<pointed>, true, true, false>(InheritanceSwitch{}); break;
				case 4: new (this) CoreModifiableAttributeMap<::std::weak_ptr<pointed>, false, false, true>(InheritanceSwitch{}); break;
				case 5: new (this) CoreModifiableAttributeMap<::std::weak_ptr<pointed>, true, false, true>(InheritanceSwitch{}); break;
				case 6: new (this) CoreModifiableAttributeMap<::std::weak_ptr<pointed>, false, true, true>(InheritanceSwitch{}); break;
				case 7: new (this) CoreModifiableAttributeMap<::std::weak_ptr<pointed>, true, true, true>(InheritanceSwitch{}); break;

				default: assert(false); break;
				}
			}

			std::string	mSearchString;
			void CopyData(const CoreModifiableAttribute& other) override
			{
				// should not be there
				assert(false);
			}
		};

		template<typename pointed, bool notifOwnerT, bool isInitT, bool isReadOnlyT>
		class CoreModifiableAttributeMap<std::shared_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT> : public CoreModifiableAttributeDataInterface<2, std::shared_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>
		{
			using CoreModifiableAttribute::mID;
			using CoreModifiableAttribute::mPlaceHolderForSonClasses;
			using CoreModifiableAttributeDataInterface<2, std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT, false >::valueProtectedAccess;


		private:
			CoreModifiableAttributeMap<std::shared_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>(CoreModifiable& owner, KigsID ID, const std::shared_ptr<pointed>& value) = delete;
			CoreModifiableAttributeMap<std::shared_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>(CoreModifiable& owner, KigsID ID) = delete;

		protected:
			friend CoreModifiable;
			// special constructor, the owner will be set afterward
			CoreModifiableAttributeMap<std::shared_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>(uintptr_t offsetOnValue, const KigsID& ID) : CoreModifiableAttributeDataInterface<2, std::shared_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT, false >(nullptr, ID)
			{
				mPlaceHolderForSonClasses = (u32)offsetOnValue;
			}
		public:

			explicit CoreModifiableAttributeMap<std::shared_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>(InheritanceSwitch tag) : CoreModifiableAttributeDataInterface<2, std::shared_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT, false>(tag) {}

			friend class CoreModifiable;

		public:

			bool setValue(const std::string& val, CoreModifiable* owner) override
			{
				RETURN_ON_READONLY(isReadOnlyT);
				mSearchString = val;
				::std::shared_ptr<pointed> prev = valueProtectedAccess(owner);
				Search(owner);
				::std::shared_ptr<pointed> newone = valueProtectedAccess(owner);
				if (prev != newone)
				{
					DO_NOTIFICATION(notifOwnerT);
					return true;
				}
				return false;
			}
			bool setValue(CMSP val, CoreModifiable* owner) override
			{
				RETURN_ON_READONLY(isReadOnlyT);
				mSearchString.clear();
				::std::shared_ptr<pointed> prev = valueProtectedAccess(owner);
				valueProtectedAccess(owner) = val;
				if (prev != val)
				{
					DO_NOTIFICATION(notifOwnerT);
					return true;
				}
				return false;
			}

			bool setValue(const char* val, CoreModifiable* owner) override
			{
				return setValue(std::string(val), owner);
			}

			bool setValue(const usString& val, CoreModifiable* owner) override
			{
				return setValue(val.ToString(), owner);
			}

			bool setValue(const unsigned short* val, CoreModifiable* owner) override
			{
				return setValue(usString(val), owner);
			}

			bool setValue(const UTF8Char* val, CoreModifiable* owner) override
			{
				return setValue(usString(val), owner);
			}

			virtual bool getValue(std::string& val, const CoreModifiable* owner) const override
			{
				const_cast<CoreModifiableAttributeMap<::std::shared_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>*>(this)->Search(owner);
				if (auto ptr = valueProtectedAccess(owner))
				{
#ifdef KEEP_NAME_AS_STRING
					val = ptr->GetRuntimeType();
#else
					val = ::std::to_string(ptr->GetRuntimeType().toUInt());
#endif

					val += ":";
					val += ptr->getName();
					return true;
				}
				else
				{
					val = mSearchString;
					return true;
				}

				return false;
			}
			virtual bool getValue(usString& value, const CoreModifiable* owner) const override
			{
				std::string result;
				if (getValue(result, owner))
				{
					value = result;
					return true;
				}
				return false;
			}

			virtual bool getValue(CMSP& val, const CoreModifiable* owner) const override
			{
				const_cast<CoreModifiableAttributeMap<::std::shared_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>*>(this)->Search(owner);
				val = valueProtectedAccess(owner);
				return true;
			}

		protected:

			void Search(const CoreModifiable* instance)
			{
				if (!mSearchString.empty())
				{
					::std::shared_ptr<pointed>& val = valueProtectedAccess(instance);

					val = CoreModifiable::SearchInstance(mSearchString, const_cast<CoreModifiable*>(instance));

					if (val)
					{
						mSearchString.clear();
					}

				}
			}

			void changeInheritance(u8 mask) override
			{
				u32	keepID = CoreModifiableAttribute::mID;
				u32 keepPlaceHolder = CoreModifiableAttribute::mPlaceHolderForSonClasses;

				std::string copyString = mSearchString;
				mSearchString.~basic_string();
				doPlacementNew(mask);
				mSearchString = copyString;

				CoreModifiableAttribute::mID = keepID;
				CoreModifiableAttribute::mPlaceHolderForSonClasses = keepPlaceHolder;

			}

			void doPlacementNew(u8 mask) override
			{
				switch (mask)
				{
				case 0: new (this) CoreModifiableAttributeMap<::std::shared_ptr<pointed>, false, false, false>(InheritanceSwitch{}); break;
				case 1: new (this) CoreModifiableAttributeMap<::std::shared_ptr<pointed>, true, false, false>(InheritanceSwitch{}); break;
				case 2: new (this) CoreModifiableAttributeMap<::std::shared_ptr<pointed>, false, true, false>(InheritanceSwitch{}); break;
				case 3: new (this) CoreModifiableAttributeMap<::std::shared_ptr<pointed>, true, true, false>(InheritanceSwitch{}); break;
				case 4: new (this) CoreModifiableAttributeMap<::std::shared_ptr<pointed>, false, false, true>(InheritanceSwitch{}); break;
				case 5: new (this) CoreModifiableAttributeMap<::std::shared_ptr<pointed>, true, false, true>(InheritanceSwitch{}); break;
				case 6: new (this) CoreModifiableAttributeMap<::std::shared_ptr<pointed>, false, true, true>(InheritanceSwitch{}); break;
				case 7: new (this) CoreModifiableAttributeMap<::std::shared_ptr<pointed>, true, true, true>(InheritanceSwitch{}); break;

				default: assert(false); break;
				}
			}
			void CopyData(const CoreModifiableAttribute& other) override
			{
				// should not be there
				assert(false);
			}

			std::string	mSearchString;
		};
	}
}