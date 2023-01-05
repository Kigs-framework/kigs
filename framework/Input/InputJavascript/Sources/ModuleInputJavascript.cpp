#include "ModuleInputJavascript.h"
#include "MouseJavascript.h"
#include "KeyboardJavascript.h"
#include "Core.h"
#include "DeviceItem.h"
#include "Timer.h"
#include "ModuleInput.h"
#include "InputDevice.h"
#include "JoystickDevice.h"


using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(ModuleInputJavascript)

ModuleInputJavascript::ModuleInputJavascript(const std::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	mJoystickCount=0;
	mIsInitOK=true;
	
}

ModuleInputJavascript::~ModuleInputJavascript()
{
	
}    


void ModuleInputJavascript::Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
	if(mIsInitOK)
	{
		BaseInit(core,"InputJavascript",params);
		DECLARE_FULL_CLASS_INFO(core,MouseJavascript,Mouse,Input)
		DECLARE_FULL_CLASS_INFO(core,KeyboardJavascript,Keyboard,Input)
	}
	
	
	SP<MouseJavascript> localmouse=core->GetInstanceOf("mouse","Mouse");
	
	
	if(localmouse)
	{
		localmouse->DoInputDeviceDescription();
		addItem(localmouse);
	}
	
	
	SP<KeyboardJavascript> localkeyboard=core->GetInstanceOf("keyboard","Keyboard");
	
	if(localkeyboard)
	{
		localkeyboard->DoInputDeviceDescription();
		addItem(localkeyboard);
	}
}       

void ModuleInputJavascript::Close()
{
    BaseClose();
}    

void ModuleInputJavascript::Update(const Time::Timer& /* timer */, void* addParam)
{
	// read info on aquired devices
	
	std::vector<ModifiableItemStruct>::const_iterator it;
	
    for (it=getItems().begin();it!=getItems().end();++it)
    {
		if((*it).mItem->isSubType(InputDevice::mClassID))
		{
			
			InputDevice* device=(InputDevice*)(*it).mItem.get();
			if(!device->IsAquired())
				device->Aquire();
			if(device->IsAquired())
			{
				device->UpdateDevice();
			}
		}
	}            
	
}    

bool	ModuleInputJavascript::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	
	if(item->isSubType(JoystickDevice::mClassID))
	{
		mJoystickCount++;
		return CoreModifiable::addItem(item);
	}
	else if(item->isSubType(InputDevice::mClassID))
	{
		return CoreModifiable::addItem(item);
	}
	
	return false;
	
}

SP<ModuleBase> Kigs::Input::PlatformInputModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(ModuleInputJavascript, "ModuleInputJavascript");
	auto ptr = MakeRefCounted<ModuleInputJavascript>("ModuleInputJavascript");
    ptr->Init(core,params);
    return ptr;
}    
