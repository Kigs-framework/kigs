#include "Upgrador.h"
#include "CoreModifiable.h"

using namespace Kigs::Core;

void UpgradorBase::UpgradeInstance(CoreModifiable* toUpgrade, bool reinit, bool attachmethod)
{
	if (attachmethod)
	{
		std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>> MethodTable;
		GetMethodTable(MethodTable);
		for (auto& t : MethodTable)
		{
			toUpgrade->InsertUpgradeMethod(t.first, t.second, this);
		}
	}
	if (reinit || !mIsInit) // if not init, don't take reinit into account
	{
		Init(toUpgrade);
	}
}
void UpgradorBase::DowngradeInstance(CoreModifiable* toDowngrade, bool doDestroy,bool detachmethod)
{
	if (detachmethod)
	{
		std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>> MethodTable;
		GetMethodTable(MethodTable);
		for (auto& t : MethodTable)
		{
			toDowngrade->RemoveMethod(t.first);
		}
	}
	if (doDestroy)
	{
		Destroy(toDowngrade);
	}
}