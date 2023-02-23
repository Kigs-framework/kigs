#include "PrecompiledHeaders.h"
#include "GeolocationDevice.h"

using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(GeolocationDevice)

GeolocationDevice::GeolocationDevice(const std::string& name, CLASS_NAME_TREE_ARG) : InputDevice(name, PASS_CLASS_NAME_TREE_ARG)
, mIsActive(false)
{

}