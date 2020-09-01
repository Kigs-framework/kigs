#include "PrecompiledHeaders.h"
#include "CoreVector.h"

CoreItemSP CoreVectorIterator::operator*() const
{
	kstl::vector<CoreItemSP>&	vectorstruct = *(kstl::vector<CoreItemSP>*)mAttachedCoreItem->getContainerStruct();

	if (mVectorIterator != vectorstruct.end())
	{
		return (*mVectorIterator);
	}

	return CoreItemSP(nullptr);
}
