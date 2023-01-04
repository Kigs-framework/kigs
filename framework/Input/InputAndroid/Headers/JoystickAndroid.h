#pragma once

#include "JoystickDevice.h"
#include "ModuleInputAndroid.h"
#include <jni.h>

namespace Kigs
{
	namespace Input
	{
		// ****************************************
		// * JoystickAndroid class
		// * --------------------------------------
		/**
		* \file	JoystickAndroid.h
		* \class	JoystickAndroid
		* \ingroup Input
		* \brief Android Joystick management.
		*
		*/
		// ****************************************

		class	JoystickAndroid : public JoystickDevice
		{
		public:

			DECLARE_CLASS_INFO(JoystickAndroid, JoystickDevice, Input)

				JoystickAndroid(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			void	UpdateDevice() override;

			void	DoInputDeviceDescription() override;

			void	IncButtonCount() { mButtonsCount++; }
			void	IncAxisCount() { mAxisCount++; }
			void	IncPOVCount() { mPovCount++; }
			virtual ~JoystickAndroid();


		protected:

			jclass		mActivityClass;
			jmethodID 	mGetBackKey;
			jmethodID 	mGetMenuKey;

		};

	}
}