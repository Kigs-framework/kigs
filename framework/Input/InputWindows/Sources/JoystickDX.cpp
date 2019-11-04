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
, myDirectInputJoystick(0)
, myAxisIndex(-1)
, myRotationIndex(-1)
{

}

JoystickDX::~JoystickDX()
{  

	if(myDirectInputJoystick)
	{
		myDirectInputJoystick->Release();
	}

}    

bool	JoystickDX::Aquire()
{
	if (JoystickDevice::Aquire())
	{
		myDirectInputJoystick->SetCooperativeLevel((myInputWindow ? (HWND)myInputWindow->GetHandle() : NULL), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		myDirectInputJoystick->Acquire();
		return true;
	}
	return false;
}

bool	JoystickDX::Release()
{
	if (JoystickDevice::Release())
	{
		myDirectInputJoystick->Unacquire();
		return true;
	}
	return false;
}

void	JoystickDX::UpdateDevice()
{
	HRESULT     hr;
    DIJOYSTATE2 dijs2;      // DirectInput joystick state structure

    if( NULL == myDirectInputJoystick ) 
        return;

    // Get the input's device state, and put the state in dims
    ZeroMemory( &dijs2, sizeof(dijs2) );
	
    hr = myDirectInputJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &dijs2 );
    if( FAILED(hr) ) 
    {
        return; 
    }

	unsigned int currentDevice=0;

	unsigned int currentButton;
	for(currentButton=0;currentButton<myButtonsCount;currentButton++)
	{
		myDeviceItems[currentDevice++]->getState()->SetValue(dijs2.rgbButtons[currentButton]& 0x80);
	}

	// axis
	if(myAxisIndex!=-1)
	{
		Point3D p((kfloat)(dijs2.lX-32768)/32768.0f,(kfloat)(dijs2.lY-32768)/32768.0f,(kfloat)(dijs2.lZ-32768)/32768.0f);
		myDeviceItems[myAxisIndex]->getState()->SetValue(p);
	}
	if(myRotationIndex!=-1)
	{
		Point3D p((kfloat)(dijs2.lRx-32768)/32768.0f,(kfloat)(dijs2.lRy-32768)/32768.0f,(kfloat)(dijs2.lRz-32768)/32768.0f);
		myDeviceItems[myRotationIndex]->getState()->SetValue(p);
	}
	currentDevice+=myAxisCount;
		
	for(currentButton=0;currentButton<myPovCount;currentButton++)
	{
		myDeviceItems[currentDevice++]->getState()->SetValue((int)dijs2.rgdwPOV[currentButton]);
	}
}

void	JoystickDX::DoInputDeviceDescription()
{
	myDirectInputJoystick->EnumObjects(EnumJoystickObjectsCallback,this,DIDFT_ALL);

	if(myAxisIndex!=-1)     myAxisCount++;
	if(myRotationIndex!=-1) myAxisCount++;

	myDeviceItemsCount=myButtonsCount+myPovCount+myAxisCount;

	DeviceItem**	devicearray=new DeviceItem*[myDeviceItemsCount];
	
	unsigned int currentDevice=0;

	unsigned int currentButton;
	for(currentButton=0;currentButton<myButtonsCount;currentButton++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<int>(0));
	}

	if(myAxisIndex)
	{
		myAxisIndex=currentDevice++;
		devicearray[myAxisIndex]=new DeviceItem(DeviceItemState<Point3D>(Point3D()));
	}
	if(myRotationIndex)
	{
		myRotationIndex=currentDevice++;
		devicearray[myRotationIndex]=new DeviceItem(DeviceItemState<Point3D>(Point3D()));
	}

	unsigned int currentPOV;
	for(currentPOV=0;currentPOV<myPovCount;currentPOV++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<int>(0));
	}

	InitItems(myDeviceItemsCount,devicearray);

	for(currentButton=0;currentButton<myDeviceItemsCount;currentButton++)
	{
		delete devicearray[currentButton];
	}

	delete[] devicearray;

	myDirectInputJoystick->SetDataFormat(&c_dfDIJoystick2);

}
