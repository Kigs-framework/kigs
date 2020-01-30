#include "PrecompiledHeaders.h"
#include "ModuleInputWUP.h"
#include "Core.h"
#include "DeviceItem.h"
#include "Timer.h"
#include "ModuleInput.h"
#include "CoreBaseApplication.h"


#include "MouseWUP.h"
#include "GazeWUP.h"
#include "KeyboardWUP.h"

#include <winrt/Windows.Devices.h>
#include <winrt/Windows.Devices.Input.h>
using namespace winrt::Windows::Devices;

ModuleInputWUP* gInstanceModuleInputWUP=0;


IMPLEMENT_CLASS_INFO(ModuleInputWUP)

ModuleInputWUP::ModuleInputWUP(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	myJoystickCount=0;
	
	myIsInitOK=true;
	
}

ModuleInputWUP::~ModuleInputWUP()
{
}    

void ModuleInputWUP::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	if (myIsInitOK)
	{
		DECLARE_FULL_CLASS_INFO(core, MouseWUP, MouseDevice, ModuleInput);
		DECLARE_FULL_CLASS_INFO(core, KeyboardWUP, KeyboardDevice, ModuleInput);
		DECLARE_FULL_CLASS_INFO(core, GazeDeviceWUP, GazeDevice, ModuleInput);

		BaseInit(core, "InputWUP", params);
		
		CMSP conf = CoreModifiable::Import("InputConf.xml");
		
		if (!conf || !conf->getAttribute("NoMouse"))
		{
			if (KigsCore::GetCoreApplication()->IsHolographic())
			{
				SP<GazeDeviceWUP> localGaze = core->GetInstanceOf("gaze", "GazeDevice");
				localGaze->DoInputDeviceDescription();
				addItem((CMSP&)localGaze);
			}
			else
			{
				auto pointerDevices = Input::PointerDevice::GetPointerDevices();
				if (pointerDevices.Size())
				{
					SP<MouseWUP> localmouse = core->GetInstanceOf("mouse", "MouseDevice");
					localmouse->DoInputDeviceDescription();
					addItem((CMSP&)localmouse);
				}
			}
		}
		
		if (!conf || !conf->getAttribute("NoKeyBoard"))
		{
			SP<KeyboardWUP> localkeyboard = core->GetInstanceOf("keyboard", "KeyboardDevice");

			// Obtain an interface to the system mouse device.
			auto capabilities = Input::KeyboardCapabilities();

			/*if (capabilities.KeyboardPresent() == 0)
			{
				localkeyboard->Destroy();
				localkeyboard = 0;
			}*/
			
			if (localkeyboard)
			{
				localkeyboard->DoInputDeviceDescription();
				addItem((CMSP&)localkeyboard);
				
			}
		}		
	}
}

void ModuleInputWUP::Close()
{
	BaseClose();
}    

void ModuleInputWUP::Update(const Timer& timer, void* addParam)
{
	// read info on aquired devices
	
	kstl::vector<ModifiableItemStruct>::const_iterator it;
	
	for (it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).myItem->isSubType(InputDevice::myClassID))
		{
			InputDevice* device=(InputDevice*)(*it).myItem.get();
			
			if(!device->IsAquired())
				device->Aquire();
			
			if(device->IsAquired())
				device->UpdateDevice();
		}
	}            
	
}    

bool	ModuleInputWUP::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if(item->isSubType(InputDevice::myClassID))
	{
		return CoreModifiable::addItem(item,pos PASS_LINK_NAME(linkName));
	}
	
	return false;
}

ModuleBase* PlatformInputModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
	
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(ModuleInputWUP,"ModuleInputWUP");
	gInstanceModuleInputWUP=new ModuleInputWUP("ModuleInputWUP");
	gInstanceModuleInputWUP->Init(core,params);
	
	return gInstanceModuleInputWUP;
}    

