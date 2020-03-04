#ifndef _ACCELEROMETERIPHONE_H_
#define _ACCELEROMETERIPHONE_H_

#include "AccelerometerDevice.h"
#include "ModuleInputIPhone.h"

class	AccelerometerIPhone : public AccelerometerDevice
{
	public:
    DECLARE_CLASS_INFO(AccelerometerIPhone,AccelerometerDevice,Input)
	
		AccelerometerIPhone(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	virtual void	UpdateDevice();
	
	virtual bool	Aquire();
	virtual bool	Release();
	
	void	DoInputDeviceDescription();
    
	protected:
	virtual ~AccelerometerIPhone();  
	
	virtual void	Start();
	virtual void	Stop();
	
	
};    

#endif //_ACCELEROMETERIPHONE_H_