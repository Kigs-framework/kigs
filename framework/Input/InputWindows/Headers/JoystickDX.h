#ifndef _JOYSTICKDX_H_
#define _JOYSTICKDX_H_

#include "JoystickDevice.h"
#include "ModuleInputDX.h"


// ****************************************
// * JoystickDX class
// * --------------------------------------
/**
* \file	JoystickDX.h
* \class	JoystickDX
* \ingroup Input
* \brief Specific DirectX joystick device.
*
*/
// ****************************************

class	JoystickDX : public JoystickDevice
{
public:

    DECLARE_CLASS_INFO(JoystickDX,JoystickDevice,Input)
    JoystickDX(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~JoystickDX();

	LPDIRECTINPUTDEVICE8& getDirectInputJoystick(){return mDirectInputJoystick;}
    
	virtual void	UpdateDevice();

	virtual bool	Aquire();
	virtual bool	Release();

	void	DoInputDeviceDescription();

	void	IncButtonCount() {mButtonsCount++;}
	void	IncAxisCount()   {mAxisCount++;}
	void	IncPOVCount()    {mPovCount++;}

	void UseAxis() { mAxisIndex=1; }
	void UseRotation() { mRotationIndex=1; }


protected:
	LPDIRECTINPUTDEVICE8 mDirectInputJoystick;

	int mAxisIndex;
	int mRotationIndex;
  
};    

#endif //_JOYSTICKDX_H_
