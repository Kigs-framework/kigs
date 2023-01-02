#pragma once

#include "CoreModifiable.h"

namespace Kigs
{
	namespace Anim
	{
		using namespace Kigs::Core;

		class AObjectResource : public CoreModifiable
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(AObjectResource, CoreModifiable, Animation)
				DECLARE_INLINE_CONSTRUCTOR(AObjectResource) {}
		};

	}
}
