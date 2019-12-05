#include "SimpleSampleClass.h"

#include <iostream>

IMPLEMENT_CLASS_INFO(SimpleSampleClass)

IMPLEMENT_CONSTRUCTOR(SimpleSampleClass)
{
	std::cout << "SimpleSampleClass constructor" << std::endl;
}

void SimpleSampleClass::InitModifiable()
{
	ParentClassType::InitModifiable();
	if (_isInit)
	{
		std::cout << "SimpleSampleClass InitModifiable" << std::endl;
	}
	
}

