#pragma once

#include "InputDevice.h"

namespace Kigs
{
	namespace Input
	{
		// ****************************************
		// * JoystickDevice class
		// * --------------------------------------
		/**
		 * \class	JoystickDevice
		 * \file	JoystickDevice.h
		 * \ingroup Input
		 * \brief	Generic Joystick / pad management.
		 */
		 // ****************************************
		class JoystickDevice : public InputDevice
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(JoystickDevice, InputDevice, Input)

				/**
				 * \brief	constructor
				 * \fn 		JoystickDevice(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
				 * \param	name : instance name
				 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
				 */
				JoystickDevice(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			/**
			 * \brief	update the joystick
			 * \fn 		virtual void	Update();
			 */
			void	UpdateDevice() override {};

			/**
			 * \brief	button count for this joystick
			 * \fn 		int				getButtonsCount()
			 * \return	the number of buttons
			 */
			int				getButtonsCount() { return mButtonsCount; }

			/**
			 * \brief	axis count for this joystick
			 * \fn 		int				getAxisCount()
			 * \return	the number of axis
			 */
			int				getAxisCount() { return mAxisCount; }

			/**
			 * \brief	POV count for this joystick
			 * \fn 		int				getPovCount()
			 * \return	the number of POV
			 */
			int				getPovCount() { return mPovCount; }

			/**
			 * \brief	retreive given button state
			 * \fn 		int				getButtonState(int button)
			 * \param	button : given button
			 * \return	state of the given button
			 */
			int				getButtonState(int button)
			{
				return GetItemiState(button);
			}

			/**
			 * \brief	retreive the x axis state
			 * \fn 		float				getXAxisState(int index)
			 * \param	index : index of the x axis
			 * \return	state of the x axis
			 */
			float				getXAxisState(int index);

			/**
			 * \brief	retreive the y axis state
			 * \fn 		float				getYAxisState(int index)
			 * \param	index : index of the y axis
			 * \return	state of the y axis
			 */
			float				getYAxisState(int index);

			/**
			 * \brief	retreive the z axis state
			 * \fn 		float				getZAxisState(int index)
			 * \param	index : index of the z axis
			 * \return	state of the y axis
			 */
			float				getZAxisState(int index);

			/**
			 * \brief	retreive the POV state
			 * \fn 		float				getPOVState(int index)
			 * \param	index : index of POV
			 * \return	state of POV
			 */
			int				getPOVState(int index)
			{
				return GetItemiState(mButtonsCount + mAxisCount + index);
			}

			/**
			 * \brief	retreive the y axis state
			 * \fn 		float				getYAxisState(int index)
			 * \param	index : index of the y axis
			 * \return	state of the y axis
			 */
			float				getRorationState(int index)
			{
				return GetItemfState(mButtonsCount + mAxisCount + mPovCount + index);
			}

		protected:
			//! number of button
			unsigned int mButtonsCount;
			//! number of axis
			unsigned int mAxisCount;
			//! number of pov
			unsigned int mPovCount;

			/**
			 * \brief	destructor
			 * \fn 		~JoystickDevice();
			 */
			virtual ~JoystickDevice();
		};

	}
}
