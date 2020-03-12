#ifndef _MULTITOUCHIPHONE_H_
#define _MULTITOUCHIPHONE_H_

#include "MultiTouchDevice.h"
#include "ModuleInputIPhone.h"

#define MAXTOUCH 5

// ****************************************
// * MultiTouchIPhone class
// * --------------------------------------
/*!  \class MultiTouchIPhone
     IPhone multi touch management (touch screen)
  \ingroup InputIPhone
*/
// ****************************************

class	MultiTouchIPhone : public MultiTouchDevice
{
	public:
    DECLARE_CLASS_INFO(MultiTouchIPhone,MultiTouchDevice,Input)
	
		MultiTouchIPhone(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	
	void	UpdateDevice();
	
	void	DoInputDeviceDescription();
    
	protected:
	virtual ~MultiTouchIPhone();
	
};    

#endif //_MULTITOUCHIPHONE_H_
