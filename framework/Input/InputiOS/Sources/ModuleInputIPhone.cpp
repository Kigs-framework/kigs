#include "ModuleInputIPhone.h"
#include "MouseIPhone.h"
#include "MultiTouchIPhone.h"
#include "AccelerometerIPhone.h"
#include "JoystickIPhone.h"
#include "Core.h"
#include "DeviceItem.h"
#include "Timer.h"
#include "ModuleInput.h"
#include "GeolocationIPhone.h"


#include <pthread.h>

IMPLEMENT_CLASS_INFO(ModuleInputIPhone)

ModuleInputIPhone::ModuleInputIPhone(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	
	myJoystickCount=0;
	myIsInitOK=true;
	
}

ModuleInputIPhone::~ModuleInputIPhone()
{
	
}    


void ModuleInputIPhone::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	if(myIsInitOK)
	{
		BaseInit(core,"InputIPhone",params);
		DECLARE_FULL_CLASS_INFO(core,JoystickIPhone,Joystick,Input)
			DECLARE_FULL_CLASS_INFO(core,MouseIPhone,Mouse,Input)
			DECLARE_FULL_CLASS_INFO(core,AccelerometerIPhone,Accelerometer,Input)
			DECLARE_FULL_CLASS_INFO(core,MultiTouchIPhone,MultiTouch,Input)
			DECLARE_FULL_CLASS_INFO(core,GeolocationIPhone,Geolocation,Input)
	}
	
	
	MouseIPhone* localmouse=(MouseIPhone*)core->GetInstanceOf("mouse",_S_2_ID("Mouse"));
	
	
	if(localmouse)
	{
		localmouse->DoInputDeviceDescription();
		addItem(localmouse);
		localmouse->Destroy();
	}
	
	MultiTouchIPhone* localMultiTouch=(MultiTouchIPhone*)core->GetInstanceOf("multitouch",_S_2_ID("MultiTouch"));
	
	if(localMultiTouch)
	{
		localMultiTouch->DoInputDeviceDescription();
		addItem(localMultiTouch);
		localMultiTouch->Destroy();
	}	
	
	JoystickIPhone* localjoystick=(JoystickIPhone*)core->GetInstanceOf("joystick",_S_2_ID("Joystick"));
	
	if(localjoystick)
	{
		localjoystick->DoInputDeviceDescription();
		addItem(localjoystick);
		localjoystick->Destroy();
	}	
	
	AccelerometerIPhone* localaccelerometer=(AccelerometerIPhone*)core->GetInstanceOf("accelerometer",_S_2_ID("Accelerometer"));
	
	if(localaccelerometer)
	{
		localaccelerometer->DoInputDeviceDescription();
		addItem(localaccelerometer);
		localaccelerometer->Destroy();
	}
    
    GeolocationIPhone* localGeolocation=(GeolocationIPhone*)core->GetInstanceOf("geolocation",_S_2_ID("Geolocation"));
    
    if(localGeolocation)
    {
        localGeolocation->DoInputDeviceDescription();
        addItem(localGeolocation);
        localGeolocation->Destroy();
    }
}       

void ModuleInputIPhone::Close()
{
    BaseClose();
}     

void ModuleInputIPhone::Update(const Timer& /* timer */, void* addParam)
{
	//pthread_mutex_lock( &eventMutex );
	// read info on aquired devices
	
	kstl::vector<ModifiableItemStruct>::const_iterator it = getItems().begin();
    kstl::vector<ModifiableItemStruct>::const_iterator ite = getItems().end();
    while(it!= ite)
    {
		if((*it).myItem->isSubType(InputDevice::myClassID))
		{
			InputDevice* device=(InputDevice*)(*it).myItem;
			
			if(!device->IsAquired())
				device->Aquire();
			if(device->IsAquired())
			{
				device->UpdateDevice();
			}
		}
        it++;
	} 
	//pthread_mutex_unlock( &eventMutex );
	
}    

bool	ModuleInputIPhone::addItem(CoreModifiable *item, ItemPosition pos DECLARE_LINK_NAME)
{
	
	if(item->isSubType(JoystickDevice::myClassID))
	{
		myJoystickCount++;
		return CoreModifiable::addItem(item);
	}
	else if(item->isSubType(InputDevice::myClassID))
	{
		return CoreModifiable::addItem(item);
	}
	
	return false;
	
}

ModuleBase* MODULEINITFUNC(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
	
	
	ModuleBase* gInstanceModuleInputIPhone=new ModuleInputIPhone("ModuleInputIPhone");
    gInstanceModuleInputIPhone->Init(core,params);
	
	return gInstanceModuleInputIPhone;
}    
