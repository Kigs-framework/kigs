#pragma once

#include "MouseDevice.h"
#include "ModuleInputDX.h"

namespace Kigs
{
	namespace Input
	{
		// ****************************************
		// * MouseDX class
		// * --------------------------------------
		/**
		* \file	MouseDX.h
		* \class	MouseDX
		* \ingroup Input
		* \brief Specific DirectX Mouse device.
		*
		*/
		// ****************************************

		class	MouseDX : public MouseDevice
		{
		public:
			DECLARE_CLASS_INFO(MouseDX, MouseDevice, Input)
				MouseDX(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~MouseDX();

			virtual bool	Aquire();
			virtual bool	Release();

			void	UpdateDevice();

			LPDIRECTINPUTDEVICE8& getDirectInputMouse() { return mDirectInputMouse; }

			void	DoInputDeviceDescription();

			void	IncWheelCount() { mWheelCount++; }
			void	IncButtonCount() { mButtonsCount++; }

		protected:
			LPDIRECTINPUTDEVICE8 mDirectInputMouse;
		};
	}
}
