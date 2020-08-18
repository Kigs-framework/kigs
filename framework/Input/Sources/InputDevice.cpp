#include "PrecompiledHeaders.h"

#include "InputDevice.h"
#include "DeviceItem.h"
#include "Window.h"

IMPLEMENT_CLASS_INFO(InputDevice)

IMPLEMENT_CONSTRUCTOR(InputDevice)
, myDeviceItems(0)
, myDeviceItemsCount(0)
, myAquireCount(0)
, myInputWindow(0)
{
	
}

InputDevice::~InputDevice()
{
	unsigned int i;
	for (i = 0; i < myDeviceItemsCount; i++)
	{
		delete myDeviceItems[i];
	}

	delete[] myDeviceItems;
}

bool InputDevice::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if(item->isSubType(Window::mClassID))
	{
		if (myInputWindow)
		{
			CMSP w(myInputWindow, StealRefTag{});
			removeItem(w PASS_LINK_NAME(linkName));
		}
		myInputWindow = (Window*)item.get();
	}

	return CoreModifiable::addItem(item,pos PASS_LINK_NAME(linkName));
}

bool InputDevice::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	if(item->isSubType(Window::mClassID))
	{
		if (myInputWindow==(Window*)item.get())
			myInputWindow=NULL;		
	}

	return CoreModifiable::removeItem(item PASS_LINK_NAME(linkName));
}

void	InputDevice::InitItems(unsigned int itemCount,DeviceItem** items)
{
	if(!itemCount)
	{
		return;
	}

	myDeviceItemsCount=itemCount;
	myDeviceItems=new DeviceItem*[myDeviceItemsCount];

	unsigned int i;
	for(i=0;i<itemCount;i++)
	{
		myDeviceItems[i]=items[i]->MakeCopy();
	}
}

kfloat	InputDevice::GetItemfState(int itemindex)
{
	if (itemindex >= (int)myDeviceItemsCount)
		return 0;
	return myDeviceItems[itemindex]->getState()->GetTypedValue(float);
}

int		InputDevice::GetItemiState(int itemindex)
{
	if (itemindex >= (int)myDeviceItemsCount)
		return 0;
	return myDeviceItems[itemindex]->getState()->GetTypedValue(int);
}
