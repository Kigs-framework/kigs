#ifndef _MULTITOUCHDEVICE_H_
#define _MULTITOUCHDEVICE_H_

#include "InputDevice.h"

class MultiTouchDevice : public InputDevice
{
public:	
	DECLARE_ABSTRACT_CLASS_INFO(MultiTouchDevice, InputDevice, Input)
	DECLARE_INLINE_CONSTRUCTOR(MultiTouchDevice) {}

	int				getTouchState(int touchIndex);
	void			getTouchPos(int touchIndex, kfloat &posX, kfloat &posY);
	int getMaxTouch()
	{
		return myMaxTouch;
	}

	void UpdateDevice() override {};

protected:
	int		myMaxTouch = 0;
};

#endif //_MULTITOUCHDEVICE_H_