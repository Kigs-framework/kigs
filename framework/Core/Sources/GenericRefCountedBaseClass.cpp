#include "PrecompiledHeaders.h"

#include "Core.h"
#include "GenericRefCountedBaseClass.h"


void GenericRefCountedBaseClass::GetRef()
{
	mRefCounter.fetch_add(1, std::memory_order_relaxed);
}

bool GenericRefCountedBaseClass::TryGetRef()
{
	int before = mRefCounter.fetch_add(1);
	if (before == 0)
	{
		mRefCounter.fetch_sub(1, std::memory_order_relaxed);
		return false;
	}
	return true;
}

void     GenericRefCountedBaseClass::Destroy()
{
	int before = mRefCounter.fetch_sub(1, std::memory_order_relaxed);
	if (before == 1)
	{
		if (checkDestroy())
		{
			mRefCounter.fetch_add(1, std::memory_order_relaxed);
		}
		else
		{
			delete this;
		}
	}
}
