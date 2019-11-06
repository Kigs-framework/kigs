#include "PrecompiledHeaders.h"

#include "VirtualDevice.h"

IMPLEMENT_CLASS_INFO(VirtualDevice)

VirtualDevice::VirtualDevice(const kstl::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
}     

VirtualDevice::~VirtualDevice()
{

}    

void	VirtualDevice::UpdateDevice()
{
}
