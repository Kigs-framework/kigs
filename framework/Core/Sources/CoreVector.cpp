#include "PrecompiledHeaders.h"
#include "CoreVector.h"

using namespace Kigs::Core;
CoreItemSP CoreVectorIterator::operator*() const
{
	std::vector<CoreItemSP>&	vectorstruct = *(std::vector<CoreItemSP>*)mAttachedCoreItem->getContainerStruct();

	if (mVectorIterator != vectorstruct.end())
	{
		return (*mVectorIterator);
	}

	return CoreItemSP(nullptr);
}
