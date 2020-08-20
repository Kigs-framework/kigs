#include "VirtualSensors.h"

IMPLEMENT_CLASS_INFO(VirtualGyroscope);
IMPLEMENT_CLASS_INFO(VirtualAccelerometer);

IMPLEMENT_CONSTRUCTOR(VirtualGyroscope)
{
	mRotationQuaternion[0] = 0;
	mRotationQuaternion[1] = 0;
	mRotationQuaternion[2] = 0;
	mRotationQuaternion[3] = 1;
}


IMPLEMENT_CONSTRUCTOR(VirtualAccelerometer){}
