#ifndef _MULTITOUCHANDROID_H_
#define _MULTITOUCHANDROID_H_

#include "MultiTouchDevice.h"
#include "ModuleInputAndroid.h"

// ****************************************
// * MultiTouchAndroid class
// * --------------------------------------
/*!  \class MultiTouchAndroid
     Android multi touch management (touch screen)
	 \ingroup InputAndroid
*/
// ****************************************

#include <jni.h>

class	MultiTouchAndroid : public MultiTouchDevice
{
public:
    DECLARE_CLASS_INFO(MultiTouchAndroid,MultiTouchDevice,Input)

    MultiTouchAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	void	UpdateDevice() override;

	void	DoInputDeviceDescription() override;
    
protected:
	virtual ~MultiTouchAndroid();
   
	DECLARE_METHOD(ReinitCB);
	
	kstl::vector<jobject> myTouchList;
	jmethodID 	getEventCount;
	jmethodID 	getEvent;
	jmethodID 	clearEventList;
   
	jmethodID Event_getX;
	jmethodID Event_getY;
	jmethodID Event_getAction;
  
};    

#endif //_MULTITOUCHANDROID_H_
