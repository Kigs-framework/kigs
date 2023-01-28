#pragma once

#include <vector>
#include <string>
#include "CoreModifiableAttribute.h"

namespace Kigs
{

	using ::std::string;
	using ::std::vector;

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

		operator vector<CoreModifiableAttribute*>& ()
		{
			return mAttributeList;
		}

		~PackCoreModifiableAttributes();

	protected:
		vector<CoreModifiableAttribute*>	mAttributeList;
		CoreModifiable* mOwner;
	};
}