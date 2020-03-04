#ifndef _THREADLOCALSTORAGEMANAGERANDROID_H_
#define _THREADLOCALSTORAGEMANAGERANDROID_H_

#include <pthread.h>
#include "ThreadLocalStorageManager.h"


// ****************************************
// * ThreadLocalStorageManagerAndroid class
// * --------------------------------------
/*!  \class ThreadEventWin32
Android ThreadLocalStorageManager class
\ingroup ThreadAndroid
*/
// ****************************************

class ThreadLocalStorageManagerAndroid : public ThreadLocalStorageManager
{
public:
	DECLARE_CLASS_INFO(ThreadLocalStorageManagerAndroid, ThreadLocalStorageManager, Thread)

	ThreadLocalStorageManagerAndroid(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	int AllocateLocalStorage() override;
	void* GetLocalStorage(unsigned int index) override;
	bool SetLocalStorage(unsigned int index, void* data) override;
	void FreeLocalStorage(unsigned int index) override;

protected:

	//! destructor
	virtual ~ThreadLocalStorageManagerAndroid();

	kstl::vector<pthread_key_t> _key_index_assoc;

};

#endif //_THREADLOCALSTORAGEMANAGERANDROID_H_
