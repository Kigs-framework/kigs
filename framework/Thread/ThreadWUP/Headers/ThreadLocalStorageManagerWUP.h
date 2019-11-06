#ifndef _THREADLOCALSTORAGEMANAGERWUP_H_
#define _THREADLOCALSTORAGEMANAGERWUP_H_

#include <windows.h>
#include "ThreadLocalStorageManager.h"


// ****************************************
// * ThreadEventWUP class
// * --------------------------------------
/*!  \class ThreadEventWUP
Windows semaphore class
\ingroup ThreadWindows
*/
// ****************************************

class ThreadLocalStorageManagerWUP : public ThreadLocalStorageManager
{
public:
	DECLARE_CLASS_INFO(ThreadLocalStorageManagerWUP, ThreadLocalStorageManager, Thread)

	ThreadLocalStorageManagerWUP(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	virtual int AllocateLocalStorage();
	virtual void* GetLocalStorage(unsigned int index);
	virtual bool SetLocalStorage(unsigned int index, void* data);
	virtual void FreeLocalStorage(unsigned int index);

protected:

	//! destructor
	virtual ~ThreadLocalStorageManagerWUP();



};

#endif //_THREADLOCALSTORAGEMANAGERWUP_H_
