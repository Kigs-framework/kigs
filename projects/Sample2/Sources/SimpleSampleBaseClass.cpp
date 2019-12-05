#include "SimpleSampleBaseClass.h"



IMPLEMENT_CLASS_INFO(SimpleSampleBaseClass)

void SimpleSampleBaseClass::InitModifiable()
{
	ParentClassType::InitModifiable();
	if (_isInit)
	{
		std::cout << "SimpleSampleBaseClass InitModifiable" << std::endl;
	}
}

