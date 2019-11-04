#ifndef _MOUSEDX_H_
#define _MOUSEDX_H_

#include "MouseDevice.h"
#include "ModuleInputDX.h"

// ****************************************
// * MouseDX class
// * --------------------------------------
/*!  \class MouseDX
     DirectX mouse management
	 \ingroup InputDX
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

	LPDIRECTINPUTDEVICE8& getDirectInputMouse(){return myDirectInputMouse;}

	void	DoInputDeviceDescription();

	void	IncWheelCount(){myWheelCount++;}
	void	IncButtonCount(){myButtonsCount++;}
    
protected:
   virtual ~MouseDX();

	LPDIRECTINPUTDEVICE8 myDirectInputMouse;  
};    

#endif //_MOUSEDX_H_
