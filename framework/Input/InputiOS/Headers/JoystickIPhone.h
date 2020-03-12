#ifndef _JOYSTICKIPHONE_H_
#define _JOYSTICKIPHONE_H_

#include "JoystickDevice.h"
#include "ModuleInputIPhone.h"

// ****************************************
// * JoystickIPhone class
// * --------------------------------------
/*!  \class JoystickIPhone
     IPhone joystick management
  \ingroup InputIPhone
*/
// ****************************************

class	JoystickIPhone : public JoystickDevice
{
	public:
	
    DECLARE_CLASS_INFO(JoystickIPhone,JoystickDevice,Input)
	
		JoystickIPhone(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	virtual void	UpdateDevice();
	
	
	void	DoInputDeviceDescription();
	
	void	IncButtonCount(){myButtonsCount++;}
	void	IncAxisCount(){myAxisCount++;}
	void	IncPOVCount(){myPovCount++;}
	
	
	protected:
	virtual ~JoystickIPhone();
	
};    

#endif //_JOYSTICKIPHONE_H_
