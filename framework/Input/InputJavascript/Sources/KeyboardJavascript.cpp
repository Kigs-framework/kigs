#include "PrecompiledHeaders.h"
#include "KeyboardJavascript.h"
#include "Core.h"
#include "Window.h"

#include <emscripten.h>
#include <iostream>
#include <wctype.h>


IMPLEMENT_CLASS_INFO(KeyboardJavascript)

// start keyboard listening
extern "C" void		AddListenerToKeyBoard();
// get virtual key code
extern "C" int		Get_Key(int index);
// get event type ( down / up )
extern "C" int		Get_KeyBoardEvent(int index);
// get unicode
extern "C" int		Get_KeyUnicode(int index);
// get event count
extern "C" int		Get_KeyBoardEventCount();
// clear event list
extern "C" void		ClearKeyBoardList();

KeyboardJavascript::KeyboardJavascript(const kstl::string& name,CLASS_NAME_TREE_ARG) :
KeyboardDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
	AddListenerToKeyBoard();
	mRecordingStringEnable=true;
}

KeyboardJavascript::~KeyboardJavascript()
{  
}    

void	KeyboardJavascript::Set_RecordingString(bool _value)
{
	mRecordingStringEnable = _value; 
	if(_value)
		mString = "";
}

void	KeyboardJavascript::UpdateDevice()
{
	mKeyUpList.clear();
	mKeyDownList.clear();
	
	int eventCount = Get_KeyBoardEventCount();
	
	kstl::vector<KeyEvent>	touchVector;
	unsigned int currentKey;
	for(int i =0; i < eventCount; i++)
	{
		currentKey = Get_Key(i);
		
		if(currentKey>0)		
		{
			KeyEvent ke;	
			ke.KeyCode = currentKey;
			unsigned int EventType = Get_KeyBoardEvent(i);
			ke.Unicode = Get_KeyUnicode(i);
		
			switch (ke.KeyCode)
			{
			case VK_NUMPAD0: ke.Unicode = u'0'; break;
			case VK_NUMPAD1: ke.Unicode = u'1'; break;
			case VK_NUMPAD2: ke.Unicode = u'2'; break;
			case VK_NUMPAD3: ke.Unicode = u'3'; break;
			case VK_NUMPAD4: ke.Unicode = u'4'; break;
			case VK_NUMPAD5: ke.Unicode = u'5'; break;
			case VK_NUMPAD6: ke.Unicode = u'6'; break;
			case VK_NUMPAD7: ke.Unicode = u'7'; break;
			case VK_NUMPAD8: ke.Unicode = u'8'; break;
			case VK_NUMPAD9: ke.Unicode = u'9'; break;
			case VK_DIVIDE: ke.Unicode = u'/'; break;
			case VK_MULTIPLY: ke.Unicode = u'*';  break;
			case VK_DECIMAL: ke.Unicode = u'.';  break;
			case VK_SUBTRACT: ke.Unicode = u'-';  break;
			case VK_ADD: ke.Unicode = u'+';  break;
			
			default:
			break;
			}
				
			ke.flag = (iswprint(ke.Unicode)) ? 1 : 0;
			
			if(EventType == 1) //Key Down
			{
				if(mDeviceItems[currentKey]->getState()->GetTypedValue(int) == 0)
				{
					ke.Action = KeyEvent::ACTION_DOWN;
					mKeyDownList.push_back(ke);
				}
				mDeviceItems[currentKey]->getState()->SetValue(1);
			}
			else if(EventType == 0) //Key up
			{
				if(mDeviceItems[currentKey]->getState()->GetTypedValue(int) == 1)
				{
					ke.Action = KeyEvent::ACTION_UP;
					mKeyUpList.push_back(ke);
				}
				mDeviceItems[currentKey]->getState()->SetValue(0);
			}
			
			touchVector.push_back(ke);
		}
	}
	//Clear the list
	ClearKeyBoardList();
	if(!touchVector.empty())
	{
		EmitSignal(Signals::KeyboardEvent, touchVector);
	}
}


void	KeyboardJavascript::DoInputDeviceDescription()
{
	mDeviceItemsCount=256;
	
	DeviceItem**	devicearray=new DeviceItem*[mDeviceItemsCount];
	
	unsigned int currentDevice=0;
	
	unsigned int index;
	for(index=0;index<mDeviceItemsCount;index++)
	{
		devicearray[currentDevice++]=new DeviceItem(DeviceItemState<int>(0));
	}
	
	InitItems(mDeviceItemsCount,devicearray);
	for(index=0;index<mDeviceItemsCount;index++)
	{
		delete devicearray[index];
	}
	
	delete[] devicearray;
	
}
