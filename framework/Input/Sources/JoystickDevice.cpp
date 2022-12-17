#include "PrecompiledHeaders.h"
#include "JoystickDevice.h"
#include "DeviceItem.h"

IMPLEMENT_CLASS_INFO(JoystickDevice)

JoystickDevice::JoystickDevice(const std::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
   mPovCount=mButtonsCount=mAxisCount=0;

}     

JoystickDevice::~JoystickDevice()
{

}    
float JoystickDevice::getXAxisState(int index)
{
	return mDeviceItems[mButtonsCount + index]->getState()->GetTypedValue(Point3D).x;
}

float JoystickDevice::getYAxisState(int index)
{
	return mDeviceItems[mButtonsCount + index]->getState()->GetTypedValue(Point3D).y;
}

float JoystickDevice::getZAxisState(int index)
{
	return mDeviceItems[mButtonsCount + index]->getState()->GetTypedValue(Point3D).z;
}
