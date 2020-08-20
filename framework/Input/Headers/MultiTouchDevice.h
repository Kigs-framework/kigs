#ifndef _MULTITOUCHDEVICE_H_
#define _MULTITOUCHDEVICE_H_

#include "InputDevice.h"
// ****************************************
// * MultiTouchDevice class
// * --------------------------------------
/**
 * \class	MultiTouchDevice
 * \file	MultiTouchDevice.h
 * \ingroup Input
 * \brief	Base class to manage generic multiple touch device.
 */
 // ****************************************
class MultiTouchDevice : public InputDevice
{
public:	
	DECLARE_ABSTRACT_CLASS_INFO(MultiTouchDevice, InputDevice, Input)
	DECLARE_INLINE_CONSTRUCTOR(MultiTouchDevice) {}

	int				getTouchState(int touchIndex);
	void			getTouchPos(int touchIndex, kfloat &posX, kfloat &posY);
	int getMaxTouch()
	{
		return mMaxTouch;
	}

	void UpdateDevice() override {};

protected:
	int		mMaxTouch = 0;
};

#endif //_MULTITOUCHDEVICE_H_