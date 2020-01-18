#include "PrecompiledHeaders.h"

#include "Core.h"
#include "GenericRefCountedBaseClass.h"
//! increment refcounter
void    GenericRefCountedBaseClass::GetRef()
{
	// thread safe
	KigsCore::Instance()->GetSemaphore();
	myRefCounter++;
	KigsCore::Instance()->ReleaseSemaphore();
}


std::lock_guard<std::recursive_mutex>* GenericRefCountedBaseClass::lockForDestroy()
{
	KigsCore::Instance()->GetSemaphore();

	return nullptr;
}
void GenericRefCountedBaseClass::unlockForDestroy(std::lock_guard<std::recursive_mutex>* lk)
{
	KigsCore::Instance()->ReleaseSemaphore();
	if (lk)
	{
		delete lk;
	}
}


void     GenericRefCountedBaseClass::Destroy()
{
	std::lock_guard<std::recursive_mutex>* lk=lockForDestroy();
	myRefCounter--;
	if (myRefCounter == 0)
	{
		if (checkDestroy())
		{
			myRefCounter++;
		}
		else
		{
			delete this;
		}
	}
	unlockForDestroy(lk);
}
