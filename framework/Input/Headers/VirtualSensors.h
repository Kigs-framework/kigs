#pragma once
#include "GyroscopeDevice.h"
#include "AccelerometerDevice.h"

// ****************************************
// * VirtualGyroscope class
// * --------------------------------------
/**
 * \class	VirtualGyroscope
 * \file	VirtualSensors.h
 * \ingroup Input
 * \brief	Emulate a gyroscope.
 */
 // ****************************************

class VirtualGyroscope : public GyroscopeDevice
{
public:
	DECLARE_CLASS_INFO(VirtualGyroscope, GyroscopeDevice, ModuleInput);
	
	DECLARE_CONSTRUCTOR(VirtualGyroscope);

	void DoInputDeviceDescription() override {}
	


	void setQuaternion(Quaternion q)
	{
		q.Normalize();
		mRotationQuaternion[0] = q.V.x;
		mRotationQuaternion[1] = q.V.y;
		mRotationQuaternion[2] = q.V.z;
		mRotationQuaternion[3] = q.w;
	}

protected:
	void Start() override {}
	void Stop() override {}
};

// ****************************************
// * VirtualAccelerometer class
// * --------------------------------------
/**
 * \class	VirtualAccelerometer
 * \file	VirtualSensors.h
 * \ingroup Input
 * \brief	Emulate an accelerometer.
 */
 // ****************************************

class VirtualAccelerometer : public AccelerometerDevice
{
public:
	DECLARE_CLASS_INFO(VirtualAccelerometer, AccelerometerDevice, ModuleInput);

	DECLARE_CONSTRUCTOR(VirtualAccelerometer);

	void DoInputDeviceDescription() override {}



	void setAcceleration(Vector3D a)
	{
		mAccX = a.x;
		mAccY = a.y;
		mAccZ = a.z;
	}
protected:
	void Start() override {}
	void Stop() override {}
};

