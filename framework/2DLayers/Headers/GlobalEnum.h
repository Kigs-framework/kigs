#pragma once

#include "GlobalPrecompiledHeaders.h"

namespace Kigs
{
	namespace Draw2D
	{
		class UIInputEvent
		{
		public:
			enum ENUM
			{
				NONE = 0,
				LEFT = 1,
				MIDDLE = 2,
				RIGHT = 4,
				PINCH = 8
			};
		};
	}
}