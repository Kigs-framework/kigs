#pragma once

#include "Core.h"
#include "CoreItem.h"
#include "usString.h"

#ifdef _MSC_VER
#pragma warning( disable : 4996 )
#endif

namespace Kigs
{
	namespace Core
	{
		class CoreVector;

		// ****************************************
		// * CoreValueBase class
		// * --------------------------------------
		/**
		* \class	CoreValueBase
		* \file		CoreValue.h
		* \ingroup  Core
		* \brief	Base class for CoreValue
		*/
		// ****************************************

		template<class T, class BaseClass>
		class CoreValueBase : public BaseClass
		{
		protected:
			explicit CoreValueBase() :BaseClass(CoreItem::COREVALUE), mValue(0) {}

			CoreValueBase(CoreItem::COREITEM_TYPE _type, const T& _value) : BaseClass(_type), mValue(_value) {}
			CoreValueBase(CoreItem::COREITEM_TYPE _type) : BaseClass(_type) {}
		public:

			virtual void set(int key, const CoreItemSP& toinsert) override
			{
				KIGS_ERROR("set called on CoreValue", 1);
			}
			virtual void set(const std::string& key, const CoreItemSP& toinsert) override
			{
				KIGS_ERROR("set called on CoreValue", 1);
			}
			virtual void set(const usString& key, const CoreItemSP& toinsert) override
			{
				KIGS_ERROR("set called on CoreValue", 1);
			}

			virtual void erase(int key) override
			{
				KIGS_ERROR("erase called on CoreValue", 1);
			}
			virtual void erase(const std::string& key) override
			{
				KIGS_ERROR("erase called on CoreValue", 1);
			}
			virtual void erase(const usString& key) override
			{
				KIGS_ERROR("erase called on CoreValue", 1);
			}

			virtual inline operator bool() const override
			{
				return false;
			}

			virtual inline operator float() const override
			{
				return 0.0f;
			}

			virtual inline operator double() const override
			{
				return 0.0f;
			}

			virtual inline operator int() const override
			{
				return 0;
			}

			virtual inline operator s64() const override
			{
				return 0;
			}

			virtual inline operator unsigned int() const override
			{
				return 0;
			}

			virtual inline operator u64() const override
			{
				return 0;
			}

			virtual inline operator std::string() const override
			{
				return "";
			}

			virtual inline operator usString() const override
			{
				return usString("");
			}

			virtual inline operator Point2D() const override
			{
				Point2D result;
				return result;
			}

			virtual inline operator Point3D() const override
			{
				Point3D result;
				return result;
			}

			virtual inline operator Vector4D() const override
			{
				Vector4D result;
				return result;
			}


			virtual bool operator==(const CoreItem& other) const override
			{
				return (mValue == other.operator T());
			}

			virtual std::string toString() const override { return ""; }

			virtual bool isString() const override { return false; }

			T& getByRef()
			{
				return mValue;
			}

			T* getByPointer()
			{
				return &mValue;
			}

			operator T* ()
			{
				return &mValue;
			}

			virtual void* getContainerStruct() override
			{
				return &mValue;
			}

		protected:
			T  mValue;
		};

		template<>
		inline CoreValueBase<bool, CoreItem>::operator bool() const
		{
			return mValue;
		}

		template<>
		inline CoreValueBase<bool, CoreItem>::operator int() const
		{
			return mValue ? 1 : 0;
		}

		template<>
		inline CoreValueBase<bool, CoreItem>::operator s64() const
		{
			return mValue ? 1 : 0;
		}

		template<>
		inline CoreValueBase<bool, CoreItem>::operator unsigned int() const
		{
			return mValue ? 1 : 0;
		}

		template<>
		inline CoreValueBase<bool, CoreItem>::operator u64() const
		{
			return mValue ? 1 : 0;
		}

		template<>
		inline CoreValueBase<bool, CoreItem>::operator float() const
		{
			return mValue ? 1.0f : 0.0f;
		}

		template<>
		inline CoreValueBase<bool, CoreItem>::operator double() const
		{
			return mValue ? 1.0 : 0.0;
		}


		template<>
		inline CoreValueBase<bool, CoreItem>::operator std::string() const
		{
			return mValue ? "true" : "false";
		}



		template<>
		inline CoreValueBase<bool, CoreItem>::operator usString() const
		{
			return mValue ? usString("true") : usString("false");
		}


#define DEFINE_COREVALUE_CAST_OPERATOR(basetype,itemtype,casttype) \
template<> \
inline CoreValueBase<basetype, itemtype>::operator casttype() const \
{ \
	return (casttype)mValue; \
} 

		DEFINE_COREVALUE_CAST_OPERATOR(float, CoreItem, float);
		DEFINE_COREVALUE_CAST_OPERATOR(float, CoreItem, double);
		DEFINE_COREVALUE_CAST_OPERATOR(float, CoreItem, int);
		DEFINE_COREVALUE_CAST_OPERATOR(float, CoreItem, s64);
		DEFINE_COREVALUE_CAST_OPERATOR(float, CoreItem, unsigned int);
		DEFINE_COREVALUE_CAST_OPERATOR(float, CoreItem, u64);

		template<>
		inline CoreValueBase<float, CoreItem>::operator std::string() const
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%f", (float)mValue);
			return L_Buffer;
		}


		template<>
		inline CoreValueBase<float, CoreItem>::operator usString() const
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%f", (float)mValue);
			return usString(L_Buffer);
		}


		template<>
		inline CoreValueBase<float, CoreItem>::operator Point2D() const
		{
			Point2D _value;
			_value.x = (float)mValue;
			_value.y = (float)mValue;
			return _value;
		}

		template<>
		inline CoreValueBase<float, CoreItem>::operator Point3D()  const
		{
			Point3D _value;
			_value.x = (float)mValue;
			_value.y = (float)mValue;
			_value.z = (float)mValue;
			return _value;
		}

		template<>
		inline CoreValueBase<float, CoreItem>::operator Vector4D()  const
		{
			Vector4D _value;
			_value.x = (float)mValue;
			_value.y = (float)mValue;
			_value.z = (float)mValue;
			_value.w = (float)mValue;
			return _value;
		}

		template<>
		inline CoreValueBase<Point2D, CoreItem>::operator Point2D() const
		{
			Point2D _value;
			_value.x = mValue.x;
			_value.y = mValue.y;
			return _value;
		}

		template<>
		inline CoreValueBase<Point2D, CoreItem>::operator Point3D()  const
		{
			Point3D _value;
			_value.x = mValue.x;
			_value.y = mValue.y;
			_value.z = 0.0f;
			return _value;
		}

		template<>
		inline CoreValueBase<Point2D, CoreItem>::operator Vector4D()  const
		{
			Vector4D _value;
			_value.x = mValue.x;
			_value.y = mValue.y;
			_value.z = 0.0f;
			_value.w = 0.0f;
			return _value;
		}

		template<>
		inline CoreValueBase<Point3D, CoreItem>::operator Point2D() const
		{
			Point2D _value;
			_value.x = mValue.x;
			_value.y = mValue.y;
			return _value;
		}

		template<>
		inline CoreValueBase<Point3D, CoreItem>::operator Point3D()  const
		{
			Point3D _value;
			_value.x = mValue.x;
			_value.y = mValue.y;
			_value.z = mValue.z;
			return _value;
		}

		DEFINE_COREVALUE_CAST_OPERATOR(int, CoreItem, float);
		DEFINE_COREVALUE_CAST_OPERATOR(int, CoreItem, double);
		DEFINE_COREVALUE_CAST_OPERATOR(int, CoreItem, int);
		DEFINE_COREVALUE_CAST_OPERATOR(int, CoreItem, s64);
		DEFINE_COREVALUE_CAST_OPERATOR(int, CoreItem, unsigned int);
		DEFINE_COREVALUE_CAST_OPERATOR(int, CoreItem, u64);


		template<>
		inline CoreValueBase<int, CoreItem>::operator bool()  const
		{
			return (mValue != 0);
		}

		template<>
		inline CoreValueBase<int, CoreItem>::operator std::string() const
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%i", (int)mValue);
			return L_Buffer;
		}


		template<>
		inline CoreValueBase<int, CoreItem>::operator usString() const
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%i", (int)mValue);
			return usString(L_Buffer);
		}


		DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreItem, float);
		DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreItem, double);
		DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreItem, int);
		DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreItem, s64);
		DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreItem, unsigned int);
		DEFINE_COREVALUE_CAST_OPERATOR(s64, CoreItem, u64);

		template<>
		inline CoreValueBase<s64, CoreItem>::operator bool()  const
		{
			return (mValue != 0);
		}

		template<>
		inline CoreValueBase<s64, CoreItem>::operator std::string() const
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%lli", (s64)mValue);
			return L_Buffer;
		}


		template<>
		inline CoreValueBase<s64, CoreItem>::operator usString() const
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%lli", (s64)mValue);
			return usString(L_Buffer);
		}


		DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreItem, float);
		DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreItem, double);
		DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreItem, int);
		DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreItem, s64);
		DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreItem, unsigned int);
		DEFINE_COREVALUE_CAST_OPERATOR(unsigned int, CoreItem, u64);

		template<>
		inline CoreValueBase<unsigned int, CoreItem>::operator bool()  const
		{
			return (mValue != 0);
		}

		template<>
		inline CoreValueBase<unsigned int, CoreItem>::operator std::string() const
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%u", (unsigned int)mValue);
			return L_Buffer;
		}

		template<>
		inline CoreValueBase<unsigned int, CoreItem>::operator usString() const
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%u", (unsigned int)mValue);
			return usString(L_Buffer);
		}


		DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreItem, float);
		DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreItem, double);
		DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreItem, int);
		DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreItem, s64);
		DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreItem, unsigned int);
		DEFINE_COREVALUE_CAST_OPERATOR(u64, CoreItem, u64);


		template<>
		inline CoreValueBase<u64, CoreItem>::operator bool()  const
		{
			return (mValue != 0);
		}

		template<>
		inline CoreValueBase<u64, CoreItem>::operator std::string() const
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%llu", (u64)mValue);
			return L_Buffer;
		}

		template<>
		inline CoreValueBase<u64, CoreItem>::operator usString() const
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%llu", (u64)mValue);
			return usString(L_Buffer);
		}


		template<>
		inline CoreValueBase<std::string, CoreItem>::operator std::string() const
		{
			return mValue;
		}

		template<>
		inline CoreValueBase<std::string, CoreItem>::operator usString() const
		{
			usString tmpval;
			tmpval = mValue;
			return tmpval;
		}



		template<>
		inline CoreValueBase<usString, CoreItem>::operator std::string() const
		{
			return mValue.ToString();
		}

		template<>
		inline CoreValueBase<usString, CoreItem>::operator usString() const
		{
			return mValue;
		}

		template<>
		inline bool CoreValueBase<usString, CoreItem>::operator==(const CoreItem& other) const
		{
			usString otherval;
			other.getValue(otherval);
			return (mValue == otherval);
		}

		template<>
		inline std::string CoreValueBase<std::string, CoreItem>::toString() const
		{
			return mValue;
		}

		template<>
		inline std::string CoreValueBase<usString, CoreItem>::toString() const
		{
			return mValue.ToString();
		}

		template<>
		inline CoreValueBase<std::string, CoreItem>::operator int() const
		{
			return atoi(mValue.c_str());
		}



		template<>
		inline CoreValueBase<std::string, CoreItem>::operator float() const
		{
			return  (float)atof(mValue.c_str());
		}


		template<>
		inline CoreValueBase<std::string, CoreItem>::operator unsigned int() const
		{
			return (unsigned int)atoi(mValue.c_str());
		}


		template<>
		inline CoreValueBase<std::string, CoreItem>::operator u64() const
		{
			u64 v;
			sscanf(mValue.c_str(), "%llu", &v);
			return v;
		}

		template<>
		inline bool CoreValueBase<std::string, CoreItem>::isString() const
		{
			return true;
		}



		// usString
		template<>
		inline CoreValueBase<usString, CoreItem>::operator int() const
		{
			return atoi(mValue.ToString().c_str());
		}


		template<>
		inline CoreValueBase<usString, CoreItem>::operator float() const
		{
			return (float)atof(mValue.ToString().c_str());
		}

		template<>
		inline CoreValueBase<usString, CoreItem>::operator unsigned int() const
		{
			return (unsigned int)atoi(mValue.ToString().c_str());
		}

		template<>
		inline CoreValueBase<usString, CoreItem>::operator u64() const
		{
			u64 v;
			sscanf(mValue.ToString().c_str(), "%llu", &v);
			return v;
		}

		template<>
		inline bool CoreValueBase<usString, CoreItem>::isString() const
		{
			return true;
		}


		// ****************************************
		// * CoreValue class
		// * --------------------------------------
		/**
		* \class	CoreValue
		* \file		CoreValue.h
		* \ingroup  Core
		* \brief	CoreItem managing a single value
		*/
		// ****************************************

		template<typename T>
		class CoreValue : public CoreValueBase<T, CoreItem>
		{
		public:

			CoreValue(const T& _value) :
				CoreValueBase<T, CoreItem>(CoreItem::COREVALUE, _value)
			{
			}

			CoreValue() :
				CoreValueBase<T, CoreItem>(CoreItem::COREVALUE)
			{
			}

			virtual CoreModifiableAttribute* createAttribute(CoreModifiable*);

			/*template<typename valType>
			CoreValue& operator= (const valType& mValue)
			{
				this->mValue = mValue;
				return *this;
			}*/
			virtual CoreItem& operator=(const bool& other)
			{
				this->mValue = (T)other;
				return *this;
			}
			virtual CoreItem& operator=(const float& other)
			{
				this->mValue = (T)other;
				return *this;
			}
			virtual CoreItem& operator=(const int& other)
			{
				this->mValue = (T)other;
				return *this;
			}
			virtual CoreItem& operator=(const s64& other)
			{
				this->mValue = (T)other;
				return *this;
			}
			virtual CoreItem& operator=(const unsigned int& other)
			{
				this->mValue = (T)other;
				return *this;
			}
			virtual CoreItem& operator=(const u64& other)
			{
				this->mValue = (T)other;
				return *this;
			}
			virtual CoreItem& operator=(const std::string& other)
			{
				KIGS_WARNING("trying to assign string value to non string CoreValue", 2);
				return *this;
			}
			virtual CoreItem& operator=(const usString& other)
			{
				KIGS_WARNING("trying to assign usString value to non usString CoreValue", 2);
				return *this;
			}
			virtual CoreItem& operator=(const Point2D& other)
			{
				KIGS_WARNING("trying to assign Point2D value to non Point2D CoreValue", 2);
				return *this;
			}
			virtual CoreItem& operator=(const Point3D& other)
			{
				KIGS_WARNING("trying to assign Point3D value to non Point3D CoreValue", 2);
				return *this;
			}
			virtual CoreItem& operator=(const Vector4D& other)
			{
				KIGS_WARNING("trying to assign Point3D value to non Point3D CoreValue", 2);
				return *this;
			}

			virtual ~CoreValue()
			{}
		};

		template<>
		inline CoreItem& CoreValue<std::string>::operator= (const float& _value)
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%f", (float)_value);
			this->mValue = L_Buffer;
			return *this;
		}

		template<>
		inline CoreItem& CoreValue<std::string>::operator= (const std::string& _value)
		{
			this->mValue = _value;
			return *this;
		}


		template<>
		inline CoreItem& CoreValue<usString>::operator= (const float& _value)
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%f", (float)_value);
			this->mValue = usString(L_Buffer);
			return *this;
		}

		template<>
		inline CoreItem& CoreValue<std::string>::operator= (const int& _value)
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%i", _value);
			this->mValue = L_Buffer;
			return *this;
		}

		template<>
		inline CoreItem& CoreValue<usString>::operator= (const int& _value)
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%i", _value);
			this->mValue = usString(L_Buffer);
			return *this;
		}

		template<>
		inline CoreItem& CoreValue<std::string>::operator= (const s64& _value)
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%lli", _value);
			this->mValue = L_Buffer;
			return *this;
		}

		template<>
		inline CoreItem& CoreValue<usString>::operator= (const s64& _value)
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%lli", _value);
			this->mValue = usString(L_Buffer);
			return *this;
		}

		template<>
		inline CoreItem& CoreValue<std::string>::operator= (const unsigned int& _value)
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%i", _value);
			this->mValue = L_Buffer;
			return *this;
		}

		template<>
		inline CoreItem& CoreValue<usString>::operator= (const unsigned int& _value)
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%i", _value);
			this->mValue = usString(L_Buffer);
			return *this;
		}

		template<>
		inline CoreItem& CoreValue<std::string>::operator= (const u64& _value)
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%llu", _value);
			this->mValue = L_Buffer;
			return *this;
		}

		template<>
		inline CoreItem& CoreValue<usString>::operator= (const u64& _value)
		{
			char L_Buffer[64] = { 0 };
			snprintf(L_Buffer, 64, "%llu", _value);
			this->mValue = usString(L_Buffer);
			return *this;
		}


		template<>
		inline CoreItem& CoreValue<std::string>::operator= (const bool& _value)
		{
			if (_value)
				this->mValue = "true";
			else
				this->mValue = "false";
			return *this;
		}

		template<>
		inline CoreItem& CoreValue<usString>::operator= (const bool& _value)
		{
			if (_value)
				this->mValue = usString("true");
			else
				this->mValue = usString("false");
			return *this;
			return *this;
		}

		template<>
		inline CoreItem& CoreValue<Point3D>::operator= (const bool& _value)
		{
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Point3D>::operator= (const float& _value)
		{
			this->mValue.x = this->mValue.y = this->mValue.z = _value;
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Point3D>::operator= (const int& _value)
		{
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Point3D>::operator= (const s64& _value)
		{
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Point3D>::operator= (const unsigned int& _value)
		{
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Point3D>::operator= (const u64& _value)
		{
			return *this;
		}


		template<>
		inline CoreItem& CoreValue<Point3D>::operator=(const Point2D& other)
		{
			this->mValue.x = other.x;
			this->mValue.y = other.y;

			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Point3D>::operator=(const Point3D& other)
		{
			this->mValue = other;
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Point3D>::operator=(const Vector4D& other)
		{
			this->mValue.x = other.x;
			this->mValue.y = other.y;
			this->mValue.z = other.z;

			return *this;
		}

		template<>
		inline CoreItem& CoreValue<Point2D>::operator= (const bool& _value)
		{
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Point2D>::operator= (const float& _value)
		{
			this->mValue.x = this->mValue.y = _value;
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Point2D>::operator= (const int& _value)
		{
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Point2D>::operator= (const s64& _value)
		{
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Point2D>::operator= (const unsigned int& _value)
		{
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Point2D>::operator= (const u64& _value)
		{
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Point2D>::operator=(const Point2D& other)
		{
			this->mValue = other;
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Point2D>::operator=(const Point3D& other)
		{
			this->mValue.x = other.x;
			this->mValue.y = other.y;
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Point2D>::operator=(const Vector4D& other)
		{
			this->mValue.x = other.x;
			this->mValue.y = other.y;
			return *this;
		}

		template<>
		inline CoreItem& CoreValue<Vector4D>::operator= (const bool& _value)
		{
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Vector4D>::operator= (const float& _value)
		{
			this->mValue.x = this->mValue.y = this->mValue.z = this->mValue.w = _value;
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Vector4D>::operator= (const int& _value)
		{
			this->mValue.x = this->mValue.y = this->mValue.z = this->mValue.w = _value;
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Vector4D>::operator= (const s64& _value)
		{
			this->mValue.x = this->mValue.y = this->mValue.z = this->mValue.w = _value;
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Vector4D>::operator= (const unsigned int& _value)
		{
			this->mValue.x = this->mValue.y = this->mValue.z = this->mValue.w = _value;
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Vector4D>::operator= (const u64& _value)
		{
			this->mValue.x = this->mValue.y = this->mValue.z = this->mValue.w = _value;
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Vector4D>::operator=(const Point2D& other)
		{
			this->mValue.x = other.x;
			this->mValue.y = other.y;

			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Vector4D>::operator=(const Point3D& other)
		{
			this->mValue.x = other.x;
			this->mValue.y = other.y;
			this->mValue.z = other.z;
			return *this;
		}
		template<>
		inline CoreItem& CoreValue<Vector4D>::operator=(const Vector4D& other)
		{
			this->mValue = other;
			return *this;
		}

	}
}