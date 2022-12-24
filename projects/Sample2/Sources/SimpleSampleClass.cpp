#include "SimpleSampleClass.h"

#include <iostream>

using namespace Kigs;
using namespace Kigs::Core;

IMPLEMENT_CLASS_INFO(SimpleSampleClass)

IMPLEMENT_CONSTRUCTOR(SimpleSampleClass)
{
	std::cout << "SimpleSampleClass constructor" << std::endl;
}

void SimpleSampleClass::InitModifiable()
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
		std::cout << "SimpleSampleClass InitModifiable " << getName() << std::endl;
		// check if something went wrong 
		if (somethingWentWrong)
		{
			// call Uninit
			UnInit();
			return;
		}
	}
}

void	SimpleSampleClass::printMessage()
{
	std::cout << "SimpleSampleClass " << getName() << " printMessage " << std::endl;
}

bool SimpleSampleClass::addItem(const CMSP& item, ItemPosition pos)
{
	if (item->isSubType("SimpleSampleClass"))
	{
		std::cout << "SimpleSampleClass " << item->getName() << " added to this ( " << getName() << " ) "<< std::endl;

		
		CoreModifiableAttribute* param = item->getAttribute("CountWhenAdded");
		if (param)
		{
			// call incrementParam method
			std::vector<CoreModifiableAttribute*> sendParams;
			sendParams.push_back(param);
			item->CallMethod("incrementParam", sendParams);

			std::cout << item->getName() << " was added " << item->getValue<int>("CountWhenAdded") << " times " << std::endl;
		}
		
	}

	return ParentClassType::addItem(item, pos);
}
