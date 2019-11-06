#include "PrecompiledHeaders.h"
#include "CompassDevice.h"

IMPLEMENT_CLASS_INFO(CompassDevice)

CompassDevice::CompassDevice(const kstl::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG)
, myCompX(*this,false,LABEL_AND_ID(CompX),KFLOAT_CONST(0.0f))
, myCompY(*this,false,LABEL_AND_ID(CompY),KFLOAT_CONST(0.0f))
, myCompZ(*this,false,LABEL_AND_ID(CompZ),KFLOAT_CONST(0.0f))
, myRate(*this, false, LABEL_AND_ID(Rate), 0)
{

}     

CompassDevice::~CompassDevice()
{
	
}    

void	CompassDevice::UpdateDevice()
{
	
}