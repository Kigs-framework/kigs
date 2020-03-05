#include "ModuleInputAndroid.h"
#include "MouseAndroid.h"
#include "KeyboardAndroid.h"
#include "MultiTouchAndroid.h"
#include "JoystickAndroid.h"
#include "AccelerometerAndroid.h"
#include "Core.h"
#include "DeviceItem.h"
#include "Timer.h"
#include "ModuleInput.h"
#include "GeolocationAndroid.h"
#include "GyroscopeAndroid.h"
#include "CompassAndroid.h"

ModuleInputAndroid* gInstanceModuleInputAndroid = 0;



IMPLEMENT_CLASS_INFO(ModuleInputAndroid)
IMPLEMENT_CONSTRUCTOR(ModuleInputAndroid)
{
	myJoystickCount = 0;
	myIsInitOK = true;
}

void ModuleInputAndroid::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	if (myIsInitOK)
	{
		BaseInit(core, "InputAndroid", params);
		DECLARE_FULL_CLASS_INFO(core, JoystickAndroid, Joystick, Input)
		DECLARE_FULL_CLASS_INFO(core, MouseAndroid, Mouse, Input)
		DECLARE_FULL_CLASS_INFO(core, KeyboardAndroid, Keyboard, Input)
		DECLARE_FULL_CLASS_INFO(core, AccelerometerAndroid, Accelerometer, Input)
		DECLARE_FULL_CLASS_INFO(core, MultiTouchAndroid, MultiTouch, Input)
		DECLARE_FULL_CLASS_INFO(core, GeolocationAndroid, Geolocation, Input)
		DECLARE_FULL_CLASS_INFO(core, GyroscopeAndroid, Gyroscope, Input)
		DECLARE_FULL_CLASS_INFO(core, CompassAndroid, Compass, Input)
	}

	CMSP conf = CoreModifiable::Import("InputConf.xml");
	SP<MultiTouchDevice> multitouch(nullptr);
	if (!conf || conf->getAttribute("NoMultiTouch") == nullptr)
	{
		multitouch = core->GetInstanceOf("multitouch", "MultiTouch");
		if (multitouch)
		{
			multitouch->DoInputDeviceDescription();
			addItem((CMSP&)multitouch);
		}
	}

	if (!conf || conf->getAttribute("NoMouse") == nullptr)
	{
		SP<MouseAndroid> localmouse = core->GetInstanceOf("mouse", "Mouse");
		if (localmouse)
		{
			if(multitouch)
				localmouse->setValue("MultiTouch", CheckUniqueObject{ multitouch.get() });

			localmouse->DoInputDeviceDescription();
			addItem((CMSP&)localmouse);
		}
	}

	if (!conf || conf->getAttribute("NoKeyboard") == nullptr)
	{
		SP<KeyboardAndroid> localkeyboard = core->GetInstanceOf("keyboard", "Keyboard");
		if (localkeyboard)
		{
			localkeyboard->DoInputDeviceDescription();
			addItem((CMSP&)localkeyboard);
		}
	}


	if (!conf || conf->getAttribute("NoJoystick") == nullptr)
	{
		SP<JoystickAndroid> localjoystick = core->GetInstanceOf("joystick", "Joystick");
		if (localjoystick)
		{
			localjoystick->DoInputDeviceDescription();
			addItem((CMSP&)localjoystick);
		}
	}

	if (!conf || conf->getAttribute("NoAccelerometre") == nullptr)
	{
		SP<AccelerometerAndroid> localaccelerometer = core->GetInstanceOf("accelerometer", "Accelerometer");
		if (localaccelerometer)
		{
			localaccelerometer->DoInputDeviceDescription();
			if (localaccelerometer->IsInit())
			{
				addItem((CMSP&)localaccelerometer);
			}
		}
	}


	if (!conf || conf->getAttribute("NoGyroscope") == nullptr)
	{
		SP<GyroscopeAndroid> localgyroscope = core->GetInstanceOf("gyroscope", "Gyroscope");
		if (localgyroscope)
		{
			localgyroscope->DoInputDeviceDescription();
			if (localgyroscope->IsInit())
			{
				addItem((CMSP&)localgyroscope);
			}
		}
	}


	if (!conf || conf->getAttribute("NoCompass") == nullptr)
	{
		SP<CompassAndroid> localCompass = core->GetInstanceOf("Compass", "Compass");
		if (localCompass)
		{
			localCompass->DoInputDeviceDescription();
			if (localCompass->IsInit())
			{
				addItem((CMSP&)localCompass);
			}
		}
	}


	if (!conf || conf->getAttribute("NoGeolocation") == nullptr)
	{
		SP<GeolocationAndroid> localGeolocation = core->GetInstanceOf("geolocation", "Geolocation");
		if (localGeolocation)
		{
			localGeolocation->DoInputDeviceDescription();
			addItem((CMSP&)localGeolocation);
		}
	}
}

void ModuleInputAndroid::Close()
{
	BaseClose();
}

void ModuleInputAndroid::Update(const Timer& /* timer */, void* /*addParam*/)
{
	// read info on aquired devices

	kstl::vector<ModifiableItemStruct>::const_iterator it;

	for (it = getItems().begin(); it != getItems().end(); ++it)
	{
		if ((*it).myItem->isSubType(InputDevice::myClassID))
		{
			InputDevice* device = (InputDevice*)(*it).myItem.get();

			if (!device->IsAquired())
				device->Aquire();
			if (device->IsAquired())
			{
				device->UpdateDevice();
			}
		}
	}

}

bool	ModuleInputAndroid::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{

	if (item->isSubType(JoystickDevice::myClassID))
	{
		myJoystickCount++;
		return CoreModifiable::addItem(item);
	}
	else if (item->isSubType(InputDevice::myClassID))
	{
		return CoreModifiable::addItem(item);
	}

	return false;

}

ModuleBase* MODULEINITFUNC(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);

	DECLARE_CLASS_INFO_WITHOUT_FACTORY(ModuleInputAndroid, "ModuleInputAndroid");
	ModuleBase* gInstanceModuleInputAndroid = new ModuleInputAndroid("ModuleInputAndroid");
	gInstanceModuleInputAndroid->Init(core, params);
	return  gInstanceModuleInputAndroid;
}
