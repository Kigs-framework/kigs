#pragma once

#include "CoreModifiableAttribute.h"

namespace Kigs
{
	namespace Core
	{
		// ****************************************
		// * maStringHeritage class
		// * --------------------------------------
		/**
		* \class	maStringHeritage
		* \ingroup CoreModifiableAttibute
		* \brief	CoreModifiableAttributeData of string with different level of notification
		*/
		// ****************************************

		/*template<bool notificationLevel, bool isInitT = false, bool isReadOnlyT = false, bool isOrphanT = false>
		class maStringHeritage : public CoreModifiableAttributeData<std::string, notificationLevel, isInitT, isReadOnlyT, isOrphanT>
		{
			DECLARE_ATTRIBUTE_HERITAGE(maStringHeritage, maStringHeritage, std::string, CoreModifiable::ATTRIBUTE_TYPE::STRING);

		public:

			//! return a const char* pointer on internal value
			//! Doesn't call modifiers!
			const char* c_str() const { return mValue.c_str(); }

			void* getRawValue(CoreModifiable* owner) final { return (void*)mValue.data(); }
			size_t MemorySize() const final { return mValue.size(); };
		};*/


		//STATIC_ASSERT_NOTIF_LEVEL_SIZES(maStringHeritage);


		using maString = CoreModifiableAttributeData<std::string, false,false,false,false>;
		using maStringOrphan = CoreModifiableAttributeData<std::string, false, false, false, true>;
		using maStringInit = CoreModifiableAttributeData<std::string, false, true>;

	}
}