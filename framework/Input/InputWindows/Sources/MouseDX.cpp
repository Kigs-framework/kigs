#include "PrecompiledHeaders.h"
#include "MouseDX.h"
#include "Core.h"
#include "DeviceItem.h"
#include "Window.h"


//IMPLEMENT_AND_REGISTER_CLASS_INFO(MouseDX, MouseDevice, Input);
IMPLEMENT_CLASS_INFO(MouseDX)

BOOL CALLBACK EnumMouseObjectsCallback( const DIDEVICEOBJECTINSTANCEA* instancea ,
                                     VOID* pContext )
{

	// enum mouse deviceItem

	MouseDX*	localmouse=(MouseDX*)pContext;

	if(instancea->guidType == GUID_ZAxis)
	{
		localmouse->IncWheelCount();
	}
	else if(instancea->guidType == GUID_Button)
	{
		localmouse->IncButtonCount();
	}

    return DIENUM_CONTINUE;
}





MouseDX::MouseDX(const kstl::string& name,CLASS_NAME_TREE_ARG) : MouseDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
    myDirectInputMouse=0;
}

MouseDX::~MouseDX()
{  
	if(myDirectInputMouse)
	{
		myDirectInputMouse->Release();
	}
}    

bool	MouseDX::Aquire()
{
	if (MouseDevice::Aquire())
	{
		myDirectInputMouse->SetCooperativeLevel((myInputWindow ? (HWND)myInputWindow->GetHandle() : NULL), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		myDirectInputMouse->Acquire();
		return true;
	}
	return false;
}

bool	MouseDX::Release()
{
	if (MouseDevice::Release())
	{
		myDirectInputMouse->Unacquire();
		return true;
	}
	return false;
}

void	MouseDX::UpdateDevice()
{
	HRESULT       hr;
  
    DIMOUSESTATE2 dims2;      // DirectInput mouse state structure

    if( NULL == myDirectInputMouse ) 
        return;
    
    // Get the input's device state, and put the state in dims
    ZeroMemory( &dims2, sizeof(dims2) );
    hr = myDirectInputMouse->GetDeviceState( sizeof(DIMOUSESTATE2), &dims2 );
    if( FAILED(hr) ) 
    {
        return; 
    }
    
	int currentDevice=0;

	// posX and posY
	myDeviceItems[currentDevice++]->getState()->SetValue((kfloat)dims2.lX);
	myDeviceItems[currentDevice++]->getState()->SetValue((kfloat)dims2.lY);
	
	// wheel
	if(myWheelCount)
	{
		myDeviceItems[currentDevice++]->getState()->SetValue((kfloat)dims2.lZ);
		if(myDZ != myPreviousDZ)
			myPreviousDZ = myDZ;
		myDZ += (kfloat)dims2.lZ;
	}

	// buttons
	int currentButton;
	for(currentButton=0;currentButton<myButtonsCount;currentButton++)
	{
		myDeviceItems[currentDevice++]->getState()->SetValue(dims2.rgbButtons[currentButton]& 0x80);
	}
	
	// call father update
	//MouseDevice::Update();
	
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	myDX = (kfloat)cursorPos.x - myPosX;
	myDY = (kfloat)cursorPos.y - myPosY;
	myPosX = (kfloat)cursorPos.x;
	myPosY = (kfloat)cursorPos.y;
}

void	MouseDX::DoInputDeviceDescription()
{
	myDirectInputMouse->EnumObjects(EnumMouseObjectsCallback,this,DIDFT_ALL);

	myDeviceItemsCount=myButtonsCount+myWheelCount+2; // +2 for posx and posy

	DeviceItem**	devicearray=new DeviceItem*[myDeviceItemsCount];
	
	unsigned int currentDevice=0;

	devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0f)));
	devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0f)));

	if(myWheelCount)
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<kfloat>(KFLOAT_CONST(0.0f)));

	int currentButton;
	for(currentButton=0;currentButton<myButtonsCount;currentButton++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<int>(0));
	}

	InitItems(myDeviceItemsCount,devicearray);

	for(currentDevice=0;currentDevice<myDeviceItemsCount;currentDevice++)
	{
		delete devicearray[currentDevice];
	}

	delete[] devicearray;

}
