#include "SimpleClass.h"
#include <iostream>

using namespace Kigs;
using namespace Kigs::Core;

IMPLEMENT_CLASS_INFO(SimpleClass)

IMPLEMENT_CONSTRUCTOR(SimpleClass)
{
	// attribute is set as init attribute : it will be set as readonly after instance was initialized
	setInitParameter("IntValue", true);
	// attribute is not set as init attribute : it will be read/write enabled
	// this will be notified when StringValue is changed
	setOwnerNotification("StringValue", true);
}

void SimpleClass::NotifyUpdate(const u32 labelid)
{
	if (labelid == KigsID("StringValue")._id)
	{
		std::cout << "StringValue new value is : " <<  mStringValue << std::endl;
	}
}

void	SimpleClass::DoSomethingFun()
{
	if (!mReference.expired()) // check that m_Ref point to an existing instance
	{
		CMSP other = mReference.lock();
		if (other->isSubType(SimpleClass::mClassID)) // if other is also a SimpleClass instance
		{
			std::cout << "referenced instance IntValue = " << other->getValue<int>("IntValue") << std::endl;
		}
		else
		{
			std::cout << "referenced instance with name " << other->getName() <<" is not a SimpleClass" << std::endl;
		}

		// if an attribute named "DynamicAttribute" is found, print its value
		std::string dynattr;
		if (other->getValue("DynamicAttribute", dynattr)) 
		{
			std::cout << "DynamicAttribute found on " << other->getName() << " with value : " << dynattr << std::endl;
		}
			 
	}
}