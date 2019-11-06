#include "PrecompiledHeaders.h"
#include "ThreadEventWin32.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(ThreadEventWin32)


ThreadEventWin32::ThreadEventWin32(const kstl::string& name, CLASS_NAME_TREE_ARG) : ThreadEvent(name, PASS_CLASS_NAME_TREE_ARG)
{
	/*myHandle = 0;
	//We replaced the semaphore by a mutex. The difference is that mutexes allow for reentrency
	myHandle = CreateEvent(
		NULL,           // default security attributes
		FALSE,			// manual reset
		FALSE,			// initial state
		NULL);          // unnamed 
	*/
	InitializeConditionVariable(&myConditionVariable);
	InitializeCriticalSection(&myCriticalSection);
}

ThreadEventWin32::~ThreadEventWin32()
{
	/*if (myHandle)
	{
		CloseHandle(myHandle);
		myHandle = 0;
	}*/
	DeleteCriticalSection(&myCriticalSection);

}

void	ThreadEventWin32::platformWait()
{
	
	EnterCriticalSection(&myCriticalSection);

	while (myCurrentCount < myEventCounter)
		SleepConditionVariableCS(&myConditionVariable, &myCriticalSection, INFINITE);

	if(myAutoReset)
		myCurrentCount = 0;

	LeaveCriticalSection(&myCriticalSection);


	
	/*WaitForSingleObject(
			myHandle,   // handle to mutex
			INFINITE);           // wait infinite ?*/
}
void	ThreadEventWin32::platformSignal()
{
	EnterCriticalSection(&myCriticalSection);
	myCurrentCount++;
	WakeConditionVariable(&myConditionVariable);
	LeaveCriticalSection(&myCriticalSection);

	//SetEvent(myHandle);
}
