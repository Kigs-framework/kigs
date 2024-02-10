#pragma once

#include "GenericRefCountedBaseClass.h"
#include "SmartPointer.h"
#include "usString.h"
#include "TecLibs/Tec3D.h"
#include "CoreTypes.h"

namespace Kigs
{
	namespace Core
	{
		class CoreModifiable;
		class CoreModifiableAttribute;
		class CoreItemIteratorBase;
		class CoreItemIterator;
		class CoreItem;


		// ****************************************
		// * CoreItemSP class
		// * --------------------------------------
		/**
		* \class	CoreItemSP
		* \file     CoreItem.h
		* \ingroup Core
		* \brief	SmartPointer class to manage CoreItem
		*/
		// ****************************************


		class CoreItemSP : public std::shared_ptr<CoreItem>
		{
		public:
			using std::shared_ptr<CoreItem>::shared_ptr;

			CoreItemSP(std::shared_ptr<CoreItem> p) : std::shared_ptr<CoreItem>(p)
			{

			}

			// Auto cast
			template<typename U>
			operator SmartPointer<U>()
			{
				return debug_checked_pointer_cast<U>(*this);
			}

			explicit operator CoreItem& ()
			{
				return *get();
			}

			CoreItemSP(const bool&);
			CoreItemSP(const float&);
			CoreItemSP(const double&);
			CoreItemSP(const s32&);
			CoreItemSP(const u32&);
			CoreItemSP(const s64&);
			CoreItemSP(const u64&);
			CoreItemSP(const v2f&);
			CoreItemSP(const v3f&);
			CoreItemSP(const v4f&);

			CoreItemSP(const std::string&, CoreModifiable* owner = nullptr);
			CoreItemSP(const char*, CoreModifiable* owner = nullptr);
			CoreItemSP(const usString&, CoreModifiable* owner = nullptr);


			CoreItemSP operator[](int i) const;
			CoreItemSP operator[](const char* key) const;
			CoreItemSP operator[](const std::string& key) const;
			CoreItemSP operator[](const usString& key) const;

			// NOTE(antoine) shared_ptr already has a bool operator which is commonly used in patterns like this : if(myCoreItemSP) { doSomething... }
			//operator bool() const;
			operator float() const;
			operator double() const;
			operator int() const;
			operator unsigned int() const;
			operator s64() const;
			operator u64() const;
			operator std::string() const;
			operator usString() const;
			operator v2f() const;
			operator v3f() const;
			operator v4f() const;

			template<typename T>
			inline T value() const
			{
				if (get())
					return (T)(*get());

				T noval(0);
				return noval;
			}

			CoreItemIterator begin() const;
			CoreItemIterator end() const;
		};



		class CoreItemIteratorBase : public std::enable_shared_from_this<CoreItemIteratorBase>
		{
		public:
			CoreItemIteratorBase() : mAttachedCoreItem(nullptr), mPos(0xFFFFFFFF)
			{

			}

			CoreItemIteratorBase(const CoreItemIteratorBase& other) : mAttachedCoreItem(other.mAttachedCoreItem), mPos(other.mPos)
			{

			}

			virtual CoreItemIteratorBase* clone()
			{
				CoreItemIteratorBase* result = new CoreItemIteratorBase(mAttachedCoreItem, mPos);
				return result;
			}

			virtual CoreItemSP operator*() const;

			virtual CoreItemIteratorBase& operator=(const CoreItemIteratorBase& other);

			virtual CoreItemIteratorBase& operator+(const int decal)
			{
				mPos = 0xFFFFFFFF;
				return *this;
			}

			virtual CoreItemIteratorBase& operator++()
			{
				mPos = 0xFFFFFFFF;
				return *this;
			}

			virtual CoreItemIteratorBase operator++(int)
			{
				CoreItemIteratorBase	tmp(*this);
				operator++();
				return tmp;
			}

			virtual bool operator==(const CoreItemIteratorBase& other) const
			{
				if ((mAttachedCoreItem == other.mAttachedCoreItem) &&
					(mPos == other.mPos))
				{
					return true;
				}
				return false;
			}

			virtual bool operator!=(const CoreItemIteratorBase& other) const
			{
				if ((mAttachedCoreItem != other.mAttachedCoreItem) ||
					(mPos != other.mPos))
				{
					return true;
				}
				return false;
			}

			virtual bool	getKey(std::string& returnedkey)
			{
				returnedkey = getName();
				return false;
			}

			virtual bool	getKey(usString& returnedkey)
			{
				returnedkey = getName();
				return false;
			}

			virtual std::string getName() const
			{
				return "";
			}

			virtual ~CoreItemIteratorBase()
			{

			}
		protected:

			friend class CoreItem;
			friend class CoreItemIterator;

			CoreItemIteratorBase(const CoreItemSP& item, unsigned int pos) : mAttachedCoreItem(item), mPos(pos)
			{

			}

			CoreItemSP		mAttachedCoreItem;
			unsigned int	mPos;
		};

		class CoreItemIterator : public SmartPointer<CoreItemIteratorBase>
		{
		public:

			CoreItemIterator(CoreItemIteratorBase* point) : SmartPointer<CoreItemIteratorBase>(point)
			{

			}

			virtual CoreItemSP operator*() const;

			virtual CoreItemIterator& operator=(const CoreItemIterator& other);

			virtual CoreItemIterator& operator+(const int decal);

			virtual CoreItemIterator& operator++();

			virtual CoreItemIterator operator++(int);

			virtual bool operator==(const CoreItemIterator& other) const;

			virtual bool operator!=(const CoreItemIterator& other) const;

			virtual bool	getKey(std::string& returnedkey);

			virtual bool	getKey(usString& returnedkey);

		protected:


		};

		// ****************************************
		// * CoreItem class
		// * --------------------------------------
		/**
		* \class	CoreItem
		* \file     CoreItem.h
		* \ingroup Core
		* \brief	Super class for CoreItem to retreivre CoreItem Values
		*/
		// ****************************************
		class CoreItem : public GenericRefCountedBaseClass
		{
		public:
			enum COREITEM_TYPE
			{
				UNKNOWN = 0,
				COREVECTOR = 1,
				COREMAP = 2,
				COREVALUE = 4,
			};

			virtual void set(int key, const CoreItemSP& toinsert) {};
			virtual void set(const std::string& key, const CoreItemSP& toinsert) {};
			virtual void set(const usString& key, const CoreItemSP& toinsert) {};

			virtual void erase(int key) {};
			virtual void erase(const std::string& key) {};
			virtual void erase(const usString& key) {};

			CoreItemSP SharedFromThis()
			{
#ifdef _DEBUG
				try
				{
					return debug_checked_pointer_cast<CoreItem>(shared_from_this());
				}
				catch (const std::bad_weak_ptr&)
				{
					// NOTE(antoine) : Possible reasons :
					// It's forbidden to call shared_from_this inside the constructor
					// If the object was new'ed manually and not yet assigned to a shared_ptr, consider using the Make* functions below instead, or std::make_shared/MakeRefCounted
#ifdef WIN32			
					__debugbreak();
#endif		
				}
				return nullptr;
#else
				return debug_checked_pointer_cast<CoreItem>(shared_from_this());
#endif	
			}

			typedef size_t size_type;

			virtual operator bool() const;

			virtual operator float() const;

			virtual operator double() const;

			virtual operator int() const;

			virtual operator s64() const;

			virtual operator unsigned int() const;

			virtual operator u64() const;

#ifndef _MSC_VER
			// bug in VC++ can not have explicit virtual cast operator
			explicit
#endif
				virtual operator std::string() const;

			virtual operator usString() const;
			virtual operator v2f() const;
			virtual operator v3f() const;
			virtual operator v4f() const;

			bool getValue(bool& _value) const {
				_value = (bool)*this;
				return true;
			}

			bool getValue(float& _value) const {
				_value = (float)*this;
				return true;
			}

			bool getValue(int& _value) const {
				_value = (int)*this;
				return true;
			}

			bool getValue(s64& _value) const {
				_value = (s64)*this;
				return true;
			}

			bool getValue(unsigned int& _value) const {
				_value = (unsigned int)*this;
				return true;
			}

			bool getValue(u64& _value) const {
				_value = (u64)*this;
				return true;
			}

			bool getValue(std::string& _value) const {
				_value = (std::string)*this;
				return true;
			}

			bool getValue(usString& _value) const {
				_value = (usString)*this;
				return true;
			}

			bool getValue(v2f& _value) const {
				_value = (v2f)*this;
				return true;
			}

			bool getValue(v3f& _value) const {
				_value = this->operator v3f();
				return true;
			}

			bool getValue(v4f& _value) const {
				_value = this->operator v4f();
				return true;
			}

			virtual bool operator==(const CoreItem& other) const
			{
				return false;
			}

			// empty assignement with value
			virtual CoreItem& operator=(const bool& other);
			virtual CoreItem& operator=(const float& other);
			virtual CoreItem& operator=(const int& other);
			virtual CoreItem& operator=(const s64& other);
			virtual CoreItem& operator=(const unsigned int& other);
			virtual CoreItem& operator=(const u64& other);
			virtual CoreItem& operator=(const std::string& other);
			virtual CoreItem& operator=(const usString& other);
			virtual CoreItem& operator=(const v2f& other);
			virtual CoreItem& operator=(const v3f& other);
			virtual CoreItem& operator=(const v4f& other);

			virtual std::string toString() const { return ""; }

			virtual bool empty() const
			{
				return true;
			}

			virtual size_type size() const
			{
				return 1;
			}

			virtual bool isString() const { return false; }

			virtual CoreModifiableAttribute* createAttribute()
			{
				return 0;
			}

			COREITEM_TYPE	GetType() const { return mType; }

			// operator [] needs to be overloaded on vectors and maps
			virtual CoreItemSP operator[](int i);

			CoreItemSP operator[](const char* key)
			{
				return (*this)[std::string(key)];
			}

			virtual CoreItemSP operator[](const std::string& key);

			virtual CoreItemSP operator[](const usString& key);

			// operator [] needs to be overloaded on vectors and maps

			virtual CoreItemIterator begin()
			{
				return CoreItemIterator(new CoreItemIteratorBase(SharedFromThis(), 0));
			}

			virtual CoreItemIterator end()
			{
				return CoreItemIterator(new CoreItemIteratorBase(SharedFromThis(), 0xFFFFFFFF));
			}

			virtual void* getContainerStruct() = 0;

		protected:

			CoreItem(COREITEM_TYPE _type) : mType(_type)
			{

			}

			COREITEM_TYPE	mType;
		};

#define DECLARE_MAKE_COREVALUE(type) CoreItemSP MakeCoreValue(type value);

		CoreItemSP MakeCoreMap();
		CoreItemSP MakeCoreMapUS();
		CoreItemSP MakeCoreVector();

		DECLARE_MAKE_COREVALUE(const bool&);
		DECLARE_MAKE_COREVALUE(const s32&);
		DECLARE_MAKE_COREVALUE(const u32&);
		DECLARE_MAKE_COREVALUE(const s64&);
		DECLARE_MAKE_COREVALUE(const u64&);
		DECLARE_MAKE_COREVALUE(const float&);
		DECLARE_MAKE_COREVALUE(const double&);
		DECLARE_MAKE_COREVALUE(const v2f&);
		DECLARE_MAKE_COREVALUE(const v3f&);
		DECLARE_MAKE_COREVALUE(const v4f&);

		CoreItemSP MakeCoreValue(const std::string& value, CoreModifiable* owner = nullptr);
		CoreItemSP MakeCoreValue(const usString& value, CoreModifiable* owner = nullptr);
		CoreItemSP MakeCoreValue(const char* value, CoreModifiable* owner = nullptr);

		template<typename smartPointOn, typename ... Args>
		CoreItemSP MakeCoreItemOfType(Args&& ... args)
		{
			return std::make_shared<smartPointOn>(std::forward<decltype(args)>(args)...);
		}



		// operator [] needs to be overloaded on vectors and maps
		inline CoreItemSP CoreItemSP::operator[](int i) const
		{
			if (get())
				return get()->operator[](i);

			return nullptr;
		}

		inline CoreItemSP CoreItemSP::operator[](const char* key) const
		{
			if (get())
				return get()->operator[](key);

			return nullptr;
		}

		inline CoreItemSP CoreItemSP::operator[](const std::string& key) const
		{
			if (get())
				return get()->operator[](key);

			return nullptr;
		}

		inline CoreItemSP CoreItemSP::operator[](const usString& key) const
		{
			if (get())
				return get()->operator[](key);

			return nullptr;
		}

		/*inline CoreItemSP::operator bool() const
		{
			return get()->operator bool();
		}*/

		inline CoreItemSP::operator float() const
		{
			return get()->operator float();
		}

		inline CoreItemSP::operator double() const
		{
			return get()->operator double();
		}

		inline CoreItemSP::operator int() const
		{
			return get()->operator int();
		}

		inline CoreItemSP::operator unsigned int() const
		{
			return get()->operator unsigned int();
		}

		inline CoreItemSP::operator s64() const
		{
			return get()->operator s64();
		}

		inline CoreItemSP::operator u64() const
		{
			return get()->operator u64();
		}

		inline CoreItemSP::operator std::string() const
		{
			return get()->operator std::string();
		}

		inline CoreItemSP::operator usString() const
		{
			return get()->operator usString();
		}

		inline CoreItemSP::operator v2f() const
		{
			return get()->operator v2f();
		}

		inline CoreItemSP::operator v3f() const
		{
			return get()->operator v3f();
		}

		inline CoreItemSP::operator v4f() const
		{
			return get()->operator v4f();
		}

		inline CoreItemIterator CoreItemSP::begin() const
		{
			return get()->begin();
		}

		inline CoreItemIterator CoreItemSP::end() const
		{
			return get()->end();
		}

		template<>
		inline std::string CoreItemSP::value<std::string>() const
		{
			if (get())
				return (std::string)(*get());

			std::string noval = "";
			return noval;
		}

		template<>
		inline usString CoreItemSP::value<usString>() const
		{
			if (get())
				return (usString)(*get());

			usString noval("");
			return noval;
		}

	}
}