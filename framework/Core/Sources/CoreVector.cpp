#include "PrecompiledHeaders.h"
#include "CoreVector.h"

CoreItem* CoreVectorIterator::operator*() const
{
	kstl::vector<RefCountedBaseClass*>&	vectorstruct = *(kstl::vector<RefCountedBaseClass*>*)myAttachedCoreItem->getContainerStruct();

	if (myVectorIterator != vectorstruct.end())
	{
		return ((CoreItem*)(*myVectorIterator));
	}

	return KigsCore::Instance()->NotFoundCoreItem();
}
