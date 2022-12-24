#include "SimpleClass.h"
#include <iostream>

using namespace Kigs;
using namespace Kigs::Core;

IMPLEMENT_CLASS_INFO(SimpleClass)

IMPLEMENT_CONSTRUCTOR(SimpleClass)
// attribute is set as init attribute : it will be set as readonly after instance was initialized
, mIntValue(*this,true,"IntValue",5)					
// attribute is not set as init attribute : it will be read/write enabled
, mStringValue(* this, false, "StringValue")
{
	// this will be notified when StringValue is changed
	mStringValue.changeNotificationLevel(Owner);
}

void SimpleClass::NotifyUpdate(const u32 labelid)
{
	if (labelid == mStringValue.getID())
	{
		std::cout << "StringValue new value is : " <<  mStringValue.const_ref() << std::endl;
	}
}

void	SimpleClass::DoSomethingFun()
{
	if ((CoreModifiable*)mRef) // check that m_Ref point to an existing instance
	{
		CoreModifiable* other = (CoreModifiable*)mRef;
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