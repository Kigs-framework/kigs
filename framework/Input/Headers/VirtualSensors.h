#pragma once
#include "GyroscopeDevice.h"
#include "AccelerometerDevice.h"

namespace Kigs
{
	namespace Input
	{
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



			void setQuaternion(quat q)
			{
				normalize(q);
				mRotationQuaternion=q;
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

			void setAcceleration(const v3f& a)
			{
				mAcceleration = a;
			}
		protected:
			void Start() override {}
			void Stop() override {}
		};

	}
}