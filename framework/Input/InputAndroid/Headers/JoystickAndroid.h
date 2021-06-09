#ifndef _JOYSTICKANDROID_H_
#define _JOYSTICKANDROID_H_

#include "JoystickDevice.h"
#include "ModuleInputAndroid.h"
#include <jni.h>

// ****************************************
// * JoystickAndroid class
// * --------------------------------------
/**
* \file	JoystickAndroid.h
* \class	JoystickAndroid
* \ingroup Input
* \brief Android Joystick management.
*
*/
// ****************************************

class	JoystickAndroid : public JoystickDevice
{
public:

    DECLARE_CLASS_INFO(JoystickAndroid,JoystickDevice,Input)

    JoystickAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
   
	void	UpdateDevice() override;

	void	DoInputDeviceDescription() override;

	void	IncButtonCount(){mButtonsCount++;}
	void	IncAxisCount(){mAxisCount++;}
	void	IncPOVCount(){mPovCount++;}
 virtual ~JoystickAndroid();
 

protected:
  
   jclass		mActivityClass;
   jmethodID 	mGetBackKey;
   jmethodID 	mGetMenuKey;
 
};    

#endif //_JOYSTICKANDROID_H_
