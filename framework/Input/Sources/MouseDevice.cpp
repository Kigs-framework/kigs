#include "PrecompiledHeaders.h"
#include "DeviceItem.h"
#include "MouseDevice.h"

IMPLEMENT_CLASS_INFO(MouseDevice)

MouseDevice::MouseDevice(const std::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG),
mPosX(*this,false,"PosX",0.0f),
mPosY(*this,false,"PosY",0.0f)
{
	mWheelCount=mButtonsCount=0;
	setReadOnly("PosX");
	setReadOnly("PosY");
	mDX = 0.0f;
	mDY = 0.0f;
	mDZ = 0.0f;
	mPreviousDZ = 0.0f;

}
void MouseDevice::setButtonState(MOUSE_BUTTONS button, int val)
{
	mDeviceItems[2 + mWheelCount + button]->getState()->SetValue(val);
}

MouseDevice::~MouseDevice()
{

}    

void	MouseDevice::UpdateDevice()
{
	mDX=mDeviceItems[0]->getState()->GetTypedValue(float);
	mDY=mDeviceItems[1]->getState()->GetTypedValue(float);
	mPosX+=mDX;
	mPosY+=mDY;
}
