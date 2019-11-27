#ifndef _MOUSEJAVASCRIPT_H_
#define _MOUSEJAVASCRIPT_H_

#include "MouseDevice.h"
#include "ModuleInputJavascript.h"

// ****************************************
// * MouseJavascript class
// * --------------------------------------
/*!  \class MouseJavascript
     mouse management
  \ingroup InputJavascript
*/
// ****************************************

class	MouseJavascript : public MouseDevice
{
	public:
    DECLARE_CLASS_INFO(MouseJavascript,MouseDevice,Input)
	
		MouseJavascript(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	bool	Aquire() override;
	bool	Release() override;
	
	void	UpdateDevice() override;

	void	DoInputDeviceDescription() override;
	
	void	IncWheelCount(){myWheelCount++;}
	void	IncButtonCount(){myButtonsCount++;}
    
	protected:
	virtual ~MouseJavascript();   
	
	
};    

#endif //_MOUSEJAVASCRIPT_H_
