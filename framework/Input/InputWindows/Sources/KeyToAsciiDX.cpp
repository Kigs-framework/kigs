#include "PrecompiledHeaders.h"
#include "Windows.h"
#include "KeyToAsciiDX.h"
#include "KeyboardDX.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(KeyToAsciiDX, KeyToAscii, Input);
IMPLEMENT_CLASS_INFO(KeyToAsciiDX)

KeyToAsciiDX::KeyToAsciiDX(const kstl::string& name,CLASS_NAME_TREE_ARG) : KeyToAscii(name,PASS_CLASS_NAME_TREE_ARG)
{
}

int KeyToAsciiDX::Convert(unsigned Code)
{
	WORD result[2];
	BYTE State[256];
	::UINT vk;
	HKL layout=GetKeyboardLayout(0);
	if (GetKeyboardState(State)==FALSE) return 0;
	switch(Code)
	{
		case DIK_NUMPAD0 : if(State[VK_NUMLOCK]) vk=VK_NUMPAD0;break;
		case DIK_NUMPAD1 : if(State[VK_NUMLOCK]) vk=VK_NUMPAD1;break;
		case DIK_NUMPAD2 : if(State[VK_NUMLOCK]) vk=VK_NUMPAD2;break;
		case DIK_NUMPAD3 : if(State[VK_NUMLOCK]) vk=VK_NUMPAD3;break;
		case DIK_NUMPAD4 : if(State[VK_NUMLOCK]) vk=VK_NUMPAD4;break;
		case DIK_NUMPAD5 : if(State[VK_NUMLOCK]) vk=VK_NUMPAD5;break;
		case DIK_NUMPAD6 : if(State[VK_NUMLOCK]) vk=VK_NUMPAD6;break;
		case DIK_NUMPAD7 : if(State[VK_NUMLOCK]) vk=VK_NUMPAD7;break;
		case DIK_NUMPAD8 : if(State[VK_NUMLOCK]) vk=VK_NUMPAD8;break;
		case DIK_NUMPAD9 : if(State[VK_NUMLOCK]) vk=VK_NUMPAD9;break;
		case DIK_DECIMAL: if (State[VK_NUMLOCK]) vk =VK_DECIMAL; break;
		case DIK_DIVIDE: vk = VK_DIVIDE; break;
		case DIK_SUBTRACT: vk = VK_SUBTRACT; break;
		case DIK_ADD: vk = VK_ADD; break;
		case DIK_NUMPADENTER: vk = VK_RETURN; break;
		default:
			vk =MapVirtualKeyEx(Code,1,layout);
			break;
	}
	int r = ToAsciiEx(vk,Code,State,result,0,layout);
	if (r==1)
	{
		int c = result[0] & 0xFF;
//		if (c>='a' && c<='z' && (State[DIK_LSHIFT] || State[DIK_RSHIFT])) c-=30;
		return c; 
	}
	return 0;
}
