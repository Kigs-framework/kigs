#ifndef _MOUSEWUP_H_
#define _MOUSEWUP_H_

#include "MouseDevice.h"
#include "ModuleInputWUP.h"

class	MouseWUP : public MouseDevice
{
	public:
    DECLARE_CLASS_INFO(MouseWUP,MouseDevice,Input)
	
		MouseWUP(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	
	virtual bool	Aquire();
	virtual bool	Release();
	
	void	UpdateDevice();
	
	//LPDIRECTINPUTDEVICE8& getDirectInputMouse(){return myDirectInputMouse;}
	
	void	DoInputDeviceDescription();
	
	void	IncWheelCount(){myWheelCount++;}
	void	IncButtonCount(){myButtonsCount++;}
	
	protected:
	virtual ~MouseWUP();
	

	int WheelDelta=0;
	bool LeftPressed=false;
	bool RightPressed=false;
	bool MiddlePressed=false;

};

#endif //_MOUSEWUP_H_
