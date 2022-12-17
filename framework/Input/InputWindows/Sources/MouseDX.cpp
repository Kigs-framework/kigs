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





MouseDX::MouseDX(const std::string& name,CLASS_NAME_TREE_ARG) : MouseDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
    mDirectInputMouse=0;
}

MouseDX::~MouseDX()
{  
	if(mDirectInputMouse)
	{
		mDirectInputMouse->Release();
	}
}    

bool	MouseDX::Aquire()
{
	if (MouseDevice::Aquire())
	{
		mDirectInputMouse->SetCooperativeLevel((mInputWindow ? (HWND)mInputWindow->GetHandle() : NULL), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		mDirectInputMouse->Acquire();
		return true;
	}
	return false;
}

bool	MouseDX::Release()
{
	if (MouseDevice::Release())
	{
		mDirectInputMouse->Unacquire();
		return true;
	}
	return false;
}

void	MouseDX::UpdateDevice()
{
	HRESULT       hr;
  
    DIMOUSESTATE2 dims2;      // DirectInput mouse state structure

    if( NULL == mDirectInputMouse ) 
        return;
    
    // Get the input's device state, and put the state in dims
    ZeroMemory( &dims2, sizeof(dims2) );
    hr = mDirectInputMouse->GetDeviceState( sizeof(DIMOUSESTATE2), &dims2 );
    if( FAILED(hr) ) 
    {
        return; 
    }
    
	int currentDevice=0;

	// posX and posY
	mDeviceItems[currentDevice++]->getState()->SetValue((float)dims2.lX);
	mDeviceItems[currentDevice++]->getState()->SetValue((float)dims2.lY);
	
	// wheel
	if(mWheelCount)
	{
		mDeviceItems[currentDevice++]->getState()->SetValue((float)dims2.lZ);
		if(mDZ != mPreviousDZ)
			mPreviousDZ = mDZ;
		mDZ += (float)dims2.lZ;
	}

	// buttons
	int currentButton;
	for(currentButton=0;currentButton<mButtonsCount;currentButton++)
	{
		mDeviceItems[currentDevice++]->getState()->SetValue(dims2.rgbButtons[currentButton]& 0x80);
	}
	
	// call father update
	//MouseDevice::Update();
	
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	mDX = (float)cursorPos.x - mPosX;
	mDY = (float)cursorPos.y - mPosY;
	mPosX = (float)cursorPos.x;
	mPosY = (float)cursorPos.y;
}

void	MouseDX::DoInputDeviceDescription()
{
	mDirectInputMouse->EnumObjects(EnumMouseObjectsCallback,this,DIDFT_ALL);

	mDeviceItemsCount=mButtonsCount+mWheelCount+2; // +2 for posx and posy

	DeviceItem**	devicearray=new DeviceItem*[mDeviceItemsCount];
	
	unsigned int currentDevice=0;

	devicearray[currentDevice++]=new DeviceItem(DeviceItemState<float>(0.0f));
	devicearray[currentDevice++]=new DeviceItem(DeviceItemState<float>(0.0f));

	if(mWheelCount)
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<float>(0.0f));

	int currentButton;
	for(currentButton=0;currentButton<mButtonsCount;currentButton++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<int>(0));
	}

	InitItems(mDeviceItemsCount,devicearray);

	for(currentDevice=0;currentDevice<mDeviceItemsCount;currentDevice++)
	{
		delete devicearray[currentDevice];
	}

	delete[] devicearray;

}
