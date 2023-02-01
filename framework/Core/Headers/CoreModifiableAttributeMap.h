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

		// CoreModifiableAttributeMapBase are never dynamic or orphan
		template<typename T, bool notifOwnerT = false, bool isInitT = false, bool isReadOnlyT = false>
		class CoreModifiableAttributeMapBase : public CoreModifiableAttributeTemplated< notifOwnerT, isInitT, isReadOnlyT, false, false>
		{
			using CoreModifiableAttribute::mID;
			using CoreModifiableAttribute::mPlaceHolderForSonClasses;
		private:
			CoreModifiableAttributeMapBase(CoreModifiable& owner, KigsID ID, const T& value) = delete;
			CoreModifiableAttributeMapBase(CoreModifiable& owner, KigsID ID) = delete;

		protected:
			friend CoreModifiable;
			// special constructor, the owner will be set afterward
			CoreModifiableAttributeMapBase(uintptr_t offsetOnValue, const KigsID& ID) : CoreModifiableAttributeTemplated<notifOwnerT, isInitT, isReadOnlyT, false, false >(nullptr, ID)
			{
				mPlaceHolderForSonClasses = (u32)offsetOnValue;
			}
		public:

			explicit CoreModifiableAttributeMapBase(InheritanceSwitch tag) : CoreModifiableAttributeTemplated<notifOwnerT, isInitT, isReadOnlyT, false, false>(tag) {}

			friend class CoreModifiable;
			size_t MemorySize() const override { return sizeof(T); };

			CoreModifiable::ATTRIBUTE_TYPE getType() const override { return TypeToEnum<T>::value; }
			CoreModifiable::ATTRIBUTE_TYPE getArrayElementType() const override { return AraryTypeToEnum<T>::value; }

			size_t getNbArrayElements() const override {
				if constexpr (impl::is_array<std::remove_cv_t<T>>::value)
				{
					return impl::arraySize<T>();
				}
				return 0;
			}
			size_t getNbArrayColumns() const override {
				if constexpr (impl::is_array<std::remove_cv_t<T>>::value)
				{
					return impl::arrayColumnCount<T>();
				}
				return 0;
			}
			size_t getNbArrayLines() const override {
				if constexpr (impl::is_array<std::remove_cv_t<T>>::value)
				{
					return impl::arrayLineCount<T>();
				}
				return 0;
			}

			void* getRawValue(CoreModifiable* owner) override { return &value(owner); };

			// TODO Return true if the copy was done correctly
			bool	CopyAttribute(const CoreModifiableAttribute& other) { return false; }


#define DECLARE_SET(type)	bool setValue(type val,CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); if( CoreConvertValue(val,value(owner)) ) { DO_NOTIFICATION(notifOwnerT); return true;} return false;  }

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SET);
			DECLARE_SET(const char*);
			DECLARE_SET(const std::string&);
			DECLARE_SET(const unsigned short*);
			DECLARE_SET(const usString&);
			DECLARE_SET(const UTF8Char*);
			DECLARE_SET(const v2f&);
			DECLARE_SET(const v3f&);
			DECLARE_SET(const v4f&);

#define DECLARE_GET(type)	bool getValue(type val,CoreModifiable* owner) const override { return CoreConvertValue(value(owner),val); }

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, DECLARE_GET);
			DECLARE_GET(std::string&);
			DECLARE_GET(usString&);
			DECLARE_GET(v2f&);
			DECLARE_GET(v3f&);
			DECLARE_GET(v4f&);


#define DECLARE_SETARRAYVALUE(type)	bool setArrayValue(type val,CoreModifiable* owner, size_t nbElements ) override {RETURN_ON_READONLY(isReadOnlyT);\
		if constexpr(impl::is_array<std::remove_cv_t<T>>::value) {\
			size_t currentColumnIndex=0, currentLineIndex=0;\
			for (size_t i = 0; i < nbElements; i++){\
				if constexpr(impl::arrayLineCount<std::remove_cv_t<T>>()>1){\
					if (!CoreConvertValue(val[i], value(owner)[currentColumnIndex][currentLineIndex])) { return false; }\
				}else {\
					if (!CoreConvertValue(val[i], value(owner)[currentColumnIndex])) { return false; }\
				}\
				currentLineIndex++;\
				if (currentLineIndex >= impl::arrayLineCount<std::remove_cv_t<T>>()){\
					currentColumnIndex++; currentLineIndex=0;\
					if (impl::arrayColumnCount<std::remove_cv_t<T>>() <= currentColumnIndex){break;}\
				}}\
			DO_NOTIFICATION(notifOwnerT); return true;}\
		return false; }

			EXPAND_MACRO_FOR_BASE_TYPES(const, *, DECLARE_SETARRAYVALUE);

#define DECLARE_GETARRAYVALUE(type) bool getArrayValue(type * const  val  ,CoreModifiable* owner, size_t  nbElements ) const override {\
		if constexpr(impl::is_array<std::remove_cv_t<T>>::value) {\
			size_t currentColumnIndex=0, currentLineIndex=0;\
			for (size_t i = 0; i < nbElements; i++){\
				if constexpr(impl::arrayLineCount<std::remove_cv_t<T>>()>1){\
					if (!CoreConvertValue(value(owner)[currentColumnIndex][currentLineIndex],val[i])) { return false; }\
				}else {\
					if (!CoreConvertValue(value(owner)[currentColumnIndex],val[i])) { return false; }\
				}\
				currentLineIndex++;\
				if (currentLineIndex >= impl::arrayLineCount<std::remove_cv_t<T>>()){\
					currentColumnIndex++; currentLineIndex=0;\
					if (impl::arrayColumnCount<std::remove_cv_t<T>>() <= currentColumnIndex){break;}\
				}}\
			DO_NOTIFICATION(notifOwnerT); return true;}\
		return false; }

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_GETARRAYVALUE);

#define DECLARE_SETARRAYELEMENTVALUE(type)	bool setArrayElementValue(type val,CoreModifiable* owner, size_t  line , size_t  column ) override {RETURN_ON_READONLY(isReadOnlyT);\
		if constexpr(impl::is_array<std::remove_cv_t<T>>::value) {\
			if( (line >= impl::arrayLineCount<std::remove_cv_t<T>>()) || (column >= impl::arrayColumnCount<std::remove_cv_t<T>>())) {return false;}\
			if constexpr(impl::arrayLineCount<std::remove_cv_t<T>>()>1){\
				if (!CoreConvertValue(val, value(owner)[column][line])) { return false; }\
			}else{\
				if (!CoreConvertValue(val, value(owner)[column])) { return false; }\
			}\
			DO_NOTIFICATION(notifOwnerT);\
			return true;}\
		return false; }

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SETARRAYELEMENTVALUE);
			DECLARE_SETARRAYELEMENTVALUE(const std::string&);

#define DECLARE_GETARRAYELEMENTVALUE(type)	bool getArrayElementValue(type  val ,CoreModifiable* owner, size_t  line , size_t  column ) const override{\
		if constexpr(impl::is_array<std::remove_cv_t<T>>::value) {\
			if( (line >= impl::arrayLineCount<std::remove_cv_t<T>>()) || (column >= impl::arrayColumnCount<std::remove_cv_t<T>>())) {return false;}\
			if constexpr(impl::arrayLineCount<std::remove_cv_t<T>>()>1){\
				if (!CoreConvertValue(value(owner)[column][line],val)) {return false;}\
			}else{\
				if (!CoreConvertValue(value(owner)[column],val)) {return false;}\
			}\
			return true;}\
		return false; }

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, DECLARE_GETARRAYELEMENTVALUE);
			DECLARE_GETARRAYELEMENTVALUE(std::string&);

#undef DECLARE_SET
#undef DECLARE_GET
#undef DECLARE_SETARRAYVALUE
#undef DECLARE_GETARRAYVALUE
#undef DECLARE_SETARRAYELEMENTVALUE
#undef DECLARE_GETARRAYELEMENTVALUE


		protected:



			T& value(void* instance)
			{
				uintptr_t pos((uintptr_t)instance);
				pos += mPlaceHolderForSonClasses;
				return *((T*)pos);
			}

			const T& value(void* instance) const
			{
				uintptr_t pos((uintptr_t)instance);
				pos += mPlaceHolderForSonClasses;
				return *((const T*)pos);
			}

			void changeInheritance(u8 mask) override
			{
				u32	keepID = mID;
				u32 keepPlaceHolder = mPlaceHolderForSonClasses;
				this->doPlacementNew(mask);
				mID = keepID;
				mPlaceHolderForSonClasses = keepPlaceHolder;
			}
		};

		template<typename T, bool notifOwnerT = false, bool isInitT = false, bool isReadOnlyT = false>
		class CoreModifiableAttributeMap : public CoreModifiableAttributeMapBase<T, notifOwnerT, isInitT, isReadOnlyT>
		{
			using CoreModifiableAttributeMapBase<T, notifOwnerT, isInitT, isReadOnlyT>::CoreModifiableAttributeMapBase;

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

		};

		// specialized

		template<typename pointed, bool notifOwnerT, bool isInitT, bool isReadOnlyT>
		class CoreModifiableAttributeMap<std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT> : public CoreModifiableAttributeMapBase<std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>
		{
			using CoreModifiableAttributeMapBase<::std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>::CoreModifiableAttributeMapBase;
			using CoreModifiableAttributeMapBase<::std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>::value;
		public:

			bool setValue(const std::string& val, CoreModifiable* owner) override
			{
				RETURN_ON_READONLY(isReadOnlyT);
				mSearchString = val;
				::std::weak_ptr<pointed> prev = value(owner);
				Search(owner);
				::std::weak_ptr<pointed> newone = value(owner);
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
				::std::weak_ptr<pointed> prev = value(owner);
				value(owner) = val;
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

			virtual bool getValue(std::string& val, CoreModifiable* owner) const override
			{
				const_cast<CoreModifiableAttributeMap<::std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>*>(this)->Search(owner);
				if (auto ptr = value(owner).lock())
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
			virtual bool getValue(usString& value, CoreModifiable* owner) const override
			{
				std::string result;
				if (getValue(result, owner))
				{
					value = result;
					return true;
				}
				return false;
			}

			virtual bool getValue(CMSP& val, CoreModifiable* owner) const override
			{
				const_cast<CoreModifiableAttributeMap<::std::weak_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>*>(this)->Search(owner);
				val = value(owner).lock();
				return true;
			}

		protected:

			void Search(void* instance)
			{
				if (!mSearchString.empty())
				{
					::std::weak_ptr<pointed>& val = value(instance);

					val = CoreModifiable::SearchInstance(mSearchString, (CoreModifiable*)instance);

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
		};

		template<typename pointed, bool notifOwnerT, bool isInitT, bool isReadOnlyT>
		class CoreModifiableAttributeMap<::std::shared_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT> : public CoreModifiableAttributeMapBase<::std::shared_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>
		{
			using CoreModifiableAttributeMapBase<::std::shared_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>::CoreModifiableAttributeMapBase;
			using CoreModifiableAttributeMapBase<::std::shared_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>::value;
		public:

			bool setValue(const std::string& val, CoreModifiable* owner) override
			{
				RETURN_ON_READONLY(isReadOnlyT);
				mSearchString = val;
				::std::shared_ptr<pointed> prev = value(owner);
				Search(owner);
				::std::shared_ptr<pointed> newone = value(owner);
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
				::std::shared_ptr<pointed> prev = value(owner);
				value(owner) = val;
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

			virtual bool getValue(std::string& val, CoreModifiable* owner) const override
			{
				const_cast<CoreModifiableAttributeMap<::std::shared_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>*>(this)->Search(owner);
				if (auto ptr = value(owner))
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
			virtual bool getValue(usString& value, CoreModifiable* owner) const override
			{
				std::string result;
				if (getValue(result, owner))
				{
					value = result;
					return true;
				}
				return false;
			}

			virtual bool getValue(CMSP& val, CoreModifiable* owner) const override
			{
				const_cast<CoreModifiableAttributeMap<::std::shared_ptr<pointed>, notifOwnerT, isInitT, isReadOnlyT>*>(this)->Search(owner);
				val = value(owner);
				return true;
			}

		protected:

			void Search(void* instance)
			{
				if (!mSearchString.empty())
				{
					::std::shared_ptr<pointed>& val = value(instance);

					val = CoreModifiable::SearchInstance(mSearchString, (CoreModifiable*)instance);

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

			std::string	mSearchString;
		};


		// 
		template<typename valT>
		void	CoreModifiable::addMappedAttribute(uintptr_t offset, const std::string& c, std::vector<std::pair<KigsID, CoreModifiableAttribute*>>* parent, CoreModifiableAttribute* toAdd)
		{
			// remove m
			std::string attrname = c.substr(1);
			// do placement new 
			new (toAdd) CoreModifiableAttributeMap<valT>(offset, attrname);
			parent->push_back({ attrname, toAdd });
		}
	}
}