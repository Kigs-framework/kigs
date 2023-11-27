#pragma once

#include "CoreModifiableAttribute.h"
#include "Core.h"

namespace Kigs
{
	namespace Core
	{

		// ****************************************
		// * maArrayBase class
		// * --------------------------------------
		/**
		* \class	maArrayBase
		* \ingroup CoreModifiableAttibute
		* \brief	CoreModifiableAttributeData for array
		*/
		// ****************************************


		// ****************************************
		// * maArrayHeritage class
		// * --------------------------------------
		/**
		* \class	maArrayHeritage
		* \ingroup CoreModifiableAttibute
		* \brief	CoreModifiableAttributeData for array with different level of notification
		*/
		// ****************************************

		template<bool notificationLevel, typename T, CoreModifiable::ATTRIBUTE_TYPE attributeElementType, size_t nbLines, size_t nbColumns, bool isInitT = false, bool isReadOnlyT = false, bool isOrphanT = false>
		class maArrayHeritage : public CoreModifiableAttributeData<std::array<T, nbLines* nbColumns>, notificationLevel, isInitT, isReadOnlyT, isOrphanT>
		{
			template<bool notiflevel, bool isInitTe, bool isReadOnlyTe, bool isOrphanTe>
			using TemplateForPlacementNew = maArrayHeritage<notiflevel, T, attributeElementType, nbLines, nbColumns, isInitTe, isReadOnlyTe, isOrphanTe>;


		public:
			using ArrayType = std::array<T, nbLines* nbColumns>;
			using CoreModifiableAttributeData<std::array<T, nbLines* nbColumns>, notificationLevel, isInitT, isReadOnlyT, isOrphanT>::setArrayValue;

			DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maArrayHeritage, TemplateForPlacementNew, ArrayType, CoreModifiable::ATTRIBUTE_TYPE::ARRAY);


		public:

			maArrayHeritage(CoreModifiable& owner, KigsID  ID, T* vals)
			: CoreModifiableAttributeData<ArrayType, notificationLevel, isInitT, isReadOnlyT, isOrphanT>(owner, ID)
			{
				// Should not use serarrayvalue here
				setArrayValue(vals, &owner, nbLines * nbColumns);
			}

			maArrayHeritage(KigsID  ID, T* vals)
			: CoreModifiableAttributeData<ArrayType, notificationLevel, isInitT, isReadOnlyT, isOrphanT>(ID, {})
			{
				// Should not use serarrayvalue here
				setArrayValue(vals, nullptr, nbLines * nbColumns);
			}

			maArrayHeritage(CoreModifiable& owner, KigsID  ID, T val0, T val1)
			: CoreModifiableAttributeData<ArrayType, notificationLevel, isInitT, isReadOnlyT, isOrphanT>(owner, ID)
			{
				assert(nbColumns >= 2);
				this->at(0, 0) = val0;
				this->at(0, 1) = val1;
			}
			maArrayHeritage(CoreModifiable& owner, KigsID  ID, T val0, T val1, T val2)
			: CoreModifiableAttributeData<ArrayType, notificationLevel, isInitT, isReadOnlyT, isOrphanT>(owner, ID)
			{
				assert(nbColumns >= 3);
				this->at(0, 0) = val0;
				this->at(0, 1) = val1;
				this->at(0, 2) = val2;
			}
			maArrayHeritage(CoreModifiable& owner, KigsID  ID, T val0, T val1, T val2, T val3)
			: CoreModifiableAttributeData<ArrayType, notificationLevel, isInitT, isReadOnlyT, isOrphanT>(owner, ID)
			{
				assert(nbColumns >= 4);
				this->at(0, 0) = val0;
				this->at(0, 1) = val1;
				this->at(0, 2) = val2;
				this->at(0, 3) = val3;
			}


			virtual bool CopyAttribute(const CoreModifiableAttribute& attribute) override
			{
				if (CoreModifiableAttributeData<ArrayType, notificationLevel, isInitT, isReadOnlyT, isOrphanT>::CopyAttribute(attribute)) return true;
				if ((attribute.getNbArrayElements() == getNbArrayElements()) && (attribute.getArrayElementType() == getArrayElementType()))
				{
					// TODO : don't use getArrayValue here
					//attribute.getArrayValue(getArrayBuffer(), getNbArrayElements());
					return true;
				}
				return false;
			}

			T& operator[](size_t index) { return this->getElement(0, index); }
			const T& operator[](size_t index) const { return this->getConstElement(0, index); }

			size_t getNbElements() const { return this->getNbArrayElements(); }

			T* getVector() { return mValue.data(); }
			const T* getConstVector() const { return mValue.data(); }

			const T& at(size_t line, size_t column) const { return mValue[line * nbColumns + column]; }
			T& at(size_t line, size_t column) { return mValue[line * nbColumns + column]; }

			virtual size_t getNbArrayElements() const override { return nbLines * nbColumns; }
			virtual size_t getNbArrayColumns() const override { return nbColumns; }
			virtual size_t getNbArrayLines() const override { return nbLines; }

			//@Refactor identical
			T* getArrayBuffer() { return  mValue.data(); }
			T* getArray() { return mValue.data(); }

			//@Refactor identical
			const T* getConstArrayBuffer() const { return  mValue.data(); }
			const T* getConstArray() const { return mValue.data(); }

			T& getElement(size_t line, size_t column) { return at(line, column); }
			const T& getConstElement(size_t line, size_t column) const { return at(line, column); }

			virtual CoreModifiable::ATTRIBUTE_TYPE getArrayElementType() const override { return attributeElementType; }

			//irtual size_t	size() const override { return nbLines * nbColumns; };

			using CoreModifiableAttributeData<ArrayType, notificationLevel, isInitT, isReadOnlyT, isOrphanT>::setValue;

			// direct access operators
			auto& operator=(v2f pt) {
				if (nbColumns < 2) return *this;
				at(0, 0) = (T)pt.x;
				at(0, 1) = (T)pt.y;
				return *this;
			}
			auto& operator=(p3f pt)
			{
				if (nbColumns < 3) return *this;
				at(0, 0) = (T)pt.x;
				at(0, 1) = (T)pt.y;
				at(0, 2) = (T)pt.z;
				return *this;
			}

			auto& operator=(v4f pt) {
				if (nbColumns < 4) return *this;
				at(0, 0) = (T)pt.x;
				at(0, 1) = (T)pt.y;
				at(0, 2) = (T)pt.z;
				at(0, 3) = (T)pt.w;
				return *this;
			}			

			virtual operator v2i() const
			{
				v2i tmpValue(0, 0);
				if ((nbLines == 1) && (nbColumns >= 2))
				{
					tmpValue.x = (s32)this->at(0, 0);
					tmpValue.y = (s32)this->at(0, 1);
				}
				return tmpValue;
			}

			virtual operator v3i() const
			{
				v3i tmpValue(0, 0, 0);
				if ((nbLines == 1) && (nbColumns >= 3))
				{
					tmpValue.x = (s32)this->at(0, 0);
					tmpValue.y = (s32)this->at(0, 1);
					tmpValue.z = (s32)this->at(0, 2);
				}
				return tmpValue;
			}

			virtual operator v2f() const
			{
				v2f tmpValue(0.0f, 0.0f);
				if ((nbLines == 1) && (nbColumns >= 2))
				{
					tmpValue.x = (float)this->at(0, 0);
					tmpValue.y = (float)this->at(0, 1);
				}
				return tmpValue;
			}

			virtual operator p3f() const
			{
				p3f tmpValue(0.0f, 0.0f, 0.0f);
				if ((nbLines == 1) && (nbColumns >= 3))
				{
					tmpValue.x = (float)this->at(0, 0);
					tmpValue.y = (float)this->at(0, 1);
					tmpValue.z = (float)this->at(0, 2);
				}
				return tmpValue;
			}

			virtual operator v4f() const
			{
				v4f tmpValue(0.0f, 0.0f, 0.0f, 0.0f);
				if ((nbLines == 1) && (nbColumns >= 4))
				{
					tmpValue.x = (float)this->at(0, 0);
					tmpValue.y = (float)this->at(0, 1);
					tmpValue.z = (float)this->at(0, 2);
					tmpValue.w = (float)this->at(0, 3);
				}
				return tmpValue;
			}
		};

		using maMatrix22DF = maArrayHeritage<false, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 2, 2,false,false,false>;
		using maMatrix22DFOrphan = maArrayHeritage<false, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 2, 2, false, false, true>;
		using maMatrix22DFInit = maArrayHeritage<false, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 2, 2,true>;
		using maMatrix33DF = maArrayHeritage<false, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 3, 3,false,false,false>;
		using maMatrix33DFOrphan = maArrayHeritage<false, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 3, 3, false, false, true>;
		using maMatrix33DFInit = maArrayHeritage<false, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 3, 3, true>;

		using maVect2DF = maArrayHeritage<false, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 1, 2,false,false,false>;
		using maVect2DFOrphan = maArrayHeritage<false, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 1, 2, false, false, true>;
		using maVect2DFInit = maArrayHeritage<false, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 1, 2, true>;
		using maVect3DF = maArrayHeritage<false, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 1, 3,false,false,false>;
		using maVect3DFOrphan = maArrayHeritage<false, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 1, 3, false, false,  true>;
		using maVect3DFInit = maArrayHeritage<false, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 1, 3, true>;
		using maVect4DF = maArrayHeritage<false, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 1, 4,false,false,false>;
		using maVect4DFOrphan = maArrayHeritage<false, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 1, 4, false, false,  true>;
		using maVect4DFInit = maArrayHeritage<false, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 1, 4, true>;
		using maVect16DF = maArrayHeritage<false, float, CoreModifiable::ATTRIBUTE_TYPE::FLOAT, 1, 16,false,false,false>;

		using maVect2DI = maArrayHeritage<false, s32, CoreModifiable::ATTRIBUTE_TYPE::INT, 1, 2,false,false,false>;
		using maVect2DIOrphan = maArrayHeritage<false, s32, CoreModifiable::ATTRIBUTE_TYPE::INT, 1, 2, false, false,  true>;
		using maVect2DIInit = maArrayHeritage<false, s32, CoreModifiable::ATTRIBUTE_TYPE::INT, 1, 2, true>;
		using maVect3DI = maArrayHeritage<false, s32, CoreModifiable::ATTRIBUTE_TYPE::INT, 1, 3,false,false,false>;
		using maVect3DIOrphan = maArrayHeritage<false, s32, CoreModifiable::ATTRIBUTE_TYPE::INT, 1, 3, false, false,  true>;
		using maVect3DIInit = maArrayHeritage<false, s32, CoreModifiable::ATTRIBUTE_TYPE::INT, 1, 3, true>;


		template<typename element_type, CoreModifiable::ATTRIBUTE_TYPE attribute_type, s32 nbElements>
		using maVector = maArrayHeritage<false, element_type, attribute_type, 1, nbElements,false,false,false>;


	}
}