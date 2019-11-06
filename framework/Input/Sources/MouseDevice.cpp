#include "PrecompiledHeaders.h"
#include "DeviceItem.h"
#include "MouseDevice.h"

IMPLEMENT_CLASS_INFO(MouseDevice)

MouseDevice::MouseDevice(const kstl::string& name,CLASS_NAME_TREE_ARG) : InputDevice(name,PASS_CLASS_NAME_TREE_ARG),
myPosX(*this,false,LABEL_AND_ID(PosX),KFLOAT_CONST(0.0f)),
myPosY(*this,false,LABEL_AND_ID(PosY),KFLOAT_CONST(0.0f))
{
	myWheelCount=myButtonsCount=0;
//	myPosX=myPosY=KFLOAT_CONST(0.0f);
	setReadOnly("PosX");
	setReadOnly("PosY");
	myDX = 0.0f;
	myDY = 0.0f;
	myDZ = 0.0f;
	myPreviousDZ = 0.0f;
/*	setReadOnly("DX");
	setReadOnly("DY");
*/
}
void MouseDevice::setButtonState(MOUSE_BUTTONS button, int val)
{
	myDeviceItems[2 + myWheelCount + button]->getState()->SetValue(val);
}

MouseDevice::~MouseDevice()
{

}    

void	MouseDevice::UpdateDevice()
{
	myDX=myDeviceItems[0]->getState()->GetTypedValue(float);
	myDY=myDeviceItems[1]->getState()->GetTypedValue(float);
	myPosX+=myDX;
	myPosY+=myDY;
}
