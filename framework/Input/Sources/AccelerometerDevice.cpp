#include "PrecompiledHeaders.h"
#include "AccelerometerDevice.h"

IMPLEMENT_CLASS_INFO(AccelerometerDevice)

AccelerometerDevice::AccelerometerDevice(const kstl::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG)
, myAccX(*this,false,LABEL_AND_ID(AccX),KFLOAT_CONST(0.0f))
, myAccY(*this,false,LABEL_AND_ID(AccY),KFLOAT_CONST(0.0f))
, myAccZ(*this,false,LABEL_AND_ID(AccZ),KFLOAT_CONST(0.0f))
, myRawAccX(*this, false, LABEL_AND_ID(RawAccX), KFLOAT_CONST(0.0f))
, myRawAccY(*this, false, LABEL_AND_ID(RawAccY), KFLOAT_CONST(0.0f))
, myRawAccZ(*this, false, LABEL_AND_ID(RawAccZ), KFLOAT_CONST(0.0f))
, myVelX(*this,false,LABEL_AND_ID(VelX),KFLOAT_CONST(0.0f))
, myVelY(*this,false,LABEL_AND_ID(VelY),KFLOAT_CONST(0.0f))
, myVelZ(*this,false,LABEL_AND_ID(VelZ),KFLOAT_CONST(0.0f))
, myPosX(*this,false,LABEL_AND_ID(PosX),KFLOAT_CONST(0.0f))
, myPosY(*this,false,LABEL_AND_ID(PosY),KFLOAT_CONST(0.0f))
, myPosZ(*this,false,LABEL_AND_ID(PosZ),KFLOAT_CONST(0.0f))
, myRate(*this, false, LABEL_AND_ID(Rate), 0)
{
}     

AccelerometerDevice::~AccelerometerDevice()
{
	
}    

void	AccelerometerDevice::UpdateDevice()
{
	
}