#ifndef _KeyboardANDROID_H_
#define _KeyboardANDROID_H_

#include "KeyboardDevice.h"
#include "ModuleInputAndroid.h"
#include "DeviceItem.h"

// ****************************************
// * KeyboardAndroid class
// * --------------------------------------
/*!  \class KeyboardAndroid
	 Android Keyboard management (touch screen)
	 \ingroup InputAndroid
*/
// ****************************************

#include <jni.h>

class KeyboardAndroid : public KeyboardDevice
{
public:
	DECLARE_CLASS_INFO(KeyboardAndroid, KeyboardDevice, Input)
	DECLARE_CONSTRUCTOR(KeyboardAndroid);

	void	UpdateDevice() override;

	const DeviceItemBaseState&	getKeyState(int key_id) override { return *myDeviceItems[key_id]->getState(); }
	void	DoInputDeviceDescription() override;

	void Show() override;
	void Hide() override;

	void PushEvent(void*, int);

protected:

	jclass myJKeyboard;
	jmethodID JGetActions;
	jmethodID JClear;

	kstl::vector<KeyEvent> mReceivedEvent;

	DECLARE_METHOD(ReinitCB);
};

#endif //_KeyboardANDROID_H_
