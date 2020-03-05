#ifndef _GEOLOCATIONIPHONE_H_
#define _GEOLOCATIONIPHONE_H_

#include "GeolocationDevice.h"
#include "ModuleInputIPhone.h"

class	GeolocationIPhone : public GeolocationDevice
{
	public:
	DECLARE_CLASS_INFO(GeolocationIPhone, GeolocationDevice, Input)
	
		GeolocationIPhone(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	virtual void	UpdateDevice();
	
	virtual bool	Aquire();
	virtual bool	Release();
	
	void	DoInputDeviceDescription();
    
	protected:
	virtual ~GeolocationIPhone();  
	
	virtual void	Start();
	virtual void	Stop();
	
};    

#endif //_GEOLOCATIONIPHONE_H_