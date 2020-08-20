#include "PrecompiledHeaders.h"
#include "DeviceItem.h"
#include "MouseDevice.h"

IMPLEMENT_CLASS_INFO(MouseDevice)

MouseDevice::MouseDevice(const kstl::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG),
mPosX(*this,false,LABEL_AND_ID(PosX),KFLOAT_CONST(0.0f)),
mPosY(*this,false,LABEL_AND_ID(PosY),KFLOAT_CONST(0.0f))
{
	mWheelCount=mButtonsCount=0;
//	myPosX=myPosY=KFLOAT_CONST(0.0f);
	setReadOnly("PosX");
	setReadOnly("PosY");
	mDX = 0.0f;
	mDY = 0.0f;
	mDZ = 0.0f;
	mPreviousDZ = 0.0f;
/*	setReadOnly("DX");
	setReadOnly("DY");
*/
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
