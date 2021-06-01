#ifndef _MOUSEANDROID_H_
#define _MOUSEANDROID_H_

#include "MouseDevice.h"
#include "ModuleInputAndroid.h"

#include "maReference.h"

#include <jni.h>

class MultiTouchDevice;

// ****************************************
// * MouseAndroid class
// * --------------------------------------
/**
* \file	MouseAndroid.h
* \class	MouseAndroid
* \ingroup Input
* \brief Android mouse management (touch).
*
*/
// ****************************************

class	MouseAndroid : public MouseDevice
{
public:
	DECLARE_CLASS_INFO(MouseAndroid, MouseDevice, Input)
	DECLARE_CONSTRUCTOR(MouseAndroid);

	bool	Aquire() override;
	bool	Release() override;

	void	UpdateDevice() override;

	void	DoInputDeviceDescription() override;

	void	IncWheelCount() { mWheelCount++; }
	void	IncButtonCount() { mButtonsCount++; }
	virtual ~MouseAndroid();

protected:

	DECLARE_METHOD(ReinitCB);

	jobject		mTouchList;
	jmethodID 	mGetEventCount;
	jmethodID 	mGetEvent;
	jmethodID 	mClearEventList;

	jmethodID mEventGetX;
	jmethodID mEventGetY;
	jmethodID mEventGetAction;

	maReference mMultiTouch = BASE_ATTRIBUTE(MultiTouch, "");
	MultiTouchDevice* mMultiTouchPointer = nullptr;

	int mFrameCountSinceLastValidTouched;
};

#endif //_MOUSEANDROID_H_
