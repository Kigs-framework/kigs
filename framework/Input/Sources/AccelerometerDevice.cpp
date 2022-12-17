#include "PrecompiledHeaders.h"
#include "AccelerometerDevice.h"

IMPLEMENT_CLASS_INFO(AccelerometerDevice)

AccelerometerDevice::AccelerometerDevice(const std::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG)
, mAccX(*this,false,"AccX",0.0f)
, mAccY(*this,false,"AccY",0.0f)
, mAccZ(*this,false,"AccZ",0.0f)
, mRawAccX(*this, false, "RawAccX", 0.0f)
, mRawAccY(*this, false, "RawAccY", 0.0f)
, mRawAccZ(*this, false, "RawAccZ", 0.0f)
, mVelX(*this,false,"VelX",0.0f)
, mVelY(*this,false,"VelY",0.0f)
, mVelZ(*this,false,"VelZ",0.0f)
, mPosX(*this,false,"PosX",0.0f)
, mPosY(*this,false,"PosY",0.0f)
, mPosZ(*this,false,"PosZ",0.0f)
, mRate(*this, false, "Rate", 0)
{
}     

AccelerometerDevice::~AccelerometerDevice()
{
	
}    

void	AccelerometerDevice::UpdateDevice()
{
	
}