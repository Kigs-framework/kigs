#include "PrecompiledHeaders.h"

#include "VirtualDevice.h"

using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(VirtualDevice)

VirtualDevice::VirtualDevice(const std::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
}     

VirtualDevice::~VirtualDevice()
{

}    

void	VirtualDevice::UpdateDevice()
{
}
