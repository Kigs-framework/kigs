#include "PrecompiledHeaders.h"
#include "JoystickDX.h"
#include "DeviceItem.h"
#include "Core.h"
#include "Window.h"


//IMPLEMENT_AND_REGISTER_CLASS_INFO(JoystickDX, JoystickDevice, Input);
IMPLEMENT_CLASS_INFO(JoystickDX)

BOOL CALLBACK EnumJoystickObjectsCallback( const DIDEVICEOBJECTINSTANCEA* instancea ,
                                     VOID* pContext )
{

	// enum joystick deviceItem

	JoystickDX*	localjoystick=(JoystickDX*)pContext;

	if(instancea->guidType == GUID_Button)
	{
		localjoystick->IncButtonCount();
	}
	else if((instancea->guidType == GUID_XAxis) || (instancea->guidType == GUID_YAxis) || (instancea->guidType == GUID_ZAxis))
	{
		localjoystick->UseAxis();
	}
	else if((instancea->guidType == GUID_RxAxis) || (instancea->guidType == GUID_RyAxis) || (instancea->guidType == GUID_RzAxis))
	{
		localjoystick->UseRotation();
	}
	else if(instancea->guidType == GUID_POV )
	{
		localjoystick->IncPOVCount();
	}
	else
	{
		printf("%s\n",instancea->tszName);
	}
    
    return DIENUM_CONTINUE;
}


JoystickDX::JoystickDX(const kstl::string& name,CLASS_NAME_TREE_ARG) : JoystickDevice(name,PASS_CLASS_NAME_TREE_ARG)
, mDirectInputJoystick(0)
, mAxisIndex(-1)
, mRotationIndex(-1)
{

}

JoystickDX::~JoystickDX()
{  

	if(mDirectInputJoystick)
	{
		mDirectInputJoystick->Release();
	}

}    

bool	JoystickDX::Aquire()
{
	if (JoystickDevice::Aquire())
	{
		mDirectInputJoystick->SetCooperativeLevel((mInputWindow ? (HWND)mInputWindow->GetHandle() : NULL), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		mDirectInputJoystick->Acquire();
		return true;
	}
	return false;
}

bool	JoystickDX::Release()
{
	if (JoystickDevice::Release())
	{
		mDirectInputJoystick->Unacquire();
		return true;
	}
	return false;
}

void	JoystickDX::UpdateDevice()
{
	HRESULT     hr;
    DIJOYSTATE2 dijs2;      // DirectInput joystick state structure

    if( NULL == mDirectInputJoystick ) 
        return;

    // Get the input's device state, and put the state in dims
    ZeroMemory( &dijs2, sizeof(dijs2) );
	
    hr = mDirectInputJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &dijs2 );
    if( FAILED(hr) ) 
    {
        return; 
    }

	unsigned int currentDevice=0;

	unsigned int currentButton;
	for(currentButton=0;currentButton<mButtonsCount;currentButton++)
	{
		mDeviceItems[currentDevice++]->getState()->SetValue(dijs2.rgbButtons[currentButton]& 0x80);
	}

	// axis
	if(mAxisIndex!=-1)
	{
		Point3D p((kfloat)(dijs2.lX-32768)/32768.0f,(kfloat)(dijs2.lY-32768)/32768.0f,(kfloat)(dijs2.lZ-32768)/32768.0f);
		mDeviceItems[mAxisIndex]->getState()->SetValue(p);
	}
	if(mRotationIndex!=-1)
	{
		Point3D p((kfloat)(dijs2.lRx-32768)/32768.0f,(kfloat)(dijs2.lRy-32768)/32768.0f,(kfloat)(dijs2.lRz-32768)/32768.0f);
		mDeviceItems[mRotationIndex]->getState()->SetValue(p);
	}
	currentDevice+=mAxisCount;
		
	for(currentButton=0;currentButton<mPovCount;currentButton++)
	{
		mDeviceItems[currentDevice++]->getState()->SetValue((int)dijs2.rgdwPOV[currentButton]);
	}
}

void	JoystickDX::DoInputDeviceDescription()
{
	mDirectInputJoystick->EnumObjects(EnumJoystickObjectsCallback,this,DIDFT_ALL);

	if(mAxisIndex!=-1)     mAxisCount++;
	if(mRotationIndex!=-1) mAxisCount++;

	mDeviceItemsCount=mButtonsCount+mPovCount+mAxisCount;

	DeviceItem**	devicearray=new DeviceItem*[mDeviceItemsCount];
	
	unsigned int currentDevice=0;

	unsigned int currentButton;
	for(currentButton=0;currentButton<mButtonsCount;currentButton++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<int>(0));
	}

	if(mAxisIndex)
	{
		mAxisIndex=currentDevice++;
		devicearray[mAxisIndex]=new DeviceItem(DeviceItemState<Point3D>(Point3D()));
	}
	if(mRotationIndex)
	{
		mRotationIndex=currentDevice++;
		devicearray[mRotationIndex]=new DeviceItem(DeviceItemState<Point3D>(Point3D()));
	}

	unsigned int currentPOV;
	for(currentPOV=0;currentPOV<mPovCount;currentPOV++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<int>(0));
	}

	InitItems(mDeviceItemsCount,devicearray);

	for(currentButton=0;currentButton<mDeviceItemsCount;currentButton++)
	{
		delete devicearray[currentButton];
	}

	delete[] devicearray;

	mDirectInputJoystick->SetDataFormat(&c_dfDIJoystick2);

}
