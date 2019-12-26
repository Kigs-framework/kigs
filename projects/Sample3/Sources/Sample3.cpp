#include <Sample3.h>
#include "SimpleClass.h"
#include <iostream>

IMPLEMENT_CLASS_INFO(Sample3);

IMPLEMENT_CONSTRUCTOR(Sample3)
{

}

void	Sample3::ProtectedInit()
{
	// Base modules have been created at this point
	// lets say that the update will sleep 1ms
	SetUpdateSleepTime(1);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), SimpleClass, SimpleClass, Application);

	SimpleClass*	instance1= KigsCore::GetInstanceOf("instance1", "SimpleClass")->as<SimpleClass>();
	// set values before initialization
	instance1->setValue("IntValue", 12);
	instance1->setValue("StringValue", "something");
	// initialize instance => init attributes will be set as read only
	instance1->Init();

	// retrieve and print values
	int v = instance1->getValue<int>("IntValue");
	std::string sv = instance1->getValue<std::string>("StringValue");

	std::cout << "Int Value : " << v << std::endl;
	std::cout << "String Value : " << sv << std::endl;

	// change values
	instance1->setValue("IntValue", 16);
	instance1->setValue("StringValue", "else");

	// retrieve and print values
	v = instance1->getValue<int>("IntValue");
	sv = instance1->getValue<std::string>("StringValue");

	// IntValue was set as read only, so it was not affected by setValue
	std::cout << "Int Value : " << v << std::endl;
	std::cout << "String Value : " << sv << std::endl;

	// create a second SimpleClass instance
	SimpleClass* instance2 = KigsCore::GetInstanceOf("instance2", "SimpleClass")->as<SimpleClass>();

	// add a string attribute with name "DynamicAttribute" to instance 2
	instance2->AddDynamicAttribute(CoreModifiable::STRING, "DynamicAttribute", "initValue");
	// add a unsigned int attribute with name "DynamicAttribute" to this 
	AddDynamicAttribute(CoreModifiable::UINT, "DynamicAttribute", 1250);

	instance1->setValue("Reference", "SimpleClass:instance2");
	instance2->setValue("Reference", "CoreBaseApplication:Sample3");

	instance1->DoSomethingFun();
	instance2->DoSomethingFun();

	// remove "DynamicAttribute" from this
	RemoveDynamicAttribute("DynamicAttribute");

	CoreModifiable* retrieveInstance;
	instance1->getValue("Reference", retrieveInstance);
	if (retrieveInstance)
	{
		std::cout << "instance1 reference " << retrieveInstance->getName() << " instance" << std::endl;
	}

	// accessing vector
	v4f vect;
	instance1->getValue("Vector", vect);
	std::cout << "vector values : {" << vect.x << "," << vect.y << "," << vect.z << "," << vect.w << "}" << std::endl;
	// set value with string
	instance1->setValue("Vector", "{2.0,1.0,0.0,-1.0}");
	instance1->getValue("Vector", vect);
	std::cout << "vector values : {" << vect.x << "," << vect.y << "," << vect.z << "," << vect.w << "}" << std::endl;
	// access with array of values
	float arrayv[4];
	instance1->getArrayValue("Vector", arrayv, 4);
	std::cout << "vector values : {" << arrayv[0] << "," << arrayv[1] << "," << arrayv[2] << "," << arrayv[3] << "}" << std::endl;
	arrayv[2] = 10.0f;
	instance1->setArrayValue("Vector", arrayv,4);
	
	// or by element 
	instance1->getArrayElementValue("Vector", arrayv[0], 0,2);
	std::cout << "vector values : {" << arrayv[0] << "," << arrayv[1] << "," << arrayv[2] << "," << arrayv[3] << "}" << std::endl;


	std::cout << "Int Value without modifier : " << instance1->getValue<int>("IntValue") << std::endl;

	// manualy add modifier 
	auto& instanceMap = KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap();
	auto itfound = instanceMap.find("CoreItemOperatorModifier");
	if (itfound != instanceMap.end())
	{
		AttachedModifierBase* toAdd = (AttachedModifierBase*)(*itfound).second();
		CoreModifiableAttribute* attr = instance1->getAttribute("IntValue");
		if (toAdd && attr)
		{
			// define getter modifier multiplying by two the value retreive with getValue
			toAdd->Init(attr, true, "input*2");
			attr->attachModifier(toAdd);
		}
	}

	// getValue now returns 2 * the stored value
	std::cout << "Int Value with modifier : " << instance1->getValue<int>("IntValue") << std::endl;

	instance1->Destroy();
	instance2->Destroy();

}

void	Sample3::ProtectedUpdate()
{
	// ask exit 
	myNeedExit = true;
}

void	Sample3::ProtectedClose()
{
}
