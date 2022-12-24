#include <Sample2.h>
#include "Core.h"
#include "SmartPointer.h"
#include "SimpleSampleClass.h"
#include "SimpleMaterialClass.h"


// Sample demonstrating basic CoreModifiable features :
// instance factory, reference counting, tree organization ...

// Kigs framework Sample2 project
// detailed CoreModifiable features :
// - instance factory
// - CoreModifiable trees
// - Reference counting
// - search instances by type, name, path...

using namespace Kigs;
using namespace Kigs::Core;

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
	// now when asked for a "SimpleSampleClass" instance, a SimpleSampleClass instance will be created  
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), SimpleSampleClass, SimpleSampleClass, Application);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), SimpleMaterialClass, SimpleMaterialClass, Application);
	
	// ask for a SimpleSampleClassBase instance named simpleclass1
	CMSP simpleclass1 = KigsCore::GetInstanceOf("simpleclass1", "SimpleSampleClass");
	// Initialise class
	simpleclass1->Init();

	// ask for two other instances
	CMSP simpleclass2 = KigsCore::GetInstanceOf("simpleclass2", "SimpleSampleClass");
	simpleclass2->Init();
	CMSP simpleclass3 = KigsCore::GetInstanceOf("simpleclass3", "SimpleSampleClass");
	simpleclass3->Init();

	// create an instance of SimpleMaterialClass
	CMSP material= KigsCore::GetInstanceOf("material", "SimpleMaterialClass");
	// manage simpleclass3 and material as one unique object
	simpleclass3->aggregateWith(material);

	// and add simpleclass2 and simpleclass3 to simpleclass1
	simpleclass1->addItem(simpleclass2); // simpleclass2 count ref is now 2
	simpleclass1->addItem(simpleclass3); // simpleclass3 count ref is now 2

	// add simpleclass1 to this
	addItem(simpleclass1);

	// ask application to update simpleclass2 at each loop turn
	AddAutoUpdate(simpleclass2.get());

	// now we can get out of the method without loosing all the created instances

}

void	Sample2::ProtectedUpdate()
{
	// retrieve instances in the instances tree using "path" 
	CMSP simpleclass2 = GetInstanceByPath("SimpleSampleClass:simpleclass1/simpleclass2");
	CMSP simpleclass1 = simpleclass2->GetInstanceByPath("/Sample2/SimpleSampleClass:simpleclass1");
	CMSP simpleclass3 = simpleclass2->GetInstanceByPath("../simpleclass3");
	simpleclass3 = GetInstanceByPath("*/simpleclass3");

	// retreive all instances named "simpleclass1" in sons list
	std::vector<CMSP> instances;
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
	instances = GetInstancesByName("CoreModifiable", "simpleclass1");
	printf("GetInstancesByName result :\n");
	for (auto i : instances)
	{
		printf("found instance named : %s\n", i->getName().c_str());
	}
	instances.clear();
	// retreive all instances of type SimpleSampleClassBase at global scope
	// WARNING : Here the type is the instance factory type
	instances = GetInstances("SimpleSampleClass");
	printf("GetInstances result :\n");
	for (auto i : instances)
	{
		printf("found instance named : %s\n", i->getName().c_str());

		i->SimpleCall("printMessage");

		// check if i has Shininess parameter
		float shine;
		if (i->getValue("Shininess", shine))
		{
			std::cout << i->getName() << " has Shininess value of " << shine << " thanks to aggregate with SimpleMaterialClass " << std::endl;
		}
	}
	instances.clear();
	

	// ask exit 
  	mNeedExit = true;
}

void	Sample2::ProtectedClose()
{
}
