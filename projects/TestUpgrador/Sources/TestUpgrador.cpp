#include <TestUpgrador.h>

#include "SimpleClass.h"

IMPLEMENT_CLASS_INFO(TestUpgrador);

IMPLEMENT_CONSTRUCTOR(TestUpgrador)
{

}

void	TestUpgrador::ProtectedInit()
{
	// Base modules have been created at this point
	// lets say that the update will sleep 1ms
	SetUpdateSleepTime(1);

	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), SimpleClass, SimpleClass, Application);

	SP<SimpleClass>	instance1 = KigsCore::GetInstanceOf("instance1", "SimpleClass");

	instance1->setValue("StringValue", "string value");
	instance1->Init();

	UpgradeSimple* yes = new UpgradeSimple();
	instance1->Upgrade(yes);


	instance1->SimpleCall("DoSomethingElse");
}

void	TestUpgrador::ProtectedUpdate()
{
}

void	TestUpgrador::ProtectedClose()
{
}
