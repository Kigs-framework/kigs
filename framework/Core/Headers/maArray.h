#pragma once

#include "CoreModifiableAttribute.h"
#include "Core.h"

namespace Kigs
{
	namespace Core
	{
		/*
			Conversions
		*/

		// set array values from braced string like : {1.0,0.0,2.0}
		template<typename T> bool CoreConvertString2Array(const std::string& stringValue, T* const arrayValue, u32 arrayNbElements)
		{
			std::string::size_type posToParse = stringValue.find('{', 0);
			bool hasBraces = (posToParse == 0);
			if (hasBraces) posToParse++;
			for (u32 i = 0; i < arrayNbElements; i++)
			{
				if (!hasBraces) posToParse = 0;
				// End of string, return
				if (posToParse >= stringValue.size()) return false;

				// search next separator : ',' or '}'
				std::string::size_type nextPosToParse = 0;
				nextPosToParse = stringValue.find(',', posToParse);

				if (nextPosToParse == std::string::npos)
				{
					nextPosToParse = stringValue.find('}', posToParse);
				}

				if (nextPosToParse == std::string::npos) // next separator not found, go to the end of the string
				{
					nextPosToParse = stringValue.size();
				}
				std::string stringToConvert;
				stringToConvert.assign(stringValue, posToParse, nextPosToParse - posToParse);
				if (!CoreConvertString2Value<T>(stringToConvert, arrayValue[i])) return false;
				posToParse = nextPosToParse + 1;
			}
			return true;
		}

		template<typename T> bool CoreConvertArray2String(std::string& stringValue, const T* arrayValue, u32 arrayNbElements)
		{
			if (arrayNbElements)
			{
				stringValue = "{";
				for (u32 i = 0; i < arrayNbElements; i++)
				{
					if (i != 0) stringValue += ",";
					std::string value;
					if (!CoreConvertValue2String<T>(value, arrayValue[i])) return false;
					stringValue += value;
				}
				stringValue += "}";
				return true;
			}
			return false;
		}


		template<typename T> void CoreCopyArray(T* const destArray, const T* srcArray, u32 arrayNbElements)
		{
			memcpy(destArray, srcArray, arrayNbElements * sizeof(T));
		}

		// specialization for string
		template<>
		inline void CoreCopyArray<std::string>(std::string* const destArray, const std::string* srcArray, u32 arrayNbElements)
		{
			for (u32 i = 0; i < arrayNbElements; i++) destArray[i] = srcArray[i];
		}

		template<typename Tdest, typename Tsrc>
		void CoreCopyCastArray(Tdest* const destArray, const Tsrc* srcArray, u32 arrayNbElements)
		{
			for (u32 elem = 0; elem < arrayNbElements; elem++)
				destArray[elem] = (Tdest)srcArray[elem];

		}

		// specialized
		template<>
		inline void CoreCopyCastArray<bool, float>(bool* const destArray, const float* srcArray, u32 arrayNbElements)
		{
			for (u32 elem = 0; elem < arrayNbElements; elem++)
				destArray[elem] = (srcArray[elem] != (float)0);

		}

		template<>
		inline void CoreCopyCastArray<bool, s32>(bool* const destArray, const s32* srcArray, u32 arrayNbElements)
		{
			for (u32 elem = 0; elem < arrayNbElements; elem++)
				destArray[elem] = (srcArray[elem] != (s32)0);

		}

		template<>
		inline void CoreCopyCastArray<float, bool>(float* const destArray, const bool* srcArray, u32 arrayNbElements)
		{
			for (u32 elem = 0; elem < arrayNbElements; elem++)
				destArray[elem] = (float)(srcArray[elem] ? 1 : 0);

		}

		template<>
		inline void CoreCopyCastArray<s32, bool>(s32* const destArray, const bool* srcArray, u32 arrayNbElements)
		{
			for (u32 elem = 0; elem < arrayNbElements; elem++)
				destArray[elem] = (s32)(srcArray[elem] ? 1 : 0);

		}


		// ****************************************
		// * maArrayBase class
		// * --------------------------------------
		/**
		* \class	maArrayBase
		* \ingroup CoreModifiableAttibute
		* \brief	CoreModifiableAttributeData for array
		*/
		// ****************************************

#ifndef COMMA
#define COMMA ,
#endif


// ****************************************
// * maArrayHeritage class
// * --------------------------------------
/**
* \class	maArrayHeritage
* \ingroup CoreModifiableAttibute
* \brief	CoreModifiableAttributeData for array with different level of notification
*/
// ****************************************

		template<s32 notificationLevel, typename T, CoreModifiable::ATTRIBUTE_TYPE attributeElementType, u32 nbLines, u32 nbColumns>  class maArrayHeritage : public CoreModifiableAttributeData<std::array<T, nbLines* nbColumns>>
		{
			template<s32 notiflevel>
			using TemplateForPlacementNew = maArrayHeritage<notiflevel, T, attributeElementType, nbLines, nbColumns>;

		public:
			using ArrayType = std::array<T, nbLines* nbColumns>;

			DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maArrayHeritage, TemplateForPlacementNew, ArrayType, CoreModifiable::ATTRIBUTE_TYPE::ARRAY);


		public:

			maArrayHeritage(CoreModifiable& owner, bool isInitAttribute, KigsID  ID, T* vals)
				: CoreModifiableAttributeData<ArrayType>(owner, isInitAttribute, ID)
			{
				setArrayValue(vals, nbLines * nbColumns);
			}

			maArrayHeritage(CoreModifiable& owner, bool isInitAttribute, KigsID  ID, T val0, T val1)
				: CoreModifiableAttributeData<ArrayType>(owner, isInitAttribute, ID)
			{
				KIGS_ASSERT(nbColumns >= 2);
				this->at(0, 0) = val0;
				this->at(0, 1) = val1;
			}
			maArrayHeritage(CoreModifiable& owner, bool isInitAttribute, KigsID  ID, T val0, T val1, T val2)
				: CoreModifiableAttributeData<ArrayType>(owner, isInitAttribute, ID)
			{
				KIGS_ASSERT(nbColumns >= 3);
				this->at(0, 0) = val0;
				this->at(0, 1) = val1;
				this->at(0, 2) = val2;
			}
			maArrayHeritage(CoreModifiable& owner, bool isInitAttribute, KigsID  ID, T val0, T val1, T val2, T val3)
				: CoreModifiableAttributeData<ArrayType>(owner, isInitAttribute, ID)
			{
				KIGS_ASSERT(nbColumns >= 4);
				this->at(0, 0) = val0;
				this->at(0, 1) = val1;
				this->at(0, 2) = val2;
				this->at(0, 3) = val3;
			}


			virtual bool CopyAttribute(const CoreModifiableAttribute& attribute) override
			{
				if (CoreModifiableAttributeData<ArrayType>::CopyAttribute(attribute)) return true;
				if ((attribute.getNbArrayElements() == getNbArrayElements()) && (attribute.getArrayElementType() == getArrayElementType()))
				{
					attribute.getArrayValue(getArrayBuffer(), getNbArrayElements());
					return true;
				}
				return false;
			}



			T& operator[](u32 index) { return this->getElement(0, index); }
			const T& operator[](u32 index) const { return this->getConstElement(0, index); }

			u32 getNbElements() const { return this->getNbArrayElements(); }

			T* getVector() { return mValue.data(); }
			const T* getConstVector() const { return mValue.data(); }


			const T& at(size_t line, size_t column) const { return mValue[line * nbColumns + column]; }
			T& at(size_t line, size_t column) { return mValue[line * nbColumns + column]; }

			virtual u32 getNbArrayElements() const override { return nbLines * nbColumns; }
			virtual u32 getNbArrayColumns() const override { return nbColumns; }
			virtual u32 getNbArrayLines() const override { return nbLines; }

			//@Refactor identical
			T* getArrayBuffer() { return  mValue.data(); }
			T* getArray() { return mValue.data(); }

			//@Refactor identical
			const T* getConstArrayBuffer() const { return  mValue.data(); }
			const T* getConstArray() const { return mValue.data(); }

			T& getElement(u32 line, u32 column) { return at(line, column); }
			const T& getConstElement(u32 line, u32 column) const { return at(line, column); }

			//@Issue this is wrong, is return the line not column like the method name implies
			/*
			const T* getConstColumn(u32 line) const { return mValue[line]; }
			T* getColumn(u32 line) { return mValue[line]; }
			*/

			virtual CoreModifiable::ATTRIBUTE_TYPE getArrayElementType() const override { return attributeElementType; }

			void broadcastValue(const T& v)
			{
				for (auto& el : mValue)
				{
					el = v;
				}
			}

			virtual s32	size() const override { return nbLines * nbColumns; };

			virtual bool getValue(void*& value) const override { value = (void*)mValue.data(); return true; }

			virtual bool getValue(bool& value) const override { T tmpValue = at(0, 0);  value = (tmpValue != (T)0); return true; }
			virtual bool getValue(s8& value) const override { T tmpValue = at(0, 0);  value = (s8)tmpValue; return true; }
			virtual bool getValue(s16& value) const override { T tmpValue = at(0, 0);  value = (s16)tmpValue; return true; }
			virtual bool getValue(s32& value) const override { T tmpValue = at(0, 0);  value = (s32)tmpValue; return true; }
			virtual bool getValue(s64& value) const override { T tmpValue = at(0, 0);  value = (s64)tmpValue; return true; }
			virtual bool getValue(u8& value) const override { T tmpValue = at(0, 0);  value = (u8)tmpValue; return true; }
			virtual bool getValue(u16& value) const override { T tmpValue = at(0, 0);  value = (u16)tmpValue; return true; }
			virtual bool getValue(u32& value) const override { T tmpValue = at(0, 0);  value = (u32)tmpValue; return true; }
			virtual bool getValue(u64& value) const override { T tmpValue = at(0, 0);  value = (u64)tmpValue; return true; }
			virtual bool getValue(float& value) const override { T tmpValue = at(0, 0);  value = (float)tmpValue; return true; }
			virtual bool getValue(double& value) const override { T tmpValue = at(0, 0);  value = (double)tmpValue; return true; }

			virtual bool getValue(std::string& value) const override { return CoreConvertArray2String<T>(value, getConstArrayBuffer(), getNbArrayElements()); }

			virtual bool getValue(Point2D& value) const override { if (nbColumns < 2) return false; Point2D tmpValue((float)at(0, 0), (float)at(0, 1));  value = tmpValue; return true; }
			virtual bool getValue(Point3D& value) const override { if (nbColumns < 3) return false; Point3D tmpValue((float)at(0, 0), (float)at(0, 1), (float)at(0, 2));  value = tmpValue; return true; }
			virtual bool getValue(Vector4D& value) const override { if (nbColumns < 4) return false; Vector4D tmpValue((float)at(0, 0), (float)at(0, 1), (float)at(0, 2), (float)at(0, 3));  value = tmpValue; return true; }

			using CoreModifiableAttributeData<ArrayType>::setValue;

			bool setValue(const Point2D& pt) override
			{
				if (nbColumns < 2) return false;
				at(0, 0) = (T)pt.x;
				at(0, 1) = (T)pt.y;
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
			bool setValue(const Point3D& pt) override
			{
				if (nbColumns < 3) return false;
				at(0, 0) = (T)pt.x;
				at(0, 1) = (T)pt.y;
				at(0, 2) = (T)pt.z;
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
			bool setValue(const Vector4D& pt) override
			{
				if (nbColumns < 4) return false;
				at(0, 0) = (T)pt.x;
				at(0, 1) = (T)pt.y;
				at(0, 2) = (T)pt.z;
				at(0, 3) = (T)pt.w;
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
			bool setValue(Point2DI pt)
			{
				if (nbColumns < 2) return false;
				at(0, 0) = (T)pt.x;
				at(0, 1) = (T)pt.y;
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
			bool setValue(Point3DI pt)
			{
				if (nbColumns < 3) return false;
				at(0, 0) = (T)pt.x;
				at(0, 1) = (T)pt.y;
				at(0, 2) = (T)pt.z;
				DO_NOTIFICATION(notificationLevel);
				return true;
			}


			// direct access operators
			auto& operator=(Point2D pt) {
				if (nbColumns < 2) return *this;
				at(0, 0) = (T)pt.x;
				at(0, 1) = (T)pt.y;
				return *this;
			}
			auto& operator=(Point3D pt)
			{
				if (nbColumns < 3) return *this;
				at(0, 0) = (T)pt.x;
				at(0, 1) = (T)pt.y;
				at(0, 2) = (T)pt.z;
				return *this;
			}

			auto& operator=(Vector4D pt) {
				if (nbColumns < 4) return *this;
				at(0, 0) = (T)pt.x;
				at(0, 1) = (T)pt.y;
				at(0, 2) = (T)pt.z;
				at(0, 3) = (T)pt.w;
				return *this;
			}

			auto& operator=(Point2DI pt) { setValue(pt); return *this; }
			auto& operator=(Point3DI pt) { setValue(pt); return *this; }



#define DECLARE_SET_VALUE_BROADCAST(type) virtual bool setValue(type value) override { if (isReadOnly()) { return false; } this->broadcastValue((T)value); DO_NOTIFICATION(notificationLevel); return true; }

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SET_VALUE_BROADCAST);

#undef DECLARE_SET_VALUE_BROADCAST

			virtual bool setValue(const char* value) override { std::string localstr(value); return setValue(localstr); }
			virtual bool setValue(const std::string& value) override
			{
				if (isReadOnly()) { return false; }
				if (CoreConvertString2Array<T>(value, getArrayBuffer(), getNbArrayElements()))
				{
					DO_NOTIFICATION(notificationLevel);
					return true;
				}
				return false;
			}

			virtual operator Point2DI() const
			{
				Point2DI tmpValue(0, 0);
				if ((nbLines == 1) && (nbColumns >= 2))
				{
					tmpValue.x = (s32)this->at(0, 0);
					tmpValue.y = (s32)this->at(0, 1);
				}
				return tmpValue;
			}

			virtual operator Point3DI() const
			{
				Point3DI tmpValue(0, 0, 0);
				if ((nbLines == 1) && (nbColumns >= 3))
				{
					tmpValue.x = (s32)this->at(0, 0);
					tmpValue.y = (s32)this->at(0, 1);
					tmpValue.z = (s32)this->at(0, 2);
				}
				return tmpValue;
			}

			virtual operator Point2D() const
			{
				Point2D tmpValue(0.0f, 0.0f);
				if ((nbLines == 1) && (nbColumns >= 2))
				{
					tmpValue.x = (float)this->at(0, 0);
					tmpValue.y = (float)this->at(0, 1);
				}
				return tmpValue;
			}

			virtual operator Point3D() const
			{
				Point3D tmpValue(0.0f, 0.0f, 0.0f);
				if ((nbLines == 1) && (nbColumns >= 3))
				{
					tmpValue.x = (float)this->at(0, 0);
					tmpValue.y = (float)this->at(0, 1);
					tmpValue.z = (float)this->at(0, 2);
				}
				return tmpValue;
			}

			virtual operator Vector4D() const
			{
				Vector4D tmpValue(0.0f, 0.0f, 0.0f, 0.0f);
				if ((nbLines == 1) && (nbColumns >= 4))
				{
					tmpValue.x = (float)this->at(0, 0);
					tmpValue.y = (float)this->at(0, 1);
					tmpValue.z = (float)this->at(0, 2);
					tmpValue.w = (float)this->at(0, 3);
				}
				return tmpValue;
			}


#define DECLARE_SETARRAYVALUE( b )	virtual bool setArrayValue(const b *value, u32 nbElements ) override {if(isReadOnly()){ return false; }\
		if(nbElements>getNbArrayElements()){ return false;} \
		CoreCopyCastArray<T,b>(getArrayBuffer(), value, nbElements); \
		DO_NOTIFICATION(notificationLevel);\
		return true;}

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SETARRAYVALUE);


#define DECLARE_SETARRAYELEMENTVALUE( b )	virtual bool setArrayElementValue(b value, u32 line, u32 column ) override {if(isReadOnly()){ return false; }\
		if(line>=nbLines || column>=nbColumns){ return false;} \
		at(line,column)=(T)value; \
		DO_NOTIFICATION(notificationLevel); \
		return true;}

			EXPAND_MACRO_FOR_NUMERIC_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_SETARRAYELEMENTVALUE);


#undef DECLARE_SETARRAYVALUE
#undef DECLARE_SETARRAYELEMENTVALUE

			virtual bool setArrayElementValue(bool value, u32 line, u32 column) override
			{
				if (isReadOnly()) { return false; }
				if (line >= nbLines || column >= nbColumns) { return false; }
				at(line, column) = (T)(value ? 1 : 0);
				DO_NOTIFICATION(notificationLevel);;
				return true;
			}

			virtual bool setArrayElementValue(const std::string& value, u32 line, u32 column) override
			{
				if (isReadOnly()) { return false; }
				if (line >= nbLines || column >= nbColumns) return false;
				if (CoreConvertString2Value<T>(value, this->at(line, column)))
				{
					DO_NOTIFICATION(notificationLevel);
					return true;
				}
				return false;
			}





#define DECLARE_GETARRAYVALUE( b )	virtual bool getArrayValue(b * const value, u32 nbElements ) const override {if(nbElements>getNbArrayElements()){ return false;} \
		CoreCopyCastArray<b,T>(value,getConstArrayBuffer(),nbElements); return true;}


			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_GETARRAYVALUE);


#undef DECLARE_GETARRAYVALUE


#define DECLARE_GETARRAYELEMENTVALUE( b )	virtual bool getArrayElementValue(b& value, u32 line, u32 column ) const override { if(line>=nbLines || column>=nbColumns){ return false;} \
		value=(b)at(line,column); return true;}

			EXPAND_MACRO_FOR_NUMERIC_TYPES(NOQUALIFIER, NOQUALIFIER, DECLARE_GETARRAYELEMENTVALUE);


#undef DECLARE_GETARRAYELEMENTVALUE

			virtual bool getArrayElementValue(bool& value, u32 line, u32 column) const override
			{
				if (line >= nbLines || column >= nbColumns) return false;
				value = (at(line, column) != (T)0);
				return true;
			}

			virtual bool getArrayElementValue(std::string& value, u32 line, u32 column) const override
			{
				if (line >= nbLines || column >= nbColumns) return false;
				return CoreConvertValue2String<T>(value, at(line, column));
			}




		};

		using maMatrix22DF = maArrayHeritage<0, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 2, 2>;
		using maMatrix33DF = maArrayHeritage<0, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 3, 3>;

		using maVect2DF = maArrayHeritage<0, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 1, 2>;
		using maVect3DF = maArrayHeritage<0, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 1, 3>;
		using maVect4DF = maArrayHeritage<0, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 1, 4>;
		using maVect16DF = maArrayHeritage<0, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 1, 16>;

		using maVect2DI = maArrayHeritage<0, s32, CoreModifiable::ATTRIBUTE_TYPE::INT, 1, 2>;
		using maVect3DI = maArrayHeritage<0, s32, CoreModifiable::ATTRIBUTE_TYPE::INT, 1, 3>;



		template<typename element_type, CoreModifiable::ATTRIBUTE_TYPE attribute_type, s32 nbElements>
		using maVector = maArrayHeritage<0, element_type, attribute_type, 1, nbElements>;

	}
}