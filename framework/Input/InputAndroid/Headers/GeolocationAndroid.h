#ifndef _GEOLOCATIONANDROID_H_
#define _GEOLOCATIONANDROID_H_

#include "GeolocationDevice.h"
#include "ModuleInputAndroid.h"

#include <jni.h>

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
	
    GeolocationAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	void	UpdateDevice() override;
	
	bool	Aquire() override;
	bool	Release() override;
	
	void	DoInputDeviceDescription() override;

//	virtual void Activate(bool active, int minTime=10000, float minDistance=0.0f);
	
    
protected:
	virtual ~GeolocationAndroid();  

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

#endif //_GEOLOCATIONANDROID_H_