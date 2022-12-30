#include "PrecompiledHeaders.h"
#include "GeolocationDevice.h"

using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(GeolocationDevice)

GeolocationDevice::GeolocationDevice(const std::string& name, CLASS_NAME_TREE_ARG) : InputDevice(name, PASS_CLASS_NAME_TREE_ARG)
, mIsActive(false)
, mLatitude(*this, false, "Latitude", 0.0)
, mLongitude(*this, false, "Longitude", 0.0)
, mAltitude(*this, false, "Altitude", 0.0)
, mAccuracy(*this, false, "Accuracy", 0.0f)
, mRate(*this, false, "Rate", 1000)
, mMinDistance(*this, false, "MinDistance", 0.0f)
{

}