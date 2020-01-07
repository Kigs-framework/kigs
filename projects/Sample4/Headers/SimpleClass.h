#pragma once
#include "CoreModifiable.h"

class SimpleClass : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(SimpleClass, CoreModifiable, Application);
	DECLARE_CONSTRUCTOR(SimpleClass);

protected:

	DECLARE_METHOD(GiveInfos);
};

