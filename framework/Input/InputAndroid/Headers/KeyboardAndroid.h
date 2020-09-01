#ifndef _KeyboardANDROID_H_
#define _KeyboardANDROID_H_

#include "KeyboardDevice.h"
#include "ModuleInputAndroid.h"
#include "DeviceItem.h"
#include <jni.h>

// ****************************************
// * KeyboardAndroid class
// * --------------------------------------
/**
* \file	KeyboardAndroid.h
* \class	KeyboardAndroid
* \ingroup Input
* \brief Android keyboard management.
*
*/
// ****************************************

class KeyboardAndroid : public KeyboardDevice
{
public:
	DECLARE_CLASS_INFO(KeyboardAndroid, KeyboardDevice, Input)
	DECLARE_CONSTRUCTOR(KeyboardAndroid);

	void	UpdateDevice() override;

	const DeviceItemBaseState&	getKeyState(int key_id) override { return *mDeviceItems[key_id]->getState(); }
	void	DoInputDeviceDescription() override;

	void Show() override;
	void Hide() override;

	void PushEvent(void*, int);

protected:

	jclass mJKeyboard;
	jmethodID mJGetActions;
	jmethodID mJClear;

	kstl::vector<KeyEvent> mReceivedEvent;

	DECLARE_METHOD(ReinitCB);
};

#endif //_KeyboardANDROID_H_
