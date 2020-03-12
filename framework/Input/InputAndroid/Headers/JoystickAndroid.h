#ifndef _JOYSTICKANDROID_H_
#define _JOYSTICKANDROID_H_

#include "JoystickDevice.h"
#include "ModuleInputAndroid.h"

// ****************************************
// * JoystickAndroid class
// * --------------------------------------
/*!  \class JoystickAndroid
     Android joystick management
	 \ingroup InputAndroid
*/
// ****************************************

#include <jni.h>

class	JoystickAndroid : public JoystickDevice
{
public:

    DECLARE_CLASS_INFO(JoystickAndroid,JoystickDevice,Input)

    JoystickAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
   
	void	UpdateDevice() override;

	void	DoInputDeviceDescription() override;

	void	IncButtonCount(){myButtonsCount++;}
	void	IncAxisCount(){myAxisCount++;}
	void	IncPOVCount(){myPovCount++;}


protected:
  virtual ~JoystickAndroid();
  
   jclass		myActivityClass;
   jmethodID 	getBackKey;
   jmethodID 	getMenuKey;
 
};    

#endif //_JOYSTICKANDROID_H_
