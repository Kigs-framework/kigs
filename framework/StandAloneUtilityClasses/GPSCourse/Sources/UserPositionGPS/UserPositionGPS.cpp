#include "..\..\Headers\CustomUI\SequenceMap\UIMap.h"
#include "..\..\Headers\UserPositionGPS/UserPositionGPS.h"
#include "Core.h"
#include "UI\UIText.h"
#include "ModuleInput.h"

using namespace Kigs::Gps;

IMPLEMENT_CLASS_INFO(UserPositionGPS);

UserPositionGPS::UserPositionGPS(const std::string & name, CLASS_NAME_TREE_ARG) :
	CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG),
	mCoordGPS_Lat(0),
	mCoordGPS_Long(0),
	mUser(*this, false, "User"),
	mMap(*this, false, "Map")
{
}

void UserPositionGPS::InitModifiable()
{
	CoreModifiable::InitModifiable();

	if (_isInit)
	{
		((UIMap*)(CoreModifiable*)mMap)->GetPositionGPSUser(mCoordGPS_Lat, mCoordGPS_Long);

		RecalculatePosition();		
	}
	else
	{
		UninitModifiable();
	}
}

bool UserPositionGPS::SetCoordGPS(double _Lat, double _Long)
{ 
	if (_Lat == mCoordGPS_Lat && _Long == mCoordGPS_Long)
		return false;

	mCoordGPS_Lat = _Lat;
	mCoordGPS_Long = _Long; 
	RecalculatePosition();
	return true;
}

void UserPositionGPS::SetDirection(Float angle)
{
	((UIImage*)(CoreModifiable*)mUser)->Set_RotationAngle(angle);
}

void UserPositionGPS::RecalculatePosition()
{

	if (((UIMap*)(CoreModifiable*)mMap) && ((UIImage*)(CoreModifiable*)mUser))
	{
		double topLeftLong, botRightLong, topLeftLat, bopRightLat;
		((UIMap*)(CoreModifiable*)mMap)->GetPositionsGPS(topLeftLong, topLeftLat, botRightLong, bopRightLat);
		((CoreModifiable*)mUser)->setArrayValue("Dock", (topLeftLong - mCoordGPS_Long) / (topLeftLong - botRightLong), (topLeftLat - mCoordGPS_Lat) / (topLeftLat - bopRightLat));

	}
}