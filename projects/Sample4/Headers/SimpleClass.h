#pragma once
#include "CoreModifiable.h"

namespace Kigs
{
	using namespace Kigs::Core;
	class SimpleClass : public CoreModifiable
	{
	public:
		DECLARE_CLASS_INFO(SimpleClass, CoreModifiable, Application);
		DECLARE_CONSTRUCTOR(SimpleClass);

	protected:

		DECLARE_METHOD(GiveInfos);
		// list all CoreModifiable methods
		COREMODIFIABLE_METHODS(GiveInfos);
	};

}