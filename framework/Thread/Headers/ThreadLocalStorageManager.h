#ifndef _THREADLOCALSTORAGEMANAGER_H_
#define _THREADLOCALSTORAGEMANAGER_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "Thread.h"

// ****************************************
// * ThreadLocalStorageManager class
// * --------------------------------------
/*!  \class ThreadLocalStorageManager
interface to platform specific thread local storage
\ingroup Thread
*/
// ****************************************

class ThreadLocalStorageManager : public CoreModifiable
{
public:

	DECLARE_ABSTRACT_CLASS_INFO(ThreadLocalStorageManager, CoreModifiable, Thread)

	//! constructor
	ThreadLocalStorageManager(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void InitModifiable() override;

	virtual int AllocateLocalStorage() = 0;
	virtual void* GetLocalStorage(unsigned int index) = 0;
	virtual bool SetLocalStorage(unsigned int index, void* data) = 0;
	virtual void FreeLocalStorage(unsigned int index) = 0;

	void RegisterThread(CoreModifiable* thread)
	{
		SetLocalStorage(myThreadIndexTLS, thread);
	}

	CoreModifiable* GetCurrentThread()
	{
		return (CoreModifiable*)GetLocalStorage(myThreadIndexTLS);
	}

	void ProtectedDestroy() override;

protected:

	//! destructor
	virtual ~ThreadLocalStorageManager();
	unsigned int myThreadIndexTLS;

};

#endif //_THREADLOCALSTORAGEMANAGER_H_
