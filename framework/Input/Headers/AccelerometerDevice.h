#pragma once
#include "InputDevice.h"

namespace Kigs
{
	namespace Input
	{
		// ****************************************
		// * AccelerometerDevice class
		// * --------------------------------------
		/**
		* \file	AccelerometerDevice.h
		* \class	AccelerometerDevice
		* \ingroup Input
		* \brief Base class for accelerometer
		*
		*/
		// ****************************************

		class AccelerometerDevice : public InputDevice
		{
		public:
			//! buttons list


			DECLARE_ABSTRACT_CLASS_INFO(AccelerometerDevice, InputDevice, Input)

				/**
				 * \brief	constructor
				 * \fn 		MouseDevice(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
				 * \param	name : instance name
				 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
				 */
				AccelerometerDevice(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);


			/**
			 * \brief	retreive current position
			 * \fn 		void			getPos(float& posX,float& posY)
			 * \param	posX : position on x axis (in/out param)
			 * \param	posY : position on y axis (in/out param)
			 */
			void			getPos(float& posX, float& posY, float& posZ) { posX = mAcceleration[0]; posY = mAcceleration[1]; posZ = mAcceleration[2]; }


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
			virtual ~AccelerometerDevice();

			virtual void	Start() = 0;
			virtual void	Stop() = 0;

			// world coordinate acceleration
			v3f		mAcceleration = { 0.0f, 0.0f, 0.0f };

			// device coordinate acceleration
			v3f		mRawAcceleration = { 0.0f, 0.0f, 0.0f };

			// world coordinate integrated velocity
			v3f		mVelocity = { 0.0f, 0.0f, 0.0f };

			// world coordinate integrated position
			v3f		mPosition = { 0.0f, 0.0f, 0.0f };;

			s32		mRate = 0;

			WRAP_ATTRIBUTES(mAcceleration,mRawAcceleration,mVelocity,mPosition,mRate);

		};

	}
}