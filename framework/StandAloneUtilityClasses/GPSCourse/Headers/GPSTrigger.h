#pragma once
#include "GeolocationDevice.h"

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


	GPSTrigger(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	bool SetCoordGPS(kdouble _Lat, kdouble _Long);

protected:
	void InitModifiable()override;
	void ProtectedDestroy()override;
	void Update(const Timer& a_Timer, void* addParam)override;
	void CheckDistance(kdouble _lat, kdouble _long);

	GeolocationDevice* mGPS;

	maDouble mCoordonateInitial_Lat;
	maDouble mCoordonateInitial_Long;
	maDouble mDistMax;

	kdouble mCoordGPS_Lat;
	kdouble mCoordGPS_Long;	
};