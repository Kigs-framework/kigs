#include "PrecompiledHeaders.h"
#include "AccelerometerDevice.h"

using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(AccelerometerDevice)

AccelerometerDevice::AccelerometerDevice(const std::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG)
, mAccX(*this,"AccX",0.0f)
, mAccY(*this,"AccY",0.0f)
, mAccZ(*this,"AccZ",0.0f)
, mRawAccX(*this, "RawAccX", 0.0f)
, mRawAccY(*this, "RawAccY", 0.0f)
, mRawAccZ(*this, "RawAccZ", 0.0f)
, mVelX(*this,"VelX",0.0f)
, mVelY(*this,"VelY",0.0f)
, mVelZ(*this,"VelZ",0.0f)
, mPosX(*this,"PosX",0.0f)
, mPosY(*this,"PosY",0.0f)
, mPosZ(*this,"PosZ",0.0f)
, mRate(*this, "Rate", 0)
{
}     

AccelerometerDevice::~AccelerometerDevice()
{
	
}    

void	AccelerometerDevice::UpdateDevice()
{
	
}