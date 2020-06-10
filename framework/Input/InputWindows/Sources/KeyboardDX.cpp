#include "PrecompiledHeaders.h"
#include "KeyboardDX.h"
#include "Core.h"
#include "Window.h"


//IMPLEMENT_AND_REGISTER_CLASS_INFO(KeyboardDX, KeyboardDevice, Input);
IMPLEMENT_CLASS_INFO(KeyboardDX)


KeyboardDX::KeyboardDX(const kstl::string& name,CLASS_NAME_TREE_ARG) : KeyboardDevice(name,PASS_CLASS_NAME_TREE_ARG)
, myDirectInputKeyboard(0)
{
	myLayout = GetKeyboardLayout(0);
}

KeyboardDX::~KeyboardDX()
{  
	if(myDirectInputKeyboard)
	{
		myDirectInputKeyboard->Release();
	}
}    

void	KeyboardDX::UpdateDevice()
{
	

	m_KeyUpList.clear();
	m_KeyDownList.clear();

	HRESULT	err;

	bool IsOk = true;
	if ((err = myDirectInputKeyboard->GetDeviceState( ((sizeof(unsigned char)) << 8), (void*)myKeys.state)) != DI_OK)
	{
		if ((err & DIERR_INPUTLOST) && (err & DIERR_NOTACQUIRED))
		{
			// try to (re-)acquire the keyboard interface
			Release();
			Aquire();
		}
		if ((err = myDirectInputKeyboard->GetDeviceState( ((sizeof(unsigned char)) << 8), (void*)myKeys.state)) != DI_OK)
		{
			IsOk = false;
		}

	}
	unsigned int currentKey;

	if (IsOk)
	{
		kstl::vector<KeyEvent>	touchVector;
		for(currentKey=0;currentKey<myDeviceItemsCount;currentKey++)
		{
			int PreviousValue = myDeviceItems[currentKey]->getState()->GetTypedValue(int);
			if( myKeys.state[currentKey] != PreviousValue)
			{
				KeyEvent ke;
				ke.Unicode = ScanToChar(currentKey, &ke.KeyCode);
				ke.flag = (iswprint(ke.Unicode))?1:0;

				if(PreviousValue > 0)
				{
					ke.Action = KeyEvent::ACTION_UP;
					m_KeyUpList.push_back(ke);
				}
				else
				{
					ke.Action = KeyEvent::ACTION_DOWN;
					m_KeyDownList.push_back(ke);
				}
				touchVector.push_back(ke);
			}
			myDeviceItems[currentKey]->getState()->SetValue(myKeys.state[currentKey]);
		}
		if(!touchVector.empty())
		{
			EmitSignal(Signals::KeyboardEvent, touchVector);
		}
	}
}


u16 KeyboardDX::ScanToChar(u32 scanCode, u32* vkCode)
{
	u32 tmpVk;
	if (vkCode == nullptr)
	{
		vkCode = &tmpVk;
	}

	//translate keyboard press scan code identifier to a virtual key
	unsigned short asciiValue = 0;
	*vkCode = 0;
	switch (scanCode)
	{
	case CM_KEY_NUMPAD0: asciiValue = u'0'; *vkCode = VK_NUMPAD0; break;
	case CM_KEY_NUMPAD1: asciiValue = u'1'; *vkCode = VK_NUMPAD1; break;
	case CM_KEY_NUMPAD2: asciiValue = u'2'; *vkCode = VK_NUMPAD2; break;
	case CM_KEY_NUMPAD3: asciiValue = u'3'; *vkCode = VK_NUMPAD3; break;
	case CM_KEY_NUMPAD4: asciiValue = u'4'; *vkCode = VK_NUMPAD4; break;
	case CM_KEY_NUMPAD5: asciiValue = u'5'; *vkCode = VK_NUMPAD5; break;
	case CM_KEY_NUMPAD6: asciiValue = u'6'; *vkCode = VK_NUMPAD6; break;
	case CM_KEY_NUMPAD7: asciiValue = u'7'; *vkCode = VK_NUMPAD7; break;
	case CM_KEY_NUMPAD8: asciiValue = u'8'; *vkCode = VK_NUMPAD8; break;
	case CM_KEY_NUMPAD9: asciiValue = u'9'; *vkCode = VK_NUMPAD9; break;
	case CM_KEY_DIVIDE: asciiValue = u'/'; *vkCode = VK_DIVIDE; break;
	case CM_KEY_MULTIPLY: asciiValue = u'*'; *vkCode = VK_MULTIPLY; break;
	case CM_KEY_DECIMAL: asciiValue = u'.'; *vkCode = VK_DECIMAL; break;
	case CM_KEY_SUBTRACT: asciiValue = u'-'; *vkCode = VK_SUBTRACT; break;
	case CM_KEY_ADD: asciiValue = u'+'; *vkCode = VK_ADD; break;
	case CM_KEY_UP:  *vkCode = VK_UP; break;
	case CM_KEY_DOWN: *vkCode = VK_DOWN; break;
	case CM_KEY_LEFT:  *vkCode = VK_LEFT; break;
	case CM_KEY_RIGHT:  *vkCode = VK_RIGHT; break;



	default:break;
	}

	if((*vkCode) == 0)
	{
		*vkCode = MapVirtualKeyEx(scanCode, MAPVK_VSC_TO_VK_EX, myLayout);
		int nb = ToAsciiEx(*vkCode, scanCode, myKeys.state, &asciiValue, 0, myLayout);
	}

	return asciiValue;
}

bool	KeyboardDX::Aquire()
{
	if (KeyboardDevice::Aquire())
	{
		HRESULT h = myDirectInputKeyboard->SetCooperativeLevel((myInputWindow ? (HWND)myInputWindow->GetHandle() : NULL), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		myDirectInputKeyboard->Acquire();
		return true;
	}
	return false;
}

bool	KeyboardDX::Release()
{
	if (KeyboardDevice::Release())
	{
		myDirectInputKeyboard->Unacquire();
		return true;
	}
	return false;
}


void	KeyboardDX::DoInputDeviceDescription()
{
	myDeviceItemsCount=256;

	DeviceItem**	devicearray=new DeviceItem*[myDeviceItemsCount];

	unsigned int currentDevice=0;

	unsigned int index;
	for(index=0;index<myDeviceItemsCount;index++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<int>(0));
	}

	InitItems(myDeviceItemsCount,devicearray);
	for(index=0;index<myDeviceItemsCount;index++)
	{
		delete devicearray[index];
	}

	delete[] devicearray;

	//myDirectInputKeyboard->SetCooperativeLevel(myHwnd, DISCL_FOREGROUND |DISCL_NONEXCLUSIVE);

	// set format to our key states
	myDirectInputKeyboard->SetDataFormat(&c_dfDIKeyboard);
}
