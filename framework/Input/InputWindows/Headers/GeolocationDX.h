#ifndef _GeolocationDX_H_
#define _GeolocationDX_H_

#include "GeolocationDevice.h"
#include "ModuleInputDX.h"

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
	
    GeolocationDX(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	virtual void	UpdateDevice();
	
	void	DoInputDeviceDescription();
    
protected:
	virtual ~GeolocationDX();  

	virtual void Start() {}
	virtual void Stop() {}
	
};    

#endif //_GeolocationDX_H_