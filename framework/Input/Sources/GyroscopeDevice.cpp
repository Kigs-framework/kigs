#include "PrecompiledHeaders.h"
#include "GyroscopeDevice.h"

IMPLEMENT_CLASS_INFO(GyroscopeDevice)

GyroscopeDevice::GyroscopeDevice(const kstl::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG)
, myRotationVelocity(*this, false, LABEL_AND_ID(RotationVelocity),0.0f, 0.0f, 0.0f)
, myQuaternion(*this, false, LABEL_AND_ID(RotationQuaternion),0.0f, 0.0f, 0.0f, 0.0f)
, myRate(*this, false, LABEL_AND_ID(Rate), 0)
{

}     

GyroscopeDevice::~GyroscopeDevice()
{
	
}    

void	GyroscopeDevice::UpdateDevice()
{
	
}