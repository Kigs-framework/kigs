#pragma once

#include "InputDevice.h"
 
namespace Kigs
{
	namespace Input
	{
		// ****************************************
		// * VirtualDevice class
		// * --------------------------------------
		/**
		 * \class	VirtualDevice
		 * \file	VirtualDevice.h
		 * \ingroup Input
		 * \brief	Base class for virtual device ( a device mapping another device ).
		 */
		 // ****************************************
		class VirtualDevice : public InputDevice
		{
		public:
			DECLARE_CLASS_INFO(VirtualDevice, InputDevice, Input)

				/**
				 * \brief	constructor
				 * \fn 		VirtualDevice(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
				 * \param	name : instance name
				 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
				 */
				VirtualDevice(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			/**
			 * \brief	destructor
			 * \fn 		~VirtualDevice();
			 */
			virtual ~VirtualDevice();

			/**
			 * \brief	initialize the device (overloaded function for each devices)
			 * \fn 		virtual void	DoInputDeviceDescription()=0;
			 */
			void	DoInputDeviceDescription() override { ; }

			/**
			 * \brief	update the device
			 * \fn 		virtual void	Update();
			 */
			void	UpdateDevice() override;

		protected:

		};

	}
}
