#pragma once

#include "CoreModifiableAttribute.h"
#include "usString.h"

namespace Kigs
{
	namespace Core
	{
		// ****************************************
		// * maUSString attribute
		// * --------------------------------------
		// ****************************************
		
		using maUSString = CoreModifiableAttributeInherited<usString, false,false,false,false>;
		using maUSStringOrphan = CoreModifiableAttributeInherited<usString, false, false, false, true>;
		using maUSStringInit = CoreModifiableAttributeInherited<usString, false, true>;
	}
}