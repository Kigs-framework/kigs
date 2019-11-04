#pragma once
#include "GyroscopeDevice.h"
#include "AccelerometerDevice.h"

class VirtualGyroscope : public GyroscopeDevice
{
public:
	DECLARE_CLASS_INFO(VirtualGyroscope, GyroscopeDevice, ModuleInput);
	
	DECLARE_CONSTRUCTOR(VirtualGyroscope);

	void DoInputDeviceDescription() override {}
	


	void setQuaternion(Quaternion q)
	{
		q.Normalize();
		myQuaternion[0] = q.V.x;
		myQuaternion[1] = q.V.y;
		myQuaternion[2] = q.V.z;
		myQuaternion[3] = q.w;
	}

protected:
	void Start() override {}
	void Stop() override {}
};


class VirtualAccelerometer : public AccelerometerDevice
{
public:
	DECLARE_CLASS_INFO(VirtualAccelerometer, AccelerometerDevice, ModuleInput);

	DECLARE_CONSTRUCTOR(VirtualAccelerometer);

	void DoInputDeviceDescription() override {}



	void setAcceleration(Vector3D a)
	{
		myAccX = a.x;
		myAccY = a.y;
		myAccZ = a.z;
	}
protected:
	void Start() override {}
	void Stop() override {}
};

