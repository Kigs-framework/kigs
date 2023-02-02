#pragma once

#include "CoreModifiableAttribute.h"
#include "usString.h"

namespace Kigs
{
	namespace Core
	{
		// ****************************************
		// * maUSStringHeritage class
		// * --------------------------------------
		/**
		* \class	maUSStringHeritage
		* \ingroup CoreModifiableAttibute
		* \brief	CoreModifiableAttributeData of usstring with different level of notification
		*/
		// ****************************************
		/*template<bool notificationLevel, bool isInitT = false, bool isReadOnlyT = false, bool isOrphanT = false>
		class maUSStringHeritage : public CoreModifiableAttributeData<usString, notificationLevel, isInitT, isReadOnlyT, isOrphanT>
		{
			DECLARE_ATTRIBUTE_HERITAGE(maUSStringHeritage, maUSStringHeritage, usString, CoreModifiable::ATTRIBUTE_TYPE::USSTRING);

		public:

			//! Extra constructors
			maUSStringHeritage(CoreModifiable& owner, KigsID ID, const std::string& value) : CoreModifiableAttributeData<usString, notificationLevel, isInitT, isReadOnlyT, isOrphanT>(owner, ID)
			{
				mValue = usString{ value };
			}

			void* getRawValue(CoreModifiable* owner) final { return (void*)mValue.us_str(); }
			size_t MemorySize() const final { return mValue.length() * sizeof(u16); };

			//! return a const unsigned short* pointer on internal value
			const unsigned short* us_str() const { return mValue.us_str(); }
			std::string ToString() { return mValue.ToString(); }
			void strcpywUtoC(char* _Dest, const unsigned short* src) { mValue.strcpywUtoC(_Dest, src); }
			std::vector<usString>	SplitByCharacter(unsigned short value) const { return mValue.SplitByCharacter(value); }
			unsigned int strlen() const { return mValue.strlen(); }

		};*/


		using maUSString = CoreModifiableAttributeData<usString, false,false,false,false>;
		using maUSStringOrphan = CoreModifiableAttributeData<usString, false, false, false, true>;
		using maUSStringInit = CoreModifiableAttributeData<usString, false, true>;
	}
}