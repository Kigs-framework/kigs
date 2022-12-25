#include <Sample4.h>
#include "SimpleClass.h"
#include "SimpleClass2.h"
#include "MoreComplexClass.h"
#include "CoreModifiableAttribute.h"
#include "Core.h"

#include <iostream>

using namespace Kigs;
using namespace Kigs::Core;

// Kigs framework Sample4 project
// detailed CoreModifiable method features :
// - declare / define method 
// - call method using CallMethod
// - call method using SimpleCall
// - wrap method
// - dynamic method 

IMPLEMENT_CLASS_INFO(Sample4);

IMPLEMENT_CONSTRUCTOR(Sample4)
{
	
}



// declare a dynamic method named addValues which can be added to any CoreModifiable instance
DEFINE_DYNAMIC_METHOD(CoreModifiable, addValues)
{
	float result = 0.0f;
	for (auto p : params)
	{
		float v;
		if (p->getValue(v))
		{
			result += v;
		}
	}
	PUSH_RETURN_VALUE(result);
	return true;
}

void	Sample4::ProtectedInit()
{
	// Base modules have been created at this point
	// lets say that the update will sleep 1ms
	SetUpdateSleepTime(1);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), SimpleClass, SimpleClass, Application);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), MoreComplexClass, MoreComplexClass, Application);

	// create one instance of each class
	CMSP instance1 = KigsCore::GetInstanceOf("instance1", "SimpleClass");
	CMSP instance2 = KigsCore::GetInstanceOf("instance2", "MoreComplexClass");

	// create dynamic attribute on this
	AddDynamicAttribute<maFloat, float>("FloatParam", 12.0f);

	// create CoreModifiableAttribute without owner

	CoreModifiableAttribute* intParam = new maInt("IntParam", 15);

	// create a parameter vector
	std::vector<CoreModifiableAttribute*> params;
	// push dynamic attribute "FloatParam" on vector
	params.push_back(getAttribute("FloatParam"));
	// push intParam
	params.push_back(intParam);
	
	// call GiveInfos on instance1 with params vector, no private parameter and sender is this  
	bool result = instance1->CallMethod("GiveInfos", params, nullptr, this);
	std::cout << "GiveInfos returns " << (result?"true":"false") << std::endl << std::endl;

	// call GiveInfos on instance2 with params vector, no private parameter and sender is this  
	int paramsCount = params.size();
	result = instance2->CallMethod("GiveInfos", params, nullptr, this);
	if (params.size() > paramsCount)
	{
		// return values added
		while(paramsCount<params.size())
		{
			std::string v;
			if(params.back()->getValue(v))
				std::cout << "GiveInfos returned value = " << v << std::endl;

			delete params.back();
			params.pop_back();
		}
	}
	std::cout << "GiveInfos returns " << (result ? "true" : "false") << std::endl << std::endl;


	// call GiveInfos on instance1 with this instance (Sample4) as parameter, no private params and no sender 
	// so the received parameter vector will be all the CoreModifiable attributes owned by this
	result = instance1->CallMethod("GiveInfos", this, nullptr, nullptr);
	std::cout << "GiveInfos returns " << (result ? "true" : "false") << std::endl << std::endl;

	// or with "SimpleCall" :
	result = instance1->SimpleCall("GiveInfos", 50, 53.0f);
	std::cout << "GiveInfos returns " << (result ? "true" : "false") << std::endl << std::endl;
	// "SimpleCall" on instance2:
	float floatresult = instance2->SimpleCall<float>("GiveInfos", 32,64,5);
	std::cout << "GiveInfos returns " << floatresult << std::endl << std::endl;

	delete intParam;

	// use wrapper

	floatresult = instance2->SimpleCall<float>("Multiply", 32, 5);
	std::cout << "instance2 Multiply returns " << floatresult << std::endl << std::endl;


	// check dynamic method
	floatresult = instance2->SimpleCall<float>("addValues", 32, 5);
	// addValues doesn't exist on instance 2, so result is 0
	std::cout << "instance2 addValues returns " << floatresult << std::endl << std::endl;
	// now add addValues on instance2
	instance2->INSERT_DYNAMIC_METHOD(addValues, addValues);
	// now addValues shoult exist on instance2
	floatresult = instance2->SimpleCall<float>("addValues", 32, 5);
	// this time add value should return the good result
	std::cout << "instance2 addValues returns " << floatresult << std::endl << std::endl;

}

void	Sample4::ProtectedUpdate()
{
	// ask exit 
	mNeedExit = true;
}

void	Sample4::ProtectedClose()
{
}

