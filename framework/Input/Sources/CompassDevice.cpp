#include "PrecompiledHeaders.h"
#include "CompassDevice.h"

IMPLEMENT_CLASS_INFO(CompassDevice)

CompassDevice::CompassDevice(const std::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG)
, mCompX(*this,false,"CompX",0.0f)
, mCompY(*this,false,"CompY",0.0f)
, mCompZ(*this,false,"CompZ",0.0f)
, mRate(*this, false, "Rate", 0)
{

}     

CompassDevice::~CompassDevice()
{
	
}    

void	CompassDevice::UpdateDevice()
{
	
}