#pragma once

#include "MouseDevice.h"
#include "ModuleInputWUP.h"

namespace Kigs
{
	namespace Input
	{
		// ****************************************
		// * MouseWUP class
		// * --------------------------------------
		/**
		 * \class	MouseWUP
		 * \file	MouseWUP.h
		 * \ingroup Input
		 * \brief	UWP mouse management.
		 */
		 // ****************************************

		class	MouseWUP : public MouseDevice
		{
		public:
			DECLARE_CLASS_INFO(MouseWUP, MouseDevice, Input)
				MouseWUP(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~MouseWUP();

			virtual bool	Aquire();
			virtual bool	Release();

			void	UpdateDevice();

			//LPDIRECTINPUTDEVICE8& getDirectInputMouse(){return myDirectInputMouse;}

			void	DoInputDeviceDescription();

			void	IncWheelCount() { mWheelCount++; }
			void	IncButtonCount() { mButtonsCount++; }

		protected:
			int WheelDelta = 0;
			bool LeftPressed = false;
			bool RightPressed = false;
			bool MiddlePressed = false;

		};
	}
}
