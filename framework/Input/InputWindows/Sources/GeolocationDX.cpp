/*
 *  GeolocationDX.cpp
 *
 */

#include "GeolocationDX.h"
#include "Core.h"
#include "DeviceItem.h"


using namespace Kigs::Input;

//IMPLEMENT_AND_REGISTER_CLASS_INFO(GeolocationDX, GeolocationDevice, Input);
IMPLEMENT_CLASS_INFO(GeolocationDX)

GeolocationDX::GeolocationDX(const std::string& name,CLASS_NAME_TREE_ARG) : GeolocationDevice(name,PASS_CLASS_NAME_TREE_ARG)
{	
}

GeolocationDX::~GeolocationDX()
{
}

 
void	GeolocationDX::UpdateDevice()
{
}

void	GeolocationDX::DoInputDeviceDescription()
{
	GeolocationDevice::InitModifiable();
}
