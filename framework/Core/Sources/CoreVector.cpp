#include "PrecompiledHeaders.h"
#include "CoreVector.h"

CoreItemSP CoreVectorIterator::operator*() const
{
	kstl::vector<CoreItemSP>&	vectorstruct = *(kstl::vector<CoreItemSP>*)myAttachedCoreItem->getContainerStruct();

	if (myVectorIterator != vectorstruct.end())
	{
		return (*myVectorIterator);
	}

	return CoreItemSP(nullptr);
}
