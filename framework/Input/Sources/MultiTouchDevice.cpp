#include "PrecompiledHeaders.h"
#include "DeviceItem.h"
#include "MultiTouchDevice.h"

IMPLEMENT_CLASS_INFO(MultiTouchDevice)


int		MultiTouchDevice::getTouchState(int touchIndex)
{
	if (touchIndex >= 0 && touchIndex < mMaxTouch)
	{
		return mDeviceItems[touchIndex*3+2]->getState()->GetTypedValue(int);
	}
	else
	{
		return 0;
	}
}

void	MultiTouchDevice::getTouchPos(int touchIndex, kfloat &posX, kfloat &posY)
{
	if (touchIndex >= 0 && touchIndex < mMaxTouch)
	{
		posX = mDeviceItems[touchIndex * 3 + 0]->getState()->GetTypedValue(float);
		posY = mDeviceItems[touchIndex * 3 + 1]->getState()->GetTypedValue(float);
	}
	else
	{
		posX = 0.f;
		posY = 0.f;
	}
}
