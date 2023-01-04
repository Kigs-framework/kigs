#include "PrecompiledHeaders.h"
#include "ModuleInputDX.h"
#include "KeyBoardDX.h"
#include "MouseDX.h"
#include "JoystickDX.h"
#include "Core.h"
#include "DeviceItem.h"
#include "Timer.h"
#include "KeyToAsciiDX.h"
#include "ModuleInput.h"
#include "GeolocationDX.h"

using namespace Kigs::Input;

ModuleInputDX* gInstanceModuleInputDX=0;

IMPLEMENT_CLASS_INFO(ModuleInputDX);

//-----------------------------------------------------------------------------
// Name: EnumJoysticksCallback()
// Desc: Called once for each enumerated joystick. If we find one, create a
//       device interface on it so we can play with it.
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
                                     VOID* pContext )
{

	// create a joystickDX

	std::string joystickname("joystick");

	char	countstring[128];

	joystickname+=_itoa(gInstanceModuleInputDX->getJoystickCount()+1,countstring,10);

	SP<JoystickDX> localJoystick = gInstanceModuleInputDX->getCore()->GetInstanceOf(joystickname,"JoystickDevice");

    HRESULT hr;

    // Obtain an interface to the enumerated joystick.
    hr = gInstanceModuleInputDX->getDirectInput()->CreateDevice( pdidInstance->guidInstance, &localJoystick->getDirectInputJoystick(), NULL );

    // If it failed, then we can't use this joystick. (Maybe the user unplugged
    // it while we were in the middle of enumerating it.)
    if( FAILED(hr) ) 
	{
        return DIENUM_CONTINUE;
	}

	localJoystick->DoInputDeviceDescription();

	gInstanceModuleInputDX->addItem(localJoystick);
  
    return DIENUM_CONTINUE;
}

ModuleInputDX::ModuleInputDX(const std::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	mDirectInput=0;
	mJoystickCount=0;

	InitCommonControls();
	mIsInitOK=true;

}
    
ModuleInputDX::~ModuleInputDX()
{
    if(mDirectInput)
	{
		mDirectInput->Release();
	}
}    


void ModuleInputDX::Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
	if(mIsInitOK)
	{
		BaseInit(core,"InputDX",params);
		
		DECLARE_FULL_CLASS_INFO(core, GeolocationDX, GeolocationDevice, Input);
		DECLARE_FULL_CLASS_INFO(core, JoystickDX, JoystickDevice, Input);
		DECLARE_FULL_CLASS_INFO(core, KeyboardDX, KeyboardDevice, Input);
		DECLARE_FULL_CLASS_INFO(core, KeyToAsciiDX, KeyToAscii, Input);
		DECLARE_FULL_CLASS_INFO(core, MouseDX, MouseDevice, Input);

		HRESULT hr;

		// Register with the DirectInput subsystem and get a pointer
		// to a IDirectInput interface we can use.
		// Create a DInput object
		if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
											 IID_IDirectInput8, (VOID**)&mDirectInput, NULL ) ) )
		{
			mIsInitOK=false;
			return;
		}


		// Look for joysticks we can use 
		if( FAILED( hr = gInstanceModuleInputDX->getDirectInput()->EnumDevices(	DI8DEVCLASS_GAMECTRL, 
																	EnumJoysticksCallback,
																	NULL, DIEDFL_ATTACHEDONLY ) ) )
		{
			mIsInitOK=false;
			return ;
		}

		CMSP conf = CoreModifiable::Import("InputConf.xml");

		if (!conf || conf->getAttribute("NoMouse")==NULL)
		{
			SP<MouseDX> localmouse = core->GetInstanceOf("mouse", "MouseDevice");

			// Obtain an interface to the system mouse device.
			if (FAILED(hr = gInstanceModuleInputDX->getDirectInput()->CreateDevice(GUID_SysMouse, &localmouse->getDirectInputMouse(), NULL)))
			{
				localmouse = nullptr;
			}
			else if (FAILED(hr = localmouse->getDirectInputMouse()->SetDataFormat(&c_dfDIMouse2)))
			{
				localmouse = nullptr;
			}


			if (localmouse)
			{
				localmouse->DoInputDeviceDescription();
				addItem(localmouse);
			}
		}

		if (!conf || conf->getAttribute("NoKeyBoard") == NULL)
		{
			SP<KeyboardDX> localkeyboard = core->GetInstanceOf("keyboard", "KeyboardDevice");

			// Obtain an interface to the system keyboard device.
			if (FAILED(hr = gInstanceModuleInputDX->getDirectInput()->CreateDevice(GUID_SysKeyboard, &localkeyboard->getDirectInputKeyboard(), NULL)))
			{
				localkeyboard = nullptr;
			}

			if (localkeyboard)
			{
				localkeyboard->DoInputDeviceDescription();
				addItem(localkeyboard);
			}
		}

		if (!conf || conf->getAttribute("NoGeolocation") == NULL)
		{
			SP<GeolocationDevice> localGeolocation = core->GetInstanceOf("geolocation", "GeolocationDevice");
			if (localGeolocation)
			{
				addItem(localGeolocation);
			}
		}

		// if no joystick, use virtual joystick

/*		if(gInstanceModuleInputDX->getJoystickCount() == 0)
		{
			std::string joystickname("joystick");

			char	countstring[128];

			joystickname+=_itoa(gInstanceModuleInputDX->getJoystickCount()+1,countstring,10);

			KeyBoardVirtualJoystickDX* localJoystick=(KeyBoardVirtualJoystickDX*)gInstanceModuleInputDX->getCore()->GetInstanceOf(joystickname,"KeyBoardVirtualJoystickDX");

			localJoystick->DoInputDeviceDescription();

			gInstanceModuleInputDX->addItem(localJoystick);
			localJoystick->Destroy();
		}*/

	}
}       

void ModuleInputDX::Close()
{
    BaseClose();
}    

void ModuleInputDX::Update(const Time::Timer& timer, void* addParam)
{
	// read info on aquired devices
	for(auto item : getItems())
    {
		if(item.mItem->isSubType(InputDevice::mClassID))
		{
			InputDevice* device=item.mItem->as<InputDevice>();
			
			if(!device->IsAquired())
				device->Aquire();
			if(device->IsAquired())
			{
				device->UpdateDevice();
			}
		}
	} 
}    

bool ModuleInputDX::addItem(const CMSP& item, ItemPosition pos)
{
	if(item->isSubType(JoystickDevice::mClassID))
	{
		mJoystickCount++;
		return CoreModifiable::addItem(item,pos PASS_LINK_NAME(linkName));
	}
	else if(item->isSubType(InputDevice::mClassID))
	{
		return CoreModifiable::addItem(item,pos PASS_LINK_NAME(linkName));
	}

	return false;
}

SP<ModuleBase> Kigs::Input::PlatformInputModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(ModuleInputDX, "ModuleInputDX");
	auto ptr = MakeRefCounted<ModuleInputDX>("ModuleInputDX");
	gInstanceModuleInputDX = ptr.get();
    gInstanceModuleInputDX->Init(core,params);
	return ptr;
}    

