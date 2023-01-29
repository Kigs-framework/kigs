#pragma once 

#include "CoreModifiableAttribute.h"

#include <any>
#include <optional>

namespace Kigs
{
	namespace Core
	{

	template<bool notificationLevel, bool isInitT = false, bool isReadOnlyT = false, bool isDynamicT = false, bool isOrphanT = false>
	class maAnyHeritage : public CoreModifiableAttributeData<std::any, notificationLevel, isInitT, isReadOnlyT, isDynamicT, isOrphanT>
		{
			DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maAnyHeritage, maAnyHeritage, std::any, CoreModifiable::ATTRIBUTE_TYPE::ANY);
		public:
			virtual operator CurrentAttributeType() const
			{
				return mValue;
			}

			template<typename T>
			auto& operator=(const T& value)
			{
				mValue = value;
				return *this;
			}

			template<typename T>
			std::optional<T> get()
			{
				T* value = std::any_cast<T>(&mValue);
				if (value) return *value;
				return {};
			}

			template<typename T>
			T* getAny()
			{
				return std::any_cast<T>(&mValue);
			}

		};

		using maAny = maAnyHeritage<false,false,false,false,false>;


		template<typename T>
		T* CoreModifiable::getAny(const KigsID& id)
		{
			auto attr = getAttribute(id);
			if (!attr)
			{
				attr = AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::ANY, id);
				static_cast<maAny*>(attr)->ref() = T{};
			}
			if (attr && attr->getType() == CoreModifiable::ATTRIBUTE_TYPE::ANY)
			{
				return static_cast<maAny*>(attr)->getAny<T>();
			}
			return nullptr;
		}
	}
}