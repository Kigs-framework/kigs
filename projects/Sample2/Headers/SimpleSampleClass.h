#pragma once
#include "SimpleSampleBaseClass.h"

class SimpleSampleClass : public SimpleSampleBaseClass
{
public:
	DECLARE_CLASS_INFO(SimpleSampleClass, SimpleSampleBaseClass, Application);
	DECLARE_CONSTRUCTOR(SimpleSampleClass);

protected:
	void InitModifiable() override;

};
