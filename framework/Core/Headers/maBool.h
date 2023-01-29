#pragma once

#include "CoreModifiableAttribute.h"

namespace Kigs
{
	namespace Core
	{
		// ****************************************
		// * maBoolHeritage class
		// * --------------------------------------
		/**
		* \class	maBoolHeritage
		* \ingroup CoreModifiableAttibute
		* \brief	CoreModifiableAttributeData for bool with different level of notification
		*/
		// ****************************************
	template<bool notificationLevel, bool isInitT = false, bool isReadOnlyT = false, bool isDynamicT = false, bool isOrphanT = false>
	class maBoolHeritage : public CoreModifiableAttributeData<bool, notificationLevel, isInitT, isReadOnlyT, isDynamicT, isOrphanT>
		{
			DECLARE_ATTRIBUTE_HERITAGE(maBoolHeritage, maBoolHeritage, bool, CoreModifiable::ATTRIBUTE_TYPE::BOOL);

		public:

			// getValue overloads
#define IMPLEMENT_GET_VALUE_BOOL(type)\
virtual bool getValue(type value, const CoreModifiable* owner) const override \
{\
	bool tmpValue = mValue;\
	value = (type)tmpValue;\
	return true;\
}

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, &, IMPLEMENT_GET_VALUE_BOOL);

			virtual bool getValue(std::string& value, const CoreModifiable* owner) const override
			{
				bool tmpValue = mValue;
				value = tmpValue ? "true" : "false";
				return true;
			}
			///

			// setValue overloads
#define IMPLEMENT_SET_VALUE_BOOL(type)\
	virtual bool setValue(type value, CoreModifiable* owner) override\
	{\
		RETURN_ON_READONLY(isReadOnlyT);\
		bool tmpValue = (value != (type)0);\
		mValue = tmpValue; \
		DO_NOTIFICATION(notificationLevel);\
		return true;\
	}

			EXPAND_MACRO_FOR_BASE_TYPES(NOQUALIFIER, NOQUALIFIER, IMPLEMENT_SET_VALUE_BOOL);


			virtual bool setValue(const std::string& value, CoreModifiable* owner) override
			{
			RETURN_ON_READONLY(isReadOnlyT);
				bool tmpValue = (value == "true" || value == "TRUE");
				mValue = tmpValue;
				DO_NOTIFICATION(notificationLevel);
				return true;
			}
		virtual bool setValue(const char* value, CoreModifiable* owner) override { RETURN_ON_READONLY(isReadOnlyT); if (value) { std::string localstr(value); return setValue(localstr, owner); } return  setValue(false, owner); }
			///

		};

		using maBool = maBoolHeritage<false,false,false,false,false>;
		using maBoolOrphan = maBoolHeritage<false, false, false, false, true>;


#undef IMPLEMENT_SET_VALUE_BOOL
#undef IMPLEMENT_GET_VALUE_BOOL
	}
}
