#include "PrecompiledHeaders.h"
#include "ThreadRunMethodWin32.h"
#include "Core.h"
#include "Thread.h"
#include "CoreBaseApplication.h"

IMPLEMENT_CLASS_INFO(ThreadRunMethodWin32)

ThreadRunMethodWin32::ThreadRunMethodWin32(const kstl::string& name,CLASS_NAME_TREE_ARG) : ThreadRunMethod(name,PASS_CLASS_NAME_TREE_ARG)
{
	myHandle=0;
}

ThreadRunMethodWin32::~ThreadRunMethodWin32()
{  
	if(myHandle)
	{
		CloseHandle(myHandle);
		myHandle=0;
	}
}

void ThreadRunMethodWin32::EndThread()
{
	if(myHandle)
	{
		// seems a bad idea
		TerminateThread(myHandle,0); 
		CloseHandle(myHandle);
		myHandle=0;
	}
}

void ThreadRunMethodWin32::StartThread()
{
    myHandle = CreateThread( 
            NULL,					// default security attributes
            0,						// use default stack size  
            (LPTHREAD_START_ROUTINE)GetThreadRunMethod(),          // thread function 
            myThread,				// argument to thread function 
            0,						// use default creation flags 
            &myThreadId);			// returns the thread identifier 

}

void	ThreadRunMethodWin32::setAffinityMask(int mask)
{
	CoreBaseApplication::setThreadAffinityMask(myHandle,mask);
}

void ThreadRunMethodWin32::sleepThread()
{
	SuspendThread(myHandle);
}

void ThreadRunMethodWin32::wakeUpThread()
{
	ResumeThread(myHandle);
}

void ThreadRunMethodWin32::waitDeath(unsigned long P_Time_Out)
{
	WaitForSingleObject(myHandle,P_Time_Out);
}

