#include "SimpleSampleBaseClass.h"
#include "CoreModifiableAttribute.h"

using namespace Kigs;
using namespace Kigs::Core;
IMPLEMENT_CLASS_INFO(SimpleSampleBaseClass)

void SimpleSampleBaseClass::InitModifiable()
{
	// check for multiple init
	if (_isInit)
	{
		// init was already done, just return
		return;
	}
	// call parent class InitModifiable
	ParentClassType::InitModifiable();
	// if everything is OK, do this initialisation
	if (_isInit)
	{
		bool somethingWentWrong = false;
		// here is some initialisation code for this 
		std::cout << "SimpleSampleBaseClass InitModifiable " << getName() << std::endl;
		// check if something went wrong 
		if (somethingWentWrong)
		{
			// call Uninit
			UnInit();
			return;
		}
	}
}

void  SimpleSampleBaseClass::Update(const Time::Timer& timer, void* addParam)
{
	// Update code
 	std::cout << "SimpleSampleBaseClass Update " << getName() << std::endl;
}

DEFINE_METHOD(SimpleSampleBaseClass, incrementParam)
{
	float val=0;
	// access first param (we could check for param name here)
	if (params[0]->getValue(val)) // if first param value can be get as float
	{
		// increment value
		params[0]->setValue(val + 1.0f);
	}

	return true;
}