#include "PrecompiledHeaders.h"
#include "JoystickDevice.h"
#include "DeviceItem.h"

IMPLEMENT_CLASS_INFO(JoystickDevice)

JoystickDevice::JoystickDevice(const kstl::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
   myPovCount=myButtonsCount=myAxisCount=0;

}     

JoystickDevice::~JoystickDevice()
{

}    
kfloat JoystickDevice::getXAxisState(int index)
{
	return myDeviceItems[myButtonsCount + index]->getState()->GetTypedValue(Point3D).x;
}

kfloat JoystickDevice::getYAxisState(int index)
{
	return myDeviceItems[myButtonsCount + index]->getState()->GetTypedValue(Point3D).y;
}

kfloat JoystickDevice::getZAxisState(int index)
{
	return myDeviceItems[myButtonsCount + index]->getState()->GetTypedValue(Point3D).z;
}
