#pragma once
#include "GeolocationDevice.h"
#include "ModuleInputAndroid.h"

#include <jni.h>

namespace Kigs
{
	namespace Input
	{
		// ****************************************
		// * GeolocationAndroid class
		// * --------------------------------------
		/**
		* \file	GeolocationAndroid.h
		* \class	GeolocationAndroid
		* \ingroup Input
		* \brief Android GPS management.
		*
		*/
		// ****************************************

		class	GeolocationAndroid : public GeolocationDevice
		{
		public:
			DECLARE_CLASS_INFO(GeolocationAndroid, GeolocationDevice, Input)

				GeolocationAndroid(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			void	UpdateDevice() override;

			bool	Aquire() override;
			bool	Release() override;

			void	DoInputDeviceDescription() override;

			//	virtual void Activate(bool active, int minTime=10000, float minDistance=0.0f);

			virtual ~GeolocationAndroid();

		protected:

			void	Start() override;
			void	Stop() override;


			jmethodID 	mGetLatitude;
			jmethodID 	mGetLongitude;
			jmethodID 	mGetAltitude;
			jmethodID 	mGetAccuracy;
			jmethodID	mGetActive;
			jmethodID	mSetActive;
			jclass		mKigsGeolocation;

		};
	}
}