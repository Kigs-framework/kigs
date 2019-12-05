#include <Sample2.h>
#include "Core.h"
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
	// now when asked for a "SampleClass" instance, a SimpleSampleClass instance will be created  
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), SimpleSampleClass, SampleClass, Application);

	// ask for a SampleClass instance named simpleclass1
	CoreModifiable* simpleclass1 = KigsCore::GetInstanceOf("simpleclass1", "SampleClass");
	// Initialise class
	simpleclass1->Init();

	// ask for two other instances
	CoreModifiable* simpleclass2 = KigsCore::GetInstanceOf("simpleclass2", "SampleClass");
	simpleclass2->Init();
	CoreModifiable* simpleclass3 = KigsCore::GetInstanceOf("simpleclass3", "SampleClass");
	simpleclass3->Init();

	// and add simpleclass2 and simpleclass3 to simpleclass1
	simpleclass1->addItem(simpleclass2); // simpleclass2 count ref is now 2
	simpleclass1->addItem(simpleclass3); // simpleclass3 count ref is now 2

	// Now let simpleclass1 manage simpleclass2 and simpleclass3 life cycle
	simpleclass2->Destroy(); // simpleclass2 count ref is now 1
	simpleclass3->Destroy(); // simpleclass3 count ref is now 1

	// add simpleclass1 to this
	addItem(simpleclass1);
	// and let this manage simpleclass1 life cycle
	simpleclass1->Destroy(); // simpleclass1 count ref is now 1

	// now we can get out of the method without loosing all the created instances
}

void	Sample2::ProtectedUpdate()
{
	// 
}

void	Sample2::ProtectedClose()
{
}
