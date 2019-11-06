#ifndef _THREADLOCALSTORAGEMANAGERWIN32_H_
#define _THREADLOCALSTORAGEMANAGERWIN32_H_

#include <windows.h>
#include "ThreadLocalStorageManager.h"


// ****************************************
// * ThreadEventWin32 class
// * --------------------------------------
/*!  \class ThreadEventWin32
Windows semaphore class
\ingroup ThreadWindows
*/
// ****************************************

class ThreadLocalStorageManagerWin32 : public ThreadLocalStorageManager
{
public:
	DECLARE_CLASS_INFO(ThreadLocalStorageManagerWin32, ThreadLocalStorageManager, Thread)

	ThreadLocalStorageManagerWin32(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	virtual int AllocateLocalStorage();
	virtual void* GetLocalStorage(unsigned int index);
	virtual bool SetLocalStorage(unsigned int index, void* data);
	virtual void FreeLocalStorage(unsigned int index);

protected:

	//! destructor
	virtual ~ThreadLocalStorageManagerWin32();



};

#endif //_THREADLOCALSTORAGEMANAGERWIN32_H_
