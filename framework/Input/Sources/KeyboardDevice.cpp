#include "PrecompiledHeaders.h"
#include "KeyboardDevice.h"
#include "DeviceItem.h"

using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(KeyboardDevice)

int KeyboardDevice::GetKey(int key_id)
{
	return getKeyState(key_id).GetTypedValue(int);
}