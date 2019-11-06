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

	kstl::string joystickname("joystick");

	char	countstring[128];

	joystickname+=_itoa(gInstanceModuleInputDX->getJoystickCount()+1,countstring,10);

	JoystickDX* localJoystick=(JoystickDX*)gInstanceModuleInputDX->getCore()->GetInstanceOf(joystickname,"Joystick");

    HRESULT hr;

    // Obtain an interface to the enumerated joystick.
    hr = gInstanceModuleInputDX->getDirectInput()->CreateDevice( pdidInstance->guidInstance, &localJoystick->getDirectInputJoystick(), NULL );

    // If it failed, then we can't use this joystick. (Maybe the user unplugged
    // it while we were in the middle of enumerating it.)
    if( FAILED(hr) ) 
	{
		localJoystick->Destroy();
        return DIENUM_CONTINUE;
	}

	localJoystick->DoInputDeviceDescription();

	gInstanceModuleInputDX->addItem(localJoystick);
	localJoystick->Destroy();
  
    return DIENUM_CONTINUE;
}

ModuleInputDX::ModuleInputDX(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	myDirectInput=0;
	myJoystickCount=0;

	InitCommonControls();
	myIsInitOK=true;

}
    
ModuleInputDX::~ModuleInputDX()
{
    if(myDirectInput)
	{
		myDirectInput->Release();
	}
}    


void ModuleInputDX::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	if(myIsInitOK)
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
											 IID_IDirectInput8, (VOID**)&myDirectInput, NULL ) ) )
		{
			myIsInitOK=false;
			return;
		}


		// Look for joysticks we can use 
		if( FAILED( hr = gInstanceModuleInputDX->getDirectInput()->EnumDevices(	DI8DEVCLASS_GAMECTRL, 
																	EnumJoysticksCallback,
																	NULL, DIEDFL_ATTACHEDONLY ) ) )
		{
			myIsInitOK=false;
			return ;
		}

		CoreModifiable * conf = CoreModifiable::Import("InputConf.xml");

		if (!conf || conf->getAttribute("NoMouse")==NULL)
		{
			MouseDX* localmouse = (MouseDX*)core->GetInstanceOf("mouse", "MouseDevice");

			// Obtain an interface to the system mouse device.
			if (FAILED(hr = gInstanceModuleInputDX->getDirectInput()->CreateDevice(GUID_SysMouse, &localmouse->getDirectInputMouse(), NULL)))
			{
				localmouse->Destroy();
				localmouse = 0;
			}
			else if (FAILED(hr = localmouse->getDirectInputMouse()->SetDataFormat(&c_dfDIMouse2)))
			{
				localmouse->Destroy();
				localmouse = 0;
			}


			if (localmouse)
			{
				localmouse->DoInputDeviceDescription();
				addItem(localmouse);
				localmouse->Destroy();
			}
		}

		if (!conf || conf->getAttribute("NoKeyBoard") == NULL)
		{
			KeyboardDX* localkeyboard = (KeyboardDX*)core->GetInstanceOf("keyboard", "KeyboardDevice");

			// Obtain an interface to the system keyboard device.
			if (FAILED(hr = gInstanceModuleInputDX->getDirectInput()->CreateDevice(GUID_SysKeyboard, &localkeyboard->getDirectInputKeyboard(), NULL)))
			{
				localkeyboard->Destroy();
				localkeyboard = 0;
			}

			if (localkeyboard)
			{
				localkeyboard->DoInputDeviceDescription();
				addItem(localkeyboard);
				localkeyboard->Destroy();
			}
		}

		if (!conf || conf->getAttribute("NoGeolocation") == NULL)
		{
			GeolocationDevice* localGeolocation = (GeolocationDevice*)core->GetInstanceOf("geolocation", "GeolocationDevice");
			if (localGeolocation)
			{
				addItem(localGeolocation);
				localGeolocation->Destroy();
			}
		}

		// if no joystick, use virtual joystick

/*		if(gInstanceModuleInputDX->getJoystickCount() == 0)
		{
			kstl::string joystickname("joystick");

			char	countstring[128];

			joystickname+=_itoa(gInstanceModuleInputDX->getJoystickCount()+1,countstring,10);

			KeyBoardVirtualJoystickDX* localJoystick=(KeyBoardVirtualJoystickDX*)gInstanceModuleInputDX->getCore()->GetInstanceOf(joystickname,"KeyBoardVirtualJoystickDX");

			localJoystick->DoInputDeviceDescription();

			gInstanceModuleInputDX->addItem(localJoystick);
			localJoystick->Destroy();
		}*/

		if (conf)
		{
			conf->Destroy();
		}
	}
}       

void ModuleInputDX::Close()
{
    BaseClose();
}    

void ModuleInputDX::Update(const Timer& timer, void* addParam)
{
	// read info on aquired devices
	for(auto item : getItems())
    {
		if(item.myItem->isSubType(InputDevice::myClassID))
		{
			InputDevice* device=item.myItem->as<InputDevice>();
			
			if(!device->IsAquired())
				device->Aquire();
			if(device->IsAquired())
			{
				device->UpdateDevice();
			}
		}
	} 
}    

bool	ModuleInputDX::addItem(CoreModifiable *item, ItemPosition pos DECLARE_LINK_NAME)
{
	if(item->isSubType(JoystickDevice::myClassID))
	{
		myJoystickCount++;
		return CoreModifiable::addItem(item,pos PASS_LINK_NAME(linkName));
	}
	else if(item->isSubType(InputDevice::myClassID))
	{
		return CoreModifiable::addItem(item,pos PASS_LINK_NAME(linkName));
	}

	return false;
}

ModuleBase* MODULEINITFUNC(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
 
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(ModuleInputDX, "ModuleInputDX");
    gInstanceModuleInputDX=new ModuleInputDX("ModuleInputDX");
    gInstanceModuleInputDX->Init(core,params);
        
	return gInstanceModuleInputDX;
}    

