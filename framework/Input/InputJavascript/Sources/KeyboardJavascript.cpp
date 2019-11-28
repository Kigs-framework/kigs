#include "PrecompiledHeaders.h"
#include "KeyboardJavascript.h"
#include "Core.h"
#include "Window.h"

#include <emscripten.h>
#include <iostream>
#include <wctype.h>


IMPLEMENT_CLASS_INFO(KeyboardJavascript)


extern "C" void		AddListenerToKeyBoard();
extern "C" int		Get_Key(int index);
extern "C" int		Get_KeyBoardEvent(int index);
extern "C" int		Get_KeyBoardEventCount();
extern "C" void		ClearKeyBoardList();

KeyboardJavascript::KeyboardJavascript(const kstl::string& name,CLASS_NAME_TREE_ARG) :
KeyboardDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
	AddListenerToKeyBoard();
	RecordingString_Enable=true;
}

KeyboardJavascript::~KeyboardJavascript()
{  
}    

void	KeyboardJavascript::Set_RecordingString(bool _value)
{
	RecordingString_Enable = _value; 
	if(_value)
		MyString = "";
}

void	KeyboardJavascript::UpdateDevice()
{
	m_KeyUpList.clear();
	m_KeyDownList.clear();
	
	int eventCount = Get_KeyBoardEventCount();
	
	kstl::vector<KeyEvent>	touchVector;
	unsigned int currentKey;
	for(int i =0; i < eventCount; i++)
	{
		currentKey = Get_Key(i);
		int PreviousValue = myDeviceItems[currentKey]->getState()->GetTypedValue(int);
		int EventType = Get_KeyBoardEvent(i);
	
		
		KeyEvent ke;
		ke.KeyCode = currentKey;
		ke.Unicode = currentKey; // TODO
		ke.flag = (iswprint(ke.Unicode)) ? 1 : 0;
		
		if(EventType == 1) //Key Down
		{
			if(myDeviceItems[currentKey]->getState()->GetTypedValue(int) == 0)
			{
				m_KeyDownList.push_back(ke);
			}
			myDeviceItems[currentKey]->getState()->SetValue(1);
		}
		else if(EventType == 0) //Key up
		{
			
			if(myDeviceItems[currentKey]->getState()->GetTypedValue(int) == 1)
				m_KeyUpList.push_back(ke);
			
			myDeviceItems[currentKey]->getState()->SetValue(0);
		}
		
		touchVector.push_back(ke);
	}
	
	//Clear la liste
	ClearKeyBoardList();
	if(!touchVector.empty())
	{
		EmitSignal(Signals::KeyboardEvent, touchVector);
	}
}


void	KeyboardJavascript::DoInputDeviceDescription()
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
	
}
