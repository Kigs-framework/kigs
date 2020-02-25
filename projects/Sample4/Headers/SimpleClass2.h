#pragma once
#include "CoreModifiable.h"

class SimpleClass2 : public CoreModifiable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(SimpleClass2, CoreModifiable, Application);
	DECLARE_CONSTRUCTOR(SimpleClass2);

protected:

	DECLARE_PURE_VIRTUAL_METHOD(GiveInfos);
	// list all CoreModifiable methods
	COREMODIFIABLE_METHODS(GiveInfos);
};

