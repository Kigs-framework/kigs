#pragma once

#include "CoreModifiableAttribute.h"
#include <array>

namespace Kigs
{
	namespace Core
	{
		// ****************************************
		// * maEnumBase class
		// * --------------------------------------
		/**
		* \class	maEnumBase
		* \ingroup CoreModifiableAttibute
		* \brief	CoreModifiableAttributeData for an enum
		*/
		// ****************************************

		template<unsigned int N>
		struct maEnumValue
		{
			int current_value = 0;
			std::array<std::string, N> value_list;
		};


		// ****************************************
		// * maEnumHeritage class
		// * --------------------------------------
		/**
		* \class	maEnumHeritage
		* \ingroup CoreModifiableAttibute
		* \brief	CoreModifiableAttributeData for an enum with different level of notification
		*/
		// ****************************************
		template<int notificationLevel, unsigned int nbElements>
		class maEnumHeritage : public CoreModifiableAttributeData<maEnumValue<nbElements>>
		{
			template<int notiflevel>
			using TemplateForPlacementNew = maEnumHeritage<notiflevel, nbElements>;

			DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maEnumHeritage, TemplateForPlacementNew, maEnumValue<nbElements>, CoreModifiable::ATTRIBUTE_TYPE::ENUM);

		public:



			maEnumHeritage(CoreModifiable& owner, bool isInitAttribute, KigsID ID, std::string val0, std::string val1, std::string val2 = "", std::string val3 = "", std::string val4 = "", std::string val5 = "", std::string val6 = "", std::string val7 = "", std::string val8 = "", std::string val9 = "")
				: CoreModifiableAttributeData<maEnumValue<nbElements>>(owner, isInitAttribute, ID)
			{
				//@TODO variadic template constructor?
				// Copy and move idiom
				if (nbElements > 0) mValue.value_list[0] = std::move(val0);
				if (nbElements > 1) mValue.value_list[1] = std::move(val1);
				if (nbElements > 2) mValue.value_list[2] = std::move(val2);
				if (nbElements > 3) mValue.value_list[3] = std::move(val3);
				if (nbElements > 4) mValue.value_list[4] = std::move(val4);
				if (nbElements > 5) mValue.value_list[5] = std::move(val5);
				if (nbElements > 6) mValue.value_list[6] = std::move(val6);
				if (nbElements > 7) mValue.value_list[7] = std::move(val7);
				if (nbElements > 8) mValue.value_list[8] = std::move(val8);
				if (nbElements > 9) mValue.value_list[9] = std::move(val9);
				mValue.current_value = 0;
			}

			std::vector<std::string> getEnumElements() const override { return { mValue.value_list.data(), mValue.value_list.data() + nbElements }; }

			virtual bool CopyAttribute(const CoreModifiableAttribute& attribute) override
			{
				if (CoreModifiableAttributeData<maEnumValue<nbElements>>::CopyAttribute(attribute)) return true;
				int val;
				if (attribute.getValue(val))
				{
					if (val >= 0 && val < nbElements)
					{
						mValue.current_value = val;
						return true;
					}
				}
				return false;
			}


			std::string& operator[](unsigned int index) { KIGS_ASSERT(index < nbElements); return mValue.value_list[index]; }
			const std::string& operator[](unsigned int index) const { KIGS_ASSERT(index < nbElements); return mValue.value_list[index]; }



			operator int() { int val = 0; getValue(val); return val; }
			operator const std::string& () const
			{
				int val; getValue(val);
				return mValue.value_list[val];
			}


#define IMPLEMENT_SET_VALUE_ENUM(type)\
	virtual bool setValue(type value) override { if (this->isReadOnly()) { return false; }  unsigned int tmpValue = (unsigned int)value; if (tmpValue < nbElements) { mValue.current_value = tmpValue;  DO_NOTIFICATION(notificationLevel);  return true; } return false; }

			EXPAND_MACRO_FOR_NUMERIC_TYPES(NOQUALIFIER, NOQUALIFIER, IMPLEMENT_SET_VALUE_ENUM);

#define IMPLEMENT_GET_VALUE_ENUM(type)\
	virtual bool getValue(type value) const override {  unsigned int tmpValue=mValue.current_value; if(tmpValue<nbElements){ value = (type)tmpValue; return true;} return false;  }

			EXPAND_MACRO_FOR_NUMERIC_TYPES(NOQUALIFIER, &, IMPLEMENT_GET_VALUE_ENUM);


#undef IMPLEMENT_SET_VALUE_ENUM
#undef IMPLEMENT_GET_VALUE_ENUM

			virtual bool setValue(const std::string& value) override
			{
				if (this->isReadOnly()) { return false; }
				unsigned int i;
				for (i = 0; i < nbElements; i++)
				{
					if (mValue.value_list[i] == value)
					{
						if (i < nbElements)
						{
							mValue.current_value = i;
							DO_NOTIFICATION(notificationLevel);
							return true;
						}
						else return false;
					}
				}
				return false;
			}

			virtual bool getValue(std::string& value) const override
			{
				unsigned int tmpValue = mValue.current_value;
				if (tmpValue < nbElements)
				{
					value = mValue.value_list[tmpValue];
				}
				return false;
			}

			virtual bool setValue(const char* value) override
			{
				std::string localstr(value);
				return setValue(localstr);
			}

		};



		template<unsigned int nbElements>
		using maEnum = maEnumHeritage<0, nbElements>;

	}
}