#include "PrecompiledHeaders.h"

#include "ModuleInput.h"
#include "InputDevice.h"
#include "Timer.h"
#include "Core.h"

#include "Window.h"
#include "TouchInputEventManager.h"

#ifdef WIN32
#define USE_VIRTUAL_SENSORS 1
#else
#define USE_VIRTUAL_SENSORS 0
#endif

#if USE_VIRTUAL_SENSORS
#include "VirtualSensors.h"
#endif

IMPLEMENT_CLASS_INFO(ModuleInput)

IMPLEMENT_CONSTRUCTOR(ModuleInput)
{
}


void ModuleInput::Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
	BaseInit(core, "Input", params);

	DECLARE_FULL_CLASS_INFO(core, TouchInputEventManager, TouchInputEventManager, Input);

	core->RegisterMainModuleList(this, InputModuleCoreIndex);

#ifdef _KIGS_ONLY_STATIC_LIB_
 	RegisterDynamic(PlatformInputModuleInit(core, params));
#endif

#if USE_VIRTUAL_SENSORS
	DECLARE_FULL_CLASS_INFO(core, VirtualGyroscope, GyroscopeDevice, ModuleInput);
	DECLARE_FULL_CLASS_INFO(core, VirtualAccelerometer, AccelerometerDevice, ModuleInput);

	CMSP cm = KigsCore::GetInstanceOf("virtual_gyroscope", "GyroscopeDevice");
	mVirtualSensors.push_back(cm);
	cm = KigsCore::GetInstanceOf("virtual_accelerometer", "AccelerometerDevice");
	mVirtualSensors.push_back(cm);
#endif
	// search for mouse, joystick and keyboard

	std::vector<CMSP>	instances;
	std::vector<CMSP>::iterator	it;
	instances = GetInstances("MouseDevice");

	it = instances.begin();
	if (it != instances.end())
	{
		mMouse = (MouseDevice*)(*it).get();
	}

	instances = GetInstances("SpatialInteractionDevice");
	it = instances.begin();
	if (it != instances.end())
	{
		mSpatialInteraction = (SpatialInteractionDevice*)(*it).get();
	}

	instances = GetInstances("MultiTouchDevice");
	it = instances.begin();
	if (it != instances.end())
	{
		mMultiTouch = (MultiTouchDevice*)(*it).get();
	}

	instances = GetInstances("AccelerometerDevice");
	it = instances.begin();
	if (it != instances.end())
	{
		mAccelerometer = (AccelerometerDevice*)(*it).get();
	}

	instances = GetInstances("GyroscopeDevice");
	it = instances.begin();
	if (it != instances.end())
	{
		mGyroscope = (GyroscopeDevice*)(*it).get();
	}

	instances = GetInstances("GeolocationDevice");
	it = instances.begin();
	if (it != instances.end())
	{
		mGeolocation = (GeolocationDevice*)(*it).get();
	}

	instances = GetInstances("CompassDevice");
	it = instances.begin();
	if (it != instances.end())
	{
		mCompass = (CompassDevice*)(*it).get();
	}
	
	instances = GetInstances("KeyboardDevice");
	it = instances.begin();
	if (it != instances.end())
	{
		mKeyboard = (KeyboardDevice*)(*it).get();
	}

	instances = GetInstances("JoystickDevice");

	if (instances.size())
	{
		mJoysticks.resize(instances.size());
	}

	int index = 0;
	for (it = instances.begin(); it != instances.end(); it++)
	{
		mJoysticks[index++] = (JoystickDevice*)(*it).get();
	}

	// create TouchInputEventManager

	mTouchManager = KigsCore::GetInstanceOf("toucheventmanager", "TouchInputEventManager");
	mTouchManager->Init();
}

void ModuleInput::Close()
{
#if USE_VIRTUAL_SENSORS
	mVirtualSensors.clear();
#endif
	BaseClose();
}

void ModuleInput::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer, addParam);

	std::list<WindowClick*>::iterator it;
	for (it = mActiveWindows.begin(); it != mActiveWindows.end(); ++it)
	{
		WindowClick* theWC = (WindowClick*)*it;
		theWC->update();
	}

	mTouchManager->CallUpdate(timer, addParam);
}

JoystickDevice*		ModuleInput::GetJoystick(int index)
{
	return mJoysticks[index];
}

void ModuleInput::WindowClickEvent(CoreModifiable *w, int buttonId, float X, float Y, bool isDown)
{
	auto theModuleInput = KigsCore::Instance()->GetModule<ModuleInput>();
	WindowClick * lClick = theModuleInput->getWindowClick(w);
	if (lClick)
		lClick->setPos(buttonId, X, Y);
	else
	{
		lClick = new WindowClick(w);
		lClick->setPos(buttonId, X, Y);
		theModuleInput->addWindowClick(lClick);
	}
}

void ModuleInput::WindowDestroyEvent(CoreModifiable *w)
{
	auto theModuleInput = KigsCore::Instance()->GetModule<ModuleInput>();
	WindowClick * lClick = theModuleInput->getWindowClick(w);
	if (lClick)
		theModuleInput->removeWindowClick(lClick);
}

bool ModuleInput::getActiveWindowPos(CoreModifiable *w, MouseDevice::MOUSE_BUTTONS buttonId, float &X, float &Y)
{
	WindowClick *lClick = getWindowClick(w);
	if (lClick)
		lClick->getPos(buttonId, X, Y);
	else
		return false;

	return true;
}

bool ModuleInput::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if (item->isSubType("Window"))
	{
		Window* lwindow = ((Window*)item.get());
		lwindow->SetClickCallback(ModuleInput::WindowClickEvent);
		lwindow->SetDestroyCallback(ModuleInput::WindowDestroyEvent);
	}

	return ModuleBase::addItem(item, pos PASS_LINK_NAME(linkName));
}

void ModuleInput::registerTouchEvent(CoreModifiable* item, const std::string& calledMethod, const std::string& eventName, unsigned int EmptyFlag)
{
	InputEventType	toregister;
	bool			eventFound = false;
	if (eventName == "Click")
	{
		toregister = Click;
		eventFound = true;
	}
	else if (eventName == "Swipe")
	{
		toregister = Swipe;
		eventFound = true;
	}
	else if (eventName == "Pinch")
	{
		toregister = Pinch;
		eventFound = true;
	}
	else if (eventName == "Scroll")
	{
		toregister = Scroll;
		eventFound = true;
	}
	else if (eventName == "DragAndDrop")
	{
		toregister = DragAndDrop;
		eventFound = true;
	}
	else if (eventName == "DirectTouch")
	{
		toregister = DirectTouch;
		eventFound = true;
	}
	else
	{
		// check if it's an int (event id)
		int result;
		int test = sscanf(eventName.c_str(), "%d", &result);
		if (test)
		{
			toregister = (InputEventType)result;
			eventFound = true;
		}
	}
		
	if(eventFound)
		getTouchManager()->registerEvent(item, calledMethod, toregister, (InputEventManagementFlag)EmptyFlag);
}
