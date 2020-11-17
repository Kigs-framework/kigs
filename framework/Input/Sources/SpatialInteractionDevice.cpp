#include "SpatialInteractionDevice.h"

IMPLEMENT_CLASS_INFO(SpatialInteractionDevice);

std::optional<v3f> SpatialInteractionDevice::GetInteractionPosition(u32 ID)const
{
	auto found = mInteractions.find(ID);
	if (found == mInteractions.end())
		return {};
	return found->second->Position;
}

std::shared_ptr<Interaction> SpatialInteractionDevice::GetInteraction(u32 ID) const
{
	auto found = mInteractions.find(ID);
	if (found == mInteractions.end())
		return {};
	return found->second;
}

std::optional<SourceState> SpatialInteractionDevice::GetInteractionState(u32 ID) const
{
	auto found = mInteractions.find(ID);
	if (found == mInteractions.end())
		return {};
	return found->second->state;
}