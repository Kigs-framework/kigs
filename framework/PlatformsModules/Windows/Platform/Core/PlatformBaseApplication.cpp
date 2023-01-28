#include "PrecompiledHeaders.h"
#include "PlatformBaseApplication.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Shellapi.h>

#ifdef KIGS_INPUT_AVAILABLE
#include "../../../../Input/Headers/ModuleInput.h"
#include "../../../../Input/Headers/KeyboardDevice.h"
#include "../../../../Input/Headers/DeviceItem.h"
#include "Platform/Input/KeyDefine.h"
#endif

#include "CoreBaseApplication.h"

#include "winInet.h"

using namespace Kigs::Core;

void	PlatformBaseApplication::OpenLink(const char* a_link)
{
	ShellExecuteA(0, "open", a_link, 0, 0, SW_SHOW);
}

void	PlatformBaseApplication::OpenLink(const unsigned short* a_link, unsigned int a_length)
{
	ShellExecuteW(0, 0, (LPCWSTR)a_link, 0, 0, SW_SHOW);
}

const char* platformName = "Win32";

const char* PlatformBaseApplication::getPlatformName()
{
	return platformName;
}

bool	PlatformBaseApplication::CheckBackKeyPressed()
{
#ifdef KIGS_INPUT_AVAILABLE
	// TODO check if esc key is pressed here ?

	if (mKeyboard == 0)
	{
		static int countTest = 0;
		if (countTest == 0)
		{
			ModuleInput* theInputModule = static_cast<ModuleInput*>(CoreGetModule(ModuleInput));
			if (theInputModule)
			{
				mKeyboard = theInputModule->GetKeyboard();
			}
			else
			{
				countTest = 10;
			}
		}
		countTest--;
	}

	if (mKeyboard)
	{
		if (mKeyboard->getKeyState(CM_KEY_ESCAPE).GetTypedValue(int))
		{
			return true;
		}
	}
#endif
	return false;
}

//#define CHECK_INTERNET
bool	PlatformBaseApplication::CheckConnexion()
{
#if CHECK_INTERNET
	bool bConnect = InternetCheckConnectionA("http://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0);
	return bConnect;
#else
	return false;
#endif
}

// get number of core / processor
unsigned int PlatformBaseApplication::getProcessorCount()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );

	return sysinfo.dwNumberOfProcessors;
}

int PlatformBaseApplication::getCpuId() {


	unsigned cpu = GetCurrentProcessorNumber();
		
	return (int)cpu;
	
}

void		PlatformBaseApplication::setCurrentThreadAffinityMask(int mask)
{
	SetThreadAffinityMask(GetCurrentThread(), mask);
}

void		PlatformBaseApplication::setThreadAffinityMask(void* h,int mask)
{
	SetThreadAffinityMask(h, mask);
}