#ifndef _VIRTUALINPUTDEVICE_H_
#define _VIRTUALINPUTDEVICE_H_

#include "CoreModifiable.h"
//#include "InputDevice.h"

class VirtualInputDevice : public CoreModifiable
{
public:
    
    DECLARE_ABSTRACT_CLASS_INFO(VirtualInputDevice,CoreModifiable,Input)
    
    VirtualInputDevice(const std::string& name);

	void	update(kdouble time);

protected:
    virtual ~VirtualInputDevice();           
};    

#endif //_VIRTUALINPUTDEVICE_H_
