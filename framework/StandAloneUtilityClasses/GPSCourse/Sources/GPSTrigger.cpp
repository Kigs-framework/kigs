#include "..\Headers\GPSTrigger.h"
#include "Core.h"
#include "ModuleInput.h"
#include "NotificationCenter.h"

using namespace Kigs::Gps; 
using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(GPSTrigger);

GPSTrigger::GPSTrigger(const std::string & name, CLASS_NAME_TREE_ARG) :
	CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG),
	mCoordGPS_Lat(0),
	mCoordGPS_Long(0)
	, mCoordonateInitial_Lat(*this, false, "CoordonateInitial_Lat", 0)
	, mCoordonateInitial_Long(*this, false, "CoordonateInitial_Long", 0)
	, mDistMax(*this, false, "DistMax", 10)
	, mGPS(NULL)

{
}

void GPSTrigger::InitModifiable()
{
	CoreModifiable::InitModifiable();

	if (_isInit)
	{
		auto input = KigsCore::GetModule<Input::ModuleInput>();
		mGPS = input->GetGeolocation();
		if (mGPS)
			mGPS->Aquire();
	}
	else
	{
		UninitModifiable();
	}
}



void GPSTrigger::Update(const Time::Timer& a_Timer, void* addParam)
{
	CoreModifiable::Update(a_Timer, addParam);

	if (mGPS == nullptr)
		return;

	double lat, lon;
	mGPS->GetCoord(lat, lon);
	CheckDistance(lat,lon);
}


void GPSTrigger::CheckDistance(double _lat, double _long)
{
	double rlat1 = PI_180 * mCoordonateInitial_Lat;
	double rlat2 = PI_180 * _lat;
	double rlon1 = PI_180 * mCoordonateInitial_Long;
	double rlon2 = PI_180 * _long;

	double theta = mCoordonateInitial_Long - _long;
	double rtheta = PI_180 * theta;

	double dist = sin(rlat1) * sin(rlat2) + cos(rlat1) * cos(rlat2) * cos(rtheta);
	dist = acos(dist);
	dist = dist * 180 / PI;
	dist = dist * 60 * 1.1515;


	dist = dist * 1.609344 * 1000;

	if (dist > mDistMax)
	{
		KigsCore::GetNotificationCenter()->postNotificationName("BackSequence", this, NULL);
	}
}