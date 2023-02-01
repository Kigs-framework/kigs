#include "PrecompiledHeaders.h"
#include "CompassDevice.h"

using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(CompassDevice)

CompassDevice::CompassDevice(const std::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG)
, mCompX(*this,"CompX",0.0f)
, mCompY(*this,"CompY",0.0f)
, mCompZ(*this,"CompZ",0.0f)
, mRate(*this, "Rate", 0)
{

}     

CompassDevice::~CompassDevice()
{
	
}    

void	CompassDevice::UpdateDevice()
{
	
}