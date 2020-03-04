#ifndef _MOUSEANDROID_H_
#define _MOUSEANDROID_H_

#include "MouseDevice.h"
#include "ModuleInputAndroid.h"

#include "maReference.h"

#include <jni.h>

class MultiTouchDevice;

class	MouseAndroid : public MouseDevice
{
public:
	DECLARE_CLASS_INFO(MouseAndroid, MouseDevice, Input)
	DECLARE_CONSTRUCTOR(MouseAndroid);

	bool	Aquire() override;
	bool	Release() override;

	void	UpdateDevice() override;

	void	DoInputDeviceDescription() override;

	void	IncWheelCount() { myWheelCount++; }
	void	IncButtonCount() { myButtonsCount++; }

protected:
	virtual ~MouseAndroid();

	DECLARE_METHOD(ReinitCB);

	jobject		myTouchList;
	jmethodID 	getEventCount;
	jmethodID 	getEvent;
	jmethodID 	clearEventList;

	jmethodID Event_getX;
	jmethodID Event_getY;
	jmethodID Event_getAction;

	maReference mMultiTouchRef = BASE_ATTRIBUTE(MultiTouch, "");
	MultiTouchDevice* mMultiTouch = nullptr;

	int m_FrameCountSinceLastValidTouched;
};

#endif //_MOUSEANDROID_H_
