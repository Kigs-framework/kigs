#pragma once
#include "CoreModifiable.h"
#include <iostream>

class SimpleSampleBaseClass : public CoreModifiable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(SimpleSampleBaseClass, CoreModifiable, Application);
	DECLARE_INLINE_CONSTRUCTOR(SimpleSampleBaseClass) { std::cout << "SimpleSampleBaseClass constructor" << std::endl; }

protected:
	void InitModifiable() override;

};
