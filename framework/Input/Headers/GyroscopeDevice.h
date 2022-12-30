#pragma once

#include "InputDevice.h"

namespace Kigs
{
	namespace Input
	{
		// ****************************************
		// * GyroscopeDevice class
		// * --------------------------------------
		/**
		 * \class	GyroscopeDevice
		 * \file	GyroscopeDevice.h
		 * \ingroup Input
		 * \brief	Device orientation management.
		 */
		 // ****************************************

		class GyroscopeDevice : public InputDevice
		{
		public:
			//! buttons list


			DECLARE_ABSTRACT_CLASS_INFO(GyroscopeDevice, InputDevice, Input)

				/**
				 * \brief	constructor
				 * \fn 		MouseDevice(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
				 * \param	name : instance name
				 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
				 */
				GyroscopeDevice(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);


			/**
			 * \brief	retreive current rotation matrix
			 * \fn 		void			getPos(float* mat )
			 * \param	posX : position on x axis (in/out param)
			 * \param	posY : position on y axis (in/out param)
			 */
			void			getRotationVelocity(float& X, float& Y, float& Z) { X = mRotationVelocity[0]; Y = mRotationVelocity[1]; Z = mRotationVelocity[2]; }

			void			getRotationQuaternion(float& x, float& y, float& z, float& w) {
				x = mRotationQuaternion[0];
				y = mRotationQuaternion[1];
				z = mRotationQuaternion[2];
				w = mRotationQuaternion[3];
			}
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
			virtual ~GyroscopeDevice();

			virtual void	Start() = 0;
			virtual void	Stop() = 0;


			maVect3DF	mRotationVelocity;
			maVect4DF   mRotationQuaternion;

			maInt		mRate;

		};

	}
}