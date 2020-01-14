#include "KeyboardWUP.h"
#include "Core.h"

#include "Platform/Main/BaseApp.h"

#include "utf8.h"

#include "winrt/Windows.UI.Text.Core.h"
#include "winrt/Windows.UI.ViewManagement.h"

using namespace winrt::Windows::UI::Text::Core;
using namespace winrt::Windows::UI::ViewManagement;

IMPLEMENT_CLASS_INFO(KeyboardWUP);

KeyboardWUP::KeyboardWUP(const kstl::string& name,CLASS_NAME_TREE_ARG) : KeyboardDevice(name,PASS_CLASS_NAME_TREE_ARG)
{
	auto window = App::GetApp()->GetWindow();
	window.CharacterReceived([this](winrt::Windows::UI::Core::CoreWindow const& window, winrt::Windows::UI::Core::CharacterReceivedEventArgs args)
	{
		std::lock_guard lk{ mMutex };
		u32 u32_value = args.KeyCode();
		std::string utf8_str;
		utf8::utf32to8(&u32_value, &u32_value + 1, std::back_inserter(utf8_str));
		std::u16string utf16_str;
		utf8::utf8to16(utf8_str.begin(), utf8_str.end(), std::back_inserter(utf16_str));
		mTyped += utf16_str;
	});
	
	window.KeyUp([this](winrt::Windows::UI::Core::CoreWindow const& window, winrt::Windows::UI::Core::KeyEventArgs args)
	{
		std::lock_guard lk{ mMutex };
		mEvents.push_back(std::make_pair(KeyEvent::ACTION_UP, args));
	});

	window.KeyDown([this](winrt::Windows::UI::Core::CoreWindow const& window, winrt::Windows::UI::Core::KeyEventArgs args)
	{
		std::lock_guard lk{ mMutex };
		mEvents.push_back(std::make_pair(KeyEvent::ACTION_DOWN, args));
	});
}

KeyboardWUP::~KeyboardWUP()
{  
}    

void	KeyboardWUP::UpdateDevice()
{
	m_KeyUpList.clear();
	m_KeyDownList.clear();
	
	kstl::vector<KeyEvent>	touchVector;
	
	// get down event
	{
		std::lock_guard lk{ mMutex };
		for (auto& p : mEvents)
		{
			KeyEvent ke;
			ke.Action = p.first;
			ke.KeyCode = (int)p.second.VirtualKey();
			ke.Unicode = ScanToChar(p.second.KeyStatus().ScanCode);
			ke.flag = (iswprint(ke.Unicode)) ? 1 : 0;
			touchVector.push_back(ke);
				
			int currentState = myDeviceItems[ke.KeyCode]->getState()->GetTypedValue(int);
				
			// down action
			if (ke.Action == KeyEvent::ACTION_DOWN)
			{
				if (currentState == 1)
					continue;
			
				myDeviceItems[ke.KeyCode]->getState()->SetValue(1);
				m_KeyDownList.push_back(ke);
			}
			// up action
			else if (ke.Action == KeyEvent::ACTION_UP)
			{
				if (currentState == 0)
					continue;
			
				myDeviceItems[ke.KeyCode]->getState()->SetValue(0);
				m_KeyUpList.push_back(ke);
			}
		}
		mEvents.clear();
	}
	
	if (!touchVector.empty())
	{
		EmitSignal(Signals::KeyboardEvent, touchVector);
	}
}

unsigned short	KeyboardWUP::ScanToChar(u32 scanCode)
{
	return (unsigned short)(scanCode);
}

bool	KeyboardWUP::Aquire()
{
	if (KeyboardDevice::Aquire())
	{
		return true;
	}
	return false;
}

bool	KeyboardWUP::Release()
{
	if (KeyboardDevice::Release())
	{
		return true;
	}
	return false;
}


void	KeyboardWUP::DoInputDeviceDescription()
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
