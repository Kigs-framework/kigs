#include <Sample3.h>
#include "SimpleClass.h"
#include <iostream>

// Kigs framework Sample3 project
// detailed CoreModifiable attribute features :
// - attribute declaration
// - dynamic attribute
// - set / get attribute values

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

	SP<SimpleClass>	instance1= KigsCore::GetInstanceOf("instance1", "SimpleClass");
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
	SP<SimpleClass> instance2 = KigsCore::GetInstanceOf("instance2", "SimpleClass");

	// add a string attribute with name "DynamicAttribute" to instance 2
	instance2->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::STRING, "DynamicAttribute", "initValue");
	// add a unsigned int attribute with name "DynamicAttribute" to this 
	AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::UINT, "DynamicAttribute", 1250);

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


	// IntValue is set like this in the XML file :
	// <Attr N="IntValue" V="eval(32*4)"/>
	// so the initial value of IntValue will be set by evaluating the expression between the "eval" parenthesis 
	// expressions can be more complex and be based on other attributes.
	auto testAttributeImport = CoreModifiable::Import("testImport.xml");
	std::cout << "Evaluated Int Value (eval(32*4)) : " << testAttributeImport->getValue<int>("IntValue") << std::endl;
}

void	Sample3::ProtectedUpdate()
{
	// ask exit 
	mNeedExit = true;
}

void	Sample3::ProtectedClose()
{
}
