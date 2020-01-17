#include "PrecompiledHeaders.h"
#include "SemaphoreWin32.h"
#include "Core.h"

IMPLEMENT_CLASS_INFO(SemaphoreWin32)


SemaphoreWin32::SemaphoreWin32(const kstl::string& name,CLASS_NAME_TREE_ARG) : Semaphore(name,PASS_CLASS_NAME_TREE_ARG)
{
	myHandle=0;
	//We replaced the semaphore by a mutex. The difference is that mutexes allow for reentrency
	myHandle=CreateMutex( 
        NULL,           // default security attributes
        FALSE,			// maximum count
        NULL);          // unnamed mutex

}

SemaphoreWin32::~SemaphoreWin32()
{  
	if(myHandle)
	{
		CloseHandle(myHandle);
		myHandle=0;
	}

}    

bool	SemaphoreWin32::addItem(CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	DWORD dwWaitResult; 
    // Try to enter the mutex gate.

    dwWaitResult = WaitForSingleObject( 
        myHandle,   // handle to mutex
        INFINITE);           // wait infinite ?
	
	// don't call father addItem, else an infinite loop is made
	//Semaphore::addItem(item,pos PASS_LINK_NAME(linkName));

	return true;
}

bool	SemaphoreWin32::removeItem(CMSP& item DECLARE_LINK_NAME)
{
	ReleaseMutex( myHandle );

	return true; 
	//Semaphore::removeItem(item);
}
