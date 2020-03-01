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
	DECLARE_CLASS_ALIAS(KigsCore::Instance(), ComplexClass, SimpleClass, UpgradeSimple, OtherUpgradeSimple);

	REGISTER_UPGRADOR(UpgradeSimple);
	REGISTER_UPGRADOR(OtherUpgradeSimple);

#define TEST_EXPORT
#ifdef TEST_EXPORT
	SP<SimpleClass>	instance1 = KigsCore::GetInstanceOf("instance1", "ComplexClass");

	instance1->setValue("StringValue", "string value");
	instance1->Init();

	Export("TestUpgrador.xml", instance1.get());
#else

	SP<SimpleClass>	instance1 = CoreModifiable::Import("TestUpgrador.xml");

#endif
	instance1->SimpleCall("DoSomethingElse");
	EmitSignal("Signal");

	instance1->CallUpdate(*GetApplicationTimer().get(),nullptr);

	instance1->SimpleCall("DoSomethingElse");
	EmitSignal("Signal");

}

void	TestUpgrador::ProtectedUpdate()
{
}

void	TestUpgrador::ProtectedClose()
{
}
