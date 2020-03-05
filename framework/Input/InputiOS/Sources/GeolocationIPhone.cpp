/*
 *  GeolocationIPhone.cpp
 *  IPhoneInputIPhone
 *
 *  Created by Damien on 11/03/15.
 *
 */

#include "GeolocationIPhone.h"
#include "Core.h"
#include "DeviceItem.h"

/****** Geolocalisation ******/
struct structGeolocation{
    double latitude;
    double longitude;
    double altitude;
    float accuracy;
};

extern structGeolocation currentGeo;

IMPLEMENT_CLASS_INFO(GeolocationIPhone)

GeolocationIPhone::GeolocationIPhone(const kstl::string& name,CLASS_NAME_TREE_ARG) : GeolocationDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
	mLatitude = 0.0;
	mLongitude = 0.0;
	mAltitude = 0.0;
	mAccuracy = 0.f;
}

GeolocationIPhone::~GeolocationIPhone()
{
	// myKigsGeolocation=0;
}

bool	GeolocationIPhone::Aquire()
{
	if (GeolocationDevice::Aquire())
	{
		Start();
		return true;
	}
	return false;
}

bool	GeolocationIPhone::Release()
{
	if (GeolocationDevice::Release())
	{
		Stop();
		return true;
	}
	return false;
}

void	GeolocationIPhone::UpdateDevice()
{
	if (mIsActive)
	{
		mLatitude = currentGeo.latitude;
		mLongitude = currentGeo.longitude;
		mAltitude = currentGeo.altitude;
		mAccuracy = currentGeo.accuracy;
	}
}

void	GeolocationIPhone::DoInputDeviceDescription()
{
	GeolocationDevice::InitModifiable();
}

void GeolocationIPhone::Start()
{
	// TODO
	if (!mIsActive)
	{
		mIsActive = true;
	}
}


void GeolocationIPhone::Stop()
{
	// TODO
	if (mIsActive)
	{
		mIsActive = false;
	}
}

