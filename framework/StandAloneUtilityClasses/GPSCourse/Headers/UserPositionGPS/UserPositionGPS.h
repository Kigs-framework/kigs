#pragma once
#include "GeolocationDevice.h"
#include "Node2D.h"
#include "maReference.h"
// ****************************************
// * UserPositionGPS class
// * --------------------------------------
/**
* \file	UserPositionGPS.h
* \class	UserPositionGPS
* \ingroup Geoloc
* \brief	 An UserPositionGPS is the class for placed the user on a map
* \author	König Jolan
* \version ukn
* \date	14/02/16
*
* Just an object to represent the user, the image on the map and its coordinates GPS
*/
// ****************************************

class UserPositionGPS :
	public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(UserPositionGPS, CoreModifiable, GeoLoc);


	UserPositionGPS(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	bool SetCoordGPS(kdouble _Lat, kdouble _Long);
	void SetDirection(Float angle);

protected:
	void InitModifiable() override;

	void RecalculatePosition();

	maReference mMap;
	maReference mUser;

	kdouble mCoordGPS_Lat;
	kdouble mCoordGPS_Long;	
};