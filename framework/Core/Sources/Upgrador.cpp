#include "Upgrador.h"
#include "CoreModifiable.h"

void UpgradorBase::UpgradeInstance(CoreModifiable* toUpgrade)
{
	kstl::vector<std::pair<KigsID, RefCountedClass::ModifiableMethod>> MethodTable;
	GetMethodTable(MethodTable);
	for (auto& t : MethodTable)
	{
		toUpgrade->InsertUpgradeMethod(t.first, t.second,this);
	}

	Init(toUpgrade);
}
void UpgradorBase::DowngradeInstance(CoreModifiable* toDowngrade)
{
	kstl::vector<std::pair<KigsID, RefCountedClass::ModifiableMethod>> MethodTable;
	GetMethodTable(MethodTable);
	for (auto& t : MethodTable)
	{
		toDowngrade->RemoveMethod(t.first);
	}
	Destroy(toDowngrade);
}