#ifndef _KEYBOARDVIRTUALJOYSTICKDX_H_
#define _KEYBOARDVIRTUALJOYSTICKDX_H_

#include "dll.h"
#include "JoystickDevice.h"
#include "ModuleInputDX.h"

class KeyboardDevice;

class	KeyBoardVirtualJoystickDX : public JoystickDevice
{
	public:
	
    DECLARE_CLASS_INFO(KeyBoardVirtualJoystickDX,JoystickDevice,Input)
	
		KeyBoardVirtualJoystickDX(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	virtual void	UpdateDevice();
	
	virtual bool	Aquire();
	virtual bool	Release();
	
	void	DoInputDeviceDescription();
	
	void	IncButtonCount(){mButtonsCount++;}
	void	IncAxisCount(){mAxisCount++;}
	void	IncPOVCount(){mPovCount++;}
	
	
	protected:
	virtual ~KeyBoardVirtualJoystickDX();
	
	KeyboardDevice*	mKeyboard;  
	
};    

#endif //_KEYBOARDVIRTUALJOYSTICKDX_H_
