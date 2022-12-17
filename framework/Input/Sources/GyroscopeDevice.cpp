#include "PrecompiledHeaders.h"
#include "GyroscopeDevice.h"

IMPLEMENT_CLASS_INFO(GyroscopeDevice)

GyroscopeDevice::GyroscopeDevice(const std::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG)
, mRotationVelocity(*this, false, "RotationVelocity",0.0f, 0.0f, 0.0f)
, mRotationQuaternion(*this, false, "RotationQuaternion",0.0f, 0.0f, 0.0f, 0.0f)
, mRate(*this, false, "Rate", 0)
{

}     

GyroscopeDevice::~GyroscopeDevice()
{
	
}    

void	GyroscopeDevice::UpdateDevice()
{
	
}