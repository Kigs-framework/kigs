#include "PrecompiledHeaders.h"

#include "InputDevice.h"
#include "DeviceItem.h"
#include "Window.h"

IMPLEMENT_CLASS_INFO(InputDevice)

IMPLEMENT_CONSTRUCTOR(InputDevice)
, mDeviceItems(0)
, mDeviceItemsCount(0)
, mAquireCount(0)
, mInputWindow(0)
{
	
}

InputDevice::~InputDevice()
{
	unsigned int i;
	for (i = 0; i < mDeviceItemsCount; i++)
	{
		delete mDeviceItems[i];
	}

	delete[] mDeviceItems;
}

bool InputDevice::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if(item->isSubType(Window::mClassID))
	{
		if (mInputWindow)
		{
			CMSP w(mInputWindow, StealRefTag{});
			removeItem(w PASS_LINK_NAME(linkName));
		}
		mInputWindow = (Window*)item.get();
	}

	return CoreModifiable::addItem(item,pos PASS_LINK_NAME(linkName));
}

bool InputDevice::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	if(item->isSubType(Window::mClassID))
	{
		if (mInputWindow==(Window*)item.get())
			mInputWindow=NULL;		
	}

	return CoreModifiable::removeItem(item PASS_LINK_NAME(linkName));
}

void	InputDevice::InitItems(unsigned int itemCount,DeviceItem** items)
{
	if(!itemCount)
	{
		return;
	}

	mDeviceItemsCount=itemCount;
	mDeviceItems=new DeviceItem*[mDeviceItemsCount];

	unsigned int i;
	for(i=0;i<itemCount;i++)
	{
		mDeviceItems[i]=items[i]->MakeCopy();
	}
}

kfloat	InputDevice::GetItemfState(int itemindex)
{
	if (itemindex >= (int)mDeviceItemsCount)
		return 0;
	return mDeviceItems[itemindex]->getState()->GetTypedValue(float);
}

int		InputDevice::GetItemiState(int itemindex)
{
	if (itemindex >= (int)mDeviceItemsCount)
		return 0;
	return mDeviceItems[itemindex]->getState()->GetTypedValue(int);
}
