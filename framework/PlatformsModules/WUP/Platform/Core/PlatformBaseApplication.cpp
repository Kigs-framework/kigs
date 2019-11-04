#include "PrecompiledHeaders.h"
#include "PlatformBaseApplication.h"
#include "CoreIncludes.h"
#include <Shellapi.h>
#include "../../../../Input/Headers/ModuleInput.h"
#include "../../../../Input/Headers/KeyboardDevice.h"
#include "../../../../Core/Headers/CoreBaseApplication.h"
#include "Platform/Input/KeyDefine.h"

void	PlatformBaseApplication::OpenLink(const char* a_link)
{
	//TODO
//	ShellExecuteA(0, "open", a_link, 0, 0, SW_SHOW);
}

void	PlatformBaseApplication::OpenLink(const unsigned short* a_link, unsigned int a_length)
{
	//TODO
	//	ShellExecuteW(0, 0, (LPCWSTR)a_link, 0, 0, SW_SHOW);
}


const char* platformName = "WUP";

const char* PlatformBaseApplication::getPlatformName()
{
	return platformName;
}


bool	PlatformBaseApplication::CheckBackKeyPressed()
{
	// TODO check if esc key is pressed here ?

	/*if (myKeyboard == 0)
	{
		static int countTest = 0;
		if (countTest == 0)
		{
			ModuleInput* theInputModule = reinterpret_cast<ModuleInput*>(CoreGetModule(ModuleInput));
			if (theInputModule)
			{
				myKeyboard = theInputModule->GetKeyboard();
			}
			else
			{
				countTest = 10;
			}
		}
		countTest--;
	}

	if (myKeyboard)
	{
		if (myKeyboard->getKeyState(CM_KEY_ESCAPE).GetValue(int))
		{
			return true;
		}
	}*/
	return false;
}

bool	PlatformBaseApplication::IsHolographic()
{
	return mIsHolographic;
}

bool	PlatformBaseApplication::CheckConnexion()
{
	// TODO
/*	bool bConnect = InternetCheckConnectionA("http://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0);
	return bConnect;*/
	return false;
}

// get number of core / processor
unsigned int PlatformBaseApplication::getProcessorCount()
{
	printf("TODO unsigned int PlatformBaseApplication::getProcessorCount()\n");
	//SYSTEM_INFO sysinfo;
	//GetSystemInfo( &sysinfo );

	//return sysinfo.dwNumberOfProcessors;
	return 1;
}

int PlatformBaseApplication::getCpuId() {
	// TODO
/*
	unsigned cpu = GetCurrentProcessorNumber();
		
	return (int)cpu;*/

	return 1;
	
}

void		PlatformBaseApplication::setCurrentThreadAffinityMask(int mask)
{// TODO
 /*
	SetThreadAffinityMask(GetCurrentThread(), mask);*/
}

void		PlatformBaseApplication::setThreadAffinityMask(void* h,int mask)
{
	// TODO
	/*
	SetThreadAffinityMask(h, mask);*/
}