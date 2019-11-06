#ifndef _JOYSTICKDX_H_
#define _JOYSTICKDX_H_

#include "JoystickDevice.h"
#include "ModuleInputDX.h"

// ****************************************
// * JoystickDX class
// * --------------------------------------
/*!  \class JoystickDX
     DirectX joystick management
	 \ingroup InputDX
*/
// ****************************************

class	JoystickDX : public JoystickDevice
{
public:

    DECLARE_CLASS_INFO(JoystickDX,JoystickDevice,Input)

    JoystickDX(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	LPDIRECTINPUTDEVICE8& getDirectInputJoystick(){return myDirectInputJoystick;}
    
	virtual void	UpdateDevice();

	virtual bool	Aquire();
	virtual bool	Release();

	void	DoInputDeviceDescription();

	void	IncButtonCount() {myButtonsCount++;}
	void	IncAxisCount()   {myAxisCount++;}
	void	IncPOVCount()    {myPovCount++;}

	void UseAxis() { myAxisIndex=1; }
	void UseRotation() { myRotationIndex=1; }


protected:
  virtual ~JoystickDX();

	LPDIRECTINPUTDEVICE8 myDirectInputJoystick;

	int myAxisIndex;
	int myRotationIndex;
  
};    

#endif //_JOYSTICKDX_H_
