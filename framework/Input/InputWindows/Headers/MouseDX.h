#ifndef _MOUSEDX_H_
#define _MOUSEDX_H_

#include "MouseDevice.h"
#include "ModuleInputDX.h"

// ****************************************
// * MouseDX class
// * --------------------------------------
/**
* \file	MouseDX.h
* \class	MouseDX
* \ingroup Input
* \brief Specific DirectX Mouse device.
*
*/
// ****************************************

class	MouseDX : public MouseDevice
{
public:
    DECLARE_CLASS_INFO(MouseDX,MouseDevice,Input)

    MouseDX(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);


	virtual bool	Aquire();
	virtual bool	Release();

	void	UpdateDevice();

	LPDIRECTINPUTDEVICE8& getDirectInputMouse(){return mDirectInputMouse;}

	void	DoInputDeviceDescription();

	void	IncWheelCount(){mWheelCount++;}
	void	IncButtonCount(){mButtonsCount++;}
    
protected:
   virtual ~MouseDX();

	LPDIRECTINPUTDEVICE8 mDirectInputMouse;  
};    

#endif //_MOUSEDX_H_
