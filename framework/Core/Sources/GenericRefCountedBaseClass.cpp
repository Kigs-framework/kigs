#include "PrecompiledHeaders.h"

#include "Core.h"
#include "GenericRefCountedBaseClass.h"


void GenericRefCountedBaseClass::GetRef()
{
	myRefCounter.fetch_add(1, std::memory_order_relaxed);
}

bool GenericRefCountedBaseClass::TryGetRef()
{
	int before = myRefCounter.fetch_add(1);
	if (before == 0)
	{
		myRefCounter.fetch_sub(1, std::memory_order_relaxed);
		return false;
	}
	return true;
}

void     GenericRefCountedBaseClass::Destroy()
{
	int before = myRefCounter.fetch_sub(1, std::memory_order_relaxed);
	if (before == 1)
	{
		if (checkDestroy())
		{
			myRefCounter.fetch_add(1, std::memory_order_relaxed);
		}
		else
		{
			delete this;
		}
	}
}
