#include "PrecompiledHeaders.h"
#include "AccelerometerDevice.h"

IMPLEMENT_CLASS_INFO(AccelerometerDevice)

AccelerometerDevice::AccelerometerDevice(const kstl::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG)
, mAccX(*this,false,LABEL_AND_ID(AccX),KFLOAT_CONST(0.0f))
, mAccY(*this,false,LABEL_AND_ID(AccY),KFLOAT_CONST(0.0f))
, mAccZ(*this,false,LABEL_AND_ID(AccZ),KFLOAT_CONST(0.0f))
, mRawAccX(*this, false, LABEL_AND_ID(RawAccX), KFLOAT_CONST(0.0f))
, mRawAccY(*this, false, LABEL_AND_ID(RawAccY), KFLOAT_CONST(0.0f))
, mRawAccZ(*this, false, LABEL_AND_ID(RawAccZ), KFLOAT_CONST(0.0f))
, mVelX(*this,false,LABEL_AND_ID(VelX),KFLOAT_CONST(0.0f))
, mVelY(*this,false,LABEL_AND_ID(VelY),KFLOAT_CONST(0.0f))
, mVelZ(*this,false,LABEL_AND_ID(VelZ),KFLOAT_CONST(0.0f))
, mPosX(*this,false,LABEL_AND_ID(PosX),KFLOAT_CONST(0.0f))
, mPosY(*this,false,LABEL_AND_ID(PosY),KFLOAT_CONST(0.0f))
, mPosZ(*this,false,LABEL_AND_ID(PosZ),KFLOAT_CONST(0.0f))
, mRate(*this, false, LABEL_AND_ID(Rate), 0)
{
}     

AccelerometerDevice::~AccelerometerDevice()
{
	
}    

void	AccelerometerDevice::UpdateDevice()
{
	
}