#pragma once

#include "CoreModifiableAttribute.h"

namespace Kigs
{
	namespace Core
	{
		// ****************************************
		// * maString attribute
		// * --------------------------------------
		// ****************************************

		using maString = CoreModifiableAttributeInherited<std::string, false,false,false,false>;
		using maStringOrphan = CoreModifiableAttributeInherited<std::string, false, false, false, true>;
		using maStringInit = CoreModifiableAttributeInherited<std::string, false, true>;

	}
}