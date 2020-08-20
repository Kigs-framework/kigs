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


void ModuleInput::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
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
	cm->GetRef();
	cm = KigsCore::GetInstanceOf("virtual_accelerometer", "AccelerometerDevice");
	cm->GetRef();
#endif
	// search for mouse, joystick and keyboard

	kstl::vector<CMSP>	instances;
	kstl::vector<CMSP>::iterator	it;
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
	kstl::vector<CMSP> L_instances=	CoreModifiable::GetInstancesByName("GyroscopeDevice", "virtual_gyroscope");
	auto itr = L_instances.begin();
	auto end = L_instances.end();
	for (; itr != end; ++itr)
		(*itr)->Destroy();
	L_instances = CoreModifiable::GetInstancesByName("AccelerometerDevice", "virtual_accelerometer");
	itr = L_instances.begin();
	end = L_instances.end();
	for(;itr!=end;++itr)
		(*itr)->Destroy();
#endif


	BaseClose();
}

void ModuleInput::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer, addParam);

	kstl::list<WindowClick*>::iterator it;
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

void ModuleInput::WindowClickEvent(CoreModifiable *w, int buttonId, kfloat X, kfloat Y, bool isDown)
{
	ModuleInput* theModuleInput = (ModuleInput*)KigsCore::Instance()->GetModule("ModuleInput");

	WindowClick *myClick = theModuleInput->getWindowClick(w);
	if (myClick)
		myClick->setPos(buttonId, X, Y);
	else
	{
		myClick = new WindowClick(w);
		myClick->setPos(buttonId, X, Y);
		theModuleInput->addWindowClick(myClick);
	}
}

void ModuleInput::WindowDestroyEvent(CoreModifiable *w)
{
	ModuleInput* theModuleInput = (ModuleInput*)KigsCore::Instance()->GetModule("ModuleInput");
	WindowClick *myClick = theModuleInput->getWindowClick(w);
	if (myClick)
		theModuleInput->removeWindowClick(myClick);
}

bool ModuleInput::getActiveWindowPos(CoreModifiable *w, MouseDevice::MOUSE_BUTTONS buttonId, kfloat &X, kfloat &Y)
{
	WindowClick *myClick = getWindowClick(w);
	if (myClick)
		myClick->getPos(buttonId, X, Y);
	else
		return false;

	return true;
}

bool ModuleInput::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if (item->isSubType("Window"))
	{
		Window* myWindow = ((Window*)item.get());
		myWindow->SetClickCallback(ModuleInput::WindowClickEvent);
		myWindow->SetDestroyCallback(ModuleInput::WindowDestroyEvent);
	}

	return ModuleBase::addItem(item, pos PASS_LINK_NAME(linkName));
}

void ModuleInput::registerTouchEvent(CoreModifiable* item, const kstl::string& calledMethod, const kstl::string& eventName, unsigned int EmptyFlag)
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
