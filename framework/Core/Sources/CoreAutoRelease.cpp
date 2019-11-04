#include "CoreAutoRelease.h"
#include "Core.h"


void	CoreAutoRelease::doAutoRelease()
{
	myInAutoReleaseLoop=true;
	kstl::set<RefCountedBaseClass*>::iterator start;
	kstl::set<RefCountedBaseClass*>::iterator end=myReleaseSet.end();

	for(start=myReleaseSet.begin();start!=end;start++)
	{
		(*start)->ProtectedDestroy();
		delete (*start);
	}
	myReleaseSet.clear();
	myInAutoReleaseLoop=false;
}

CoreAutoRelease::CoreAutoRelease()
{
	myReleaseSet.clear();
	myInAutoReleaseLoop = false;
}

CoreAutoRelease::~CoreAutoRelease()
{
	doAutoRelease();
}

void CoreAutoRelease::addInstance(RefCountedBaseClass* torelease)
{
	if (myInAutoReleaseLoop)
	{
		torelease->ProtectedDestroy();
	}
	else
	{
		myReleaseSet.insert(torelease);
	}
}
void CoreAutoRelease::removeInstance(RefCountedBaseClass* torelease)
{
	if (myInAutoReleaseLoop)
	{
		KIGS_ERROR("trying to restore destroyed instance in autorelease loop", 1);
	}
	kstl::set<RefCountedBaseClass*>::iterator foundinstance = myReleaseSet.find(torelease);
	if (foundinstance != myReleaseSet.end())
	{
		myReleaseSet.erase(foundinstance);
	}
}