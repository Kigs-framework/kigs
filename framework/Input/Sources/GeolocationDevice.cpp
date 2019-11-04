#include "PrecompiledHeaders.h"
#include "GeolocationDevice.h"

IMPLEMENT_CLASS_INFO(GeolocationDevice)

GeolocationDevice::GeolocationDevice(const kstl::string& name, CLASS_NAME_TREE_ARG) : InputDevice(name, PASS_CLASS_NAME_TREE_ARG)
, mIsActive(false)
, mLatitude(*this, false, LABEL_AND_ID(Latitude), KDOUBLE_CONST(0.0))
, mLongitude(*this, false, LABEL_AND_ID(Longitude), KDOUBLE_CONST(0.0))
, mAltitude(*this, false, LABEL_AND_ID(Altitude), KDOUBLE_CONST(0.0))
, mAccuracy(*this, false, LABEL_AND_ID(Accuracy), KFLOAT_CONST(0.0f))
, myRate(*this, false, LABEL_AND_ID(Rate), 1000)
, myMinDistance(*this, false, LABEL_AND_ID(MinDistance), KFLOAT_CONST(0.0f))
{
	/*setReadOnly("Latitude");
	setReadOnly("Longitude");
	setReadOnly("Altitude");
	setReadOnly("Accuracy");*/
}