#include "Upgrador.h"
#include "CoreModifiable.h"

void UpgradorBase::UpgradeInstance(CoreModifiable* toUpgrade, bool reinit)
{
	if (reinit)
	{
		std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>> MethodTable;
		GetMethodTable(MethodTable);
		for (auto& t : MethodTable)
		{
			toUpgrade->InsertUpgradeMethod(t.first, t.second, this);
		}
		Init(toUpgrade);
	}
}
void UpgradorBase::DowngradeInstance(CoreModifiable* toDowngrade, bool doDestroy)
{
	if (doDestroy)
	{
		std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>> MethodTable;
		GetMethodTable(MethodTable);
		for (auto& t : MethodTable)
		{
			toDowngrade->RemoveMethod(t.first);
		}
		Destroy(toDowngrade);
	}
}