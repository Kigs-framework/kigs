#ifndef _MOUSEIPHONE_H_
#define _MOUSEIPHONE_H_

#include "MouseDevice.h"
#include "ModuleInputIPhone.h"

// ****************************************
// * MouseIPhone class
// * --------------------------------------
/*!  \class MouseIPhone
     IPhone mouse management (touche screen)
  \ingroup InputIPhone
*/
// ****************************************

class	MouseIPhone : public MouseDevice
{
	public:
    DECLARE_CLASS_INFO(MouseIPhone,MouseDevice,Input)
	
		MouseIPhone(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	
	virtual bool	Aquire();
	virtual bool	Release();
	
	void	UpdateDevice();
	
	void	DoInputDeviceDescription();
	
	void	IncWheelCount(){myWheelCount++;}
	void	IncButtonCount(){myButtonsCount++;}
    
	protected:
	virtual ~MouseIPhone();   
	
	int m_FrameCountSinceLastValidTouched;
};    

#endif //_MOUSEIPHONE_H_
