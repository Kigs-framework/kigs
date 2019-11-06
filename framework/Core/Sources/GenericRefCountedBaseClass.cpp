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



void     GenericRefCountedBaseClass::Destroy()
{
	KigsCore::Instance()->GetSemaphore();
	myRefCounter--;
	if (myRefCounter == 0)
	{
		delete this;
	}
	KigsCore::Instance()->ReleaseSemaphore();
}
