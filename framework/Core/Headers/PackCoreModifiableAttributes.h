#pragma once

#include <vector>
#include <string>
#include "CoreModifiableAttribute.h"

namespace Kigs
{
	namespace Core
	{

		class CoreModifiable;

		class PackCoreModifiableAttributes
		{
		public:
			PackCoreModifiableAttributes(CoreModifiable* owner) :mOwner(owner)
			{
				mAttributeList.clear();
			}

			template<typename T>
			PackCoreModifiableAttributes& operator<<(T&& V);

			void AddAttribute(CoreModifiableAttribute* attr) { mAttributeList.push_back(attr); }

			operator std::vector<CoreModifiableAttribute*>& ()
			{
				return mAttributeList;
			}

			~PackCoreModifiableAttributes()
			{
				for (auto attr : mAttributeList)
					delete attr;
			}

		protected:
			std::vector<CoreModifiableAttribute*>	mAttributeList;
			CoreModifiable* mOwner;
		};
	}
}