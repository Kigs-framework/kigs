#pragma once

#include "InputDevice.h"

namespace Kigs
{
	namespace Input
	{
		// ****************************************
		// * GeolocationDevice class
		// * --------------------------------------
		/**
		 * \class	GeolocationDevice
		 * \file	GeolocationDevice.h
		 * \ingroup Input
		 * \brief	GPS management.
		 */
		 // ****************************************
		class GeolocationDevice : public InputDevice
		{
		public:
			//! buttons list


			DECLARE_ABSTRACT_CLASS_INFO(GeolocationDevice, InputDevice, Input)

				/**
				* \brief	constructor
				* \fn 		GeolocationDevice(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
				* \param	name : instance name
				* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
				*/
				GeolocationDevice(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);


			/**
			* \brief	retreive current position
			* \fn 		void			getCoord(double& latitude, double& longitude)
			* \param	latitude : latitude (in/out param)
			* \param	longitude : longitude (in/out param)
			*/
			void			GetCoord(double& latitude, double& longitude) { latitude = mLatitude; longitude = mLongitude; }

			/**
			* \brief	compute current position
			* \fn 		virtual void	Update();
			*/
			void	UpdateDevice() override { ; }

		protected:
			/**
			* \brief	destructor
			* \fn 		~MouseDevice();
			*/
			virtual ~GeolocationDevice() { ; }

			virtual void	Start() = 0;
			virtual void	Stop() = 0;


			bool		mIsActive;
			maDouble	mLatitude;
			maDouble	mLongitude;
			maDouble	mAltitude;
			maFloat		mAccuracy;
			maInt		mRate;
			maFloat		mMinDistance;
		};




		inline double gps_distance(double* a, double* b)
		{
			double R = 6378.137; // Radius of earth in KM
			double dLat = (b[0] - a[0]) * PI / 180.0;
			double dLon = (b[1] - a[1]) * PI / 180.0;
			double aa = sin(dLat / 2) * sin(dLat / 2) + cos(a[0] * PI / 180) * cos(b[0] * PI / 180.0) * sin(dLon / 2) * sin(dLon / 2);
			return 2.0 * atan2(sqrt(aa), sqrt(1 - aa)) * R * 1000.0; // meters
		}

		inline double gps_angle(double* a, double* b)
		{
			double dy = a[0] - b[0];
			double dx = cos(PI / 180.0 * b[0]) * (a[1] - b[1]);
			return atan2(dy, dx);
		}

		inline Vector3D gps_position(double* origin, double* coords)
		{
			double angle = gps_angle(coords, origin);
			double dist = gps_distance(origin, coords);
			return Vector3D((float)(dist * cos(angle)), (float)(dist * sin(angle)), 0.0f);
		}


	}
}