#include "..\Headers\GPSTrigger.h"
#include "Core.h"
#include "ModuleInput.h"
#include "NotificationCenter.h"

IMPLEMENT_CLASS_INFO(GPSTrigger);

GPSTrigger::GPSTrigger(const kstl::string & name, CLASS_NAME_TREE_ARG) :
	CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG),
	mCoordGPS_Lat(0),
	mCoordGPS_Long(0)
	, mCoordonateInitial_Lat(*this, false, LABEL_AND_ID(CoordonateInitial_Lat), 0)
	, mCoordonateInitial_Long(*this, false, LABEL_AND_ID(CoordonateInitial_Long), 0)
	, mDistMax(*this, false, LABEL_AND_ID(DistMax), 10)
	, mGPS(NULL)

{
}

void GPSTrigger::InitModifiable()
{
	CoreModifiable::InitModifiable();

	if (_isInit)
	{
		ModuleInput* input = (ModuleInput*)KigsCore::GetModule("ModuleInput");
		mGPS = input->GetGeolocation();
		if (mGPS)
			mGPS->Aquire();
	}
	else
	{
		UninitModifiable();
	}
}

void GPSTrigger::ProtectedDestroy()
{
	if (mGPS)
		mGPS->Destroy();

	CoreModifiable::Destroy();
}

void GPSTrigger::Update(const Timer& a_Timer, void* addParam)
{
	CoreModifiable::Update(a_Timer, addParam);

	if (mGPS == nullptr)
		return;

	double lat, lon;
	mGPS->GetCoord(lat, lon);
	CheckDistance(lat,lon);
}


void GPSTrigger::CheckDistance(kdouble _lat, kdouble _long)
{
	kdouble rlat1 = PI_180 * mCoordonateInitial_Lat;
	kdouble rlat2 = PI_180 * _lat;
	kdouble rlon1 = PI_180 * mCoordonateInitial_Long;
	kdouble rlon2 = PI_180 * _long;

	kdouble theta = mCoordonateInitial_Long - _long;
	kdouble rtheta = PI_180 * theta;

	kdouble dist = sin(rlat1) * sin(rlat2) + cos(rlat1) * cos(rlat2) * cos(rtheta);
	dist = acos(dist);
	dist = dist * 180 / PI;
	dist = dist * 60 * 1.1515;


	dist = dist * 1.609344 * 1000;

	if (dist > mDistMax)
	{
		KigsCore::GetNotificationCenter()->postNotificationName("BackSequence", this, NULL);
	}
}