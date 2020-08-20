#include "PrecompiledHeaders.h"
#include "GyroscopeDevice.h"

IMPLEMENT_CLASS_INFO(GyroscopeDevice)

GyroscopeDevice::GyroscopeDevice(const kstl::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG)
, mRotationVelocity(*this, false, LABEL_AND_ID(RotationVelocity),0.0f, 0.0f, 0.0f)
, mRotationQuaternion(*this, false, LABEL_AND_ID(RotationQuaternion),0.0f, 0.0f, 0.0f, 0.0f)
, mRate(*this, false, LABEL_AND_ID(Rate), 0)
{

}     

GyroscopeDevice::~GyroscopeDevice()
{
	
}    

void	GyroscopeDevice::UpdateDevice()
{
	
}