#pragma once

#include "GeolocationDevice.h"
#include "ModuleInputDX.h"

namespace Kigs
{
	namespace Input
	{

		// ****************************************
		// * GeolocationDX class
		// * --------------------------------------
		/**
		* \file	GeolocationDX.h
		* \class	GeolocationDX
		* \ingroup Input
		* \brief Specific DirectX geolocation device.
		*
		*/
		// ****************************************
		class	GeolocationDX : public GeolocationDevice
		{
		public:
			DECLARE_CLASS_INFO(GeolocationDX, GeolocationDevice, Input)
				GeolocationDX(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~GeolocationDX();

			virtual void	UpdateDevice();

			void	DoInputDeviceDescription();

		protected:
			virtual void Start() {}
			virtual void Stop() {}

		};
	}
}