#pragma once

#include "InputDevice.h"
namespace Kigs
{
	namespace Input
	{
		// ****************************************
		// * CompassDevice class
		// * --------------------------------------
		/**
		* \file	CompassDevice.h
		* \class	CompassDevice
		* \ingroup Input
		* \brief Base class for compass management.
		*
		*/
		// ****************************************

		class CompassDevice : public InputDevice
		{
		public:
			//! buttons list


			DECLARE_ABSTRACT_CLASS_INFO(CompassDevice, InputDevice, Input)

				/**
				 * \brief	constructor
				 * \fn 		CompassDevice(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
				 * \param	name : instance name
				 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
				 */
				CompassDevice(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);


			/**
			 * \brief	retreive current position
			 * \fn 		void			getPos(float& posX,float& posY)
			 * \param	posX : position on x axis (in/out param)
			 * \param	posY : position on y axis (in/out param)
			 */
			void			getPos(float& posX, float& posY, float& posZ) { posX = mCompasCoords[0]; posY = mCompasCoords[1]; posZ = mCompasCoords[2]; }


			/**
			 * \brief	compute current position
			 * \fn 		virtual void	Update();
			 */
			void	UpdateDevice() override;



		protected:
			/**
			 * \brief	destructor
			 * \fn 		~MouseDevice();
			 */
			virtual ~CompassDevice();

			virtual void	Start() = 0;
			virtual void	Stop() = 0;

			v3f		mCompasCoords = { 0.0f, 0.0f, 0.0f };
			s32		mRate = 0;

			WRAP_ATTRIBUTES(mCompasCoords, mRate);

		};

	}
}