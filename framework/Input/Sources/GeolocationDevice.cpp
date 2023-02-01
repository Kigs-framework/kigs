#include "PrecompiledHeaders.h"
#include "GeolocationDevice.h"

using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(GeolocationDevice)

GeolocationDevice::GeolocationDevice(const std::string& name, CLASS_NAME_TREE_ARG) : InputDevice(name, PASS_CLASS_NAME_TREE_ARG)
, mIsActive(false)
, mLatitude(*this, "Latitude", 0.0)
, mLongitude(*this, "Longitude", 0.0)
, mAltitude(*this, "Altitude", 0.0)
, mAccuracy(*this, "Accuracy", 0.0f)
, mRate(*this, "Rate", 1000)
, mMinDistance(*this, "MinDistance", 0.0f)
{

}