#include "MoreComplexClass.h"
#include "CoreModifiableAttribute.h"
#include "AttributePacking.h"
#include <iostream>

IMPLEMENT_CLASS_INFO(MoreComplexClass)

IMPLEMENT_CONSTRUCTOR(MoreComplexClass)
{

}

// define member method named GiveInfos on MoreComplexClass 
DEFINE_METHOD(MoreComplexClass, GiveInfos)
{

	std::cout << "MoreComplexClass GiveInfos method called on " << getName() << " instance" << std::endl;
	if (sender)
		std::cout << "-- sender : " << sender->getName() << std::endl;

	for (auto p : params)
	{
#ifdef KEEP_NAME_AS_STRING
		std::string v;
		if (p->getValue(v))
		{
			std::cout << "-- parameter : " << p->getID()._id_name << " value is : " << v << std::endl;
		}
		else
		{
			std::cout << "-- parameter : " << p->getID()._id_name << " value cannot be evaluated as string" << std::endl;
		}
#else
		std::string v;
		if (p->getValue(v))
		{
			std::cout << "-- parameter : " << p->getID()._id << " value is : " << v << std::endl;
		}
		else
		{
			std::cout << "-- parameter : " << p->getID()._id << " value cannot be evaluated as string" << std::endl;
		}
#endif
	}

	if (privateParams)
		std::cout << "-- private parameter is not null" << std::endl;
	else
		std::cout << "-- private parameter is null" << std::endl;

	PUSH_RETURN_VALUE(53);

	if(params.size()&1) // returns true if parameter count is odd
		return true;

	return false;
}