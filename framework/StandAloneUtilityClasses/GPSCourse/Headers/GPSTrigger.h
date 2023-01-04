#pragma once
#include "GeolocationDevice.h"

namespace Kigs
{
	namespace Gps
	{
		using namespace Kigs::Core;

		// ****************************************
		// * GPSTrigger class
		// * --------------------------------------
		/**
		* \file	GPSTrigger.h
		* \class	GPSTrigger
		* \ingroup Geoloc
		* \brief	 Used to go back of a sequence if user go to far from the interest point
		* \author	König Jolan
		* \version ukn
		* \date	26/07/16
		*
		*/
		// ****************************************

		class GPSTrigger :
			public CoreModifiable
		{
		public:
			DECLARE_CLASS_INFO(GPSTrigger, CoreModifiable, GeoLoc);


			GPSTrigger(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			bool SetCoordGPS(double _Lat, double _Long);


		protected:
			void InitModifiable()override;
			void Update(const Time::Timer& a_Timer, void* addParam)override;
			void CheckDistance(double _lat, double _long);

			Input::GeolocationDevice* mGPS;

			maDouble mCoordonateInitial_Lat;
			maDouble mCoordonateInitial_Long;
			maDouble mDistMax;

			double mCoordGPS_Lat;
			double mCoordGPS_Long;
		};
	}
}