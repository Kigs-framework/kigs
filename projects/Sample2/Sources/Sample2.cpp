#include <Sample2.h>
#include "Core.h"
#include "SmartPointer.h"
#include "SimpleSampleClass.h"

IMPLEMENT_CLASS_INFO(Sample2);

IMPLEMENT_CONSTRUCTOR(Sample2)
{

}

void	Sample2::ProtectedInit()
{
	// Base modules have been created at this point
	// lets say that the update will sleep 1ms
	SetUpdateSleepTime(1);

	// declare project class to instance factory
	// now when asked for a "SimpleSampleClassBase" instance, a SimpleSampleClass instance will be created  
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), SimpleSampleClass, SimpleSampleClassBase, Application);

	// ask for a SimpleSampleClassBase instance named simpleclass1
	CoreModifiable* simpleclass1 = KigsCore::GetInstanceOf("simpleclass1", "SimpleSampleClassBase");
	// Initialise class
	simpleclass1->Init();

	// ask for two other instances
	CoreModifiable* simpleclass2 = KigsCore::GetInstanceOf("simpleclass2", "SimpleSampleClassBase");
	simpleclass2->Init();
	SmartPointer<CoreModifiable> simpleclass3 = KigsCore::CreateInstance("simpleclass3", "SimpleSampleClassBase");
	simpleclass3->Init();

	// and add simpleclass2 and simpleclass3 to simpleclass1
	simpleclass1->addItem(simpleclass2); // simpleclass2 count ref is now 2
	simpleclass1->addItem(simpleclass3.get()); // simpleclass3 count ref is now 2

	// Now let simpleclass1 manage simpleclass2 and simpleclass3 life cycle
	simpleclass2->Destroy(); // simpleclass2 count ref is now 1

	// add simpleclass1 to this
	addItem(simpleclass1);
	// and let this manage simpleclass1 life cycle
	simpleclass1->Destroy(); // simpleclass1 count ref is now 1

	// now we can get out of the method without loosing all the created instances

}

void	Sample2::ProtectedUpdate()
{
	// retrieve instances in the instances tree using "path" 
	CoreModifiable* simpleclass2 = GetInstanceByPath("SimpleSampleClass:simpleclass1/simpleclass2");
	CoreModifiable* simpleclass1 = simpleclass2->GetInstanceByPath("/Sample2/SimpleSampleClass:simpleclass1");
	CoreModifiable* simpleclass3 = simpleclass2->GetInstanceByPath("../simpleclass3");
	simpleclass3 = GetInstanceByPath("*/simpleclass3");

	// retreive all instances named "simpleclass1" in sons list
	std::set<CoreModifiable*> instances;
	GetSonInstancesByName("CoreModifiable", "simpleclass1",instances);
	printf("GetSonInstancesByName result :\n");
	for (auto i : instances)
	{
		printf("found instance named : %s\n", i->getName().c_str());
	}
	instances.clear();
	// retreive all instances named "simpleclass2" recursively in sons list
	GetSonInstancesByName("CoreModifiable", "simpleclass2", instances,true);
	printf("Recursive GetSonInstancesByName result :\n");
	for (auto i : instances)
	{
		printf("found instance named : %s\n", i->getName().c_str());
	}
	instances.clear();
	// retreive all instances of type CoreModifiable in sons list
	GetSonInstancesByType("CoreModifiable", instances);
	printf("GetSonInstancesByType result :\n");
	for (auto i : instances)
	{
		printf("found instance named : %s\n", i->getName().c_str());
	}
	instances.clear();
	// retreive all instances of type SimpleSampleClass recursively in sons list
	GetSonInstancesByType("SimpleSampleClass", instances,true);
	printf("Recursive GetSonInstancesByType result :\n");
	for (auto i : instances)
	{
		printf("found instance named : %s\n", i->getName().c_str());
	}

	// retreive all instances named "simpleclass1" at global scope
	GetInstancesByName("CoreModifiable", "simpleclass1", instances);
	printf("GetInstancesByName result :\n");
	for (auto i : instances)
	{
		printf("found instance named : %s\n", i->getName().c_str());
	}
	instances.clear();
	// retreive all instances of type SimpleSampleClassBase at global scope
	// WARNING : Here the type is the instance factory type
	GetInstances("SimpleSampleClassBase", instances);
	printf("GetInstances result :\n");
	for (auto i : instances)
	{
		printf("found instance named : %s\n", i->getName().c_str());
	}
	instances.clear();
	

	// ask exit 
  	myNeedExit = true;
}

void	Sample2::ProtectedClose()
{
}
