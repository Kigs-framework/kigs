#include "VirtualSensors.h"

IMPLEMENT_CLASS_INFO(VirtualGyroscope);
IMPLEMENT_CLASS_INFO(VirtualAccelerometer);

IMPLEMENT_CONSTRUCTOR(VirtualGyroscope)
{
	myQuaternion[0] = 0;
	myQuaternion[1] = 0;
	myQuaternion[2] = 0;
	myQuaternion[3] = 1;
}


IMPLEMENT_CONSTRUCTOR(VirtualAccelerometer){}
