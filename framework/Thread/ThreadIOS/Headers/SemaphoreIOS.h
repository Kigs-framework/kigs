#ifndef _SEMAPHOREIOS_H_
#define _SEMAPHOREIOS_H_

#include "Semaphore.h"
#include <pthread.h>

// ****************************************
// * SemaphoreIOS class
// * --------------------------------------
/*!  \class SemaphoreIOS
      IPhone semaphore class
	 \ingroup ThreadIOS
*/
// ****************************************

class SemaphoreIOS : public Semaphore
{
public:
	DECLARE_CLASS_INFO(SemaphoreIOS, Semaphore, Thread)

	SemaphoreIOS(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	/*! \brief addItem
		overload from CoreModifiable
	*/
	virtual bool	addItem(CoreModifiable *item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME);

	/*! \brief removeItem
		overload from CoreModifiable
	*/
	virtual bool	removeItem(CoreModifiable *item DECLARE_DEFAULT_LINK_NAME);
protected:
   
	//! destructor
	virtual ~SemaphoreIOS();

    pthread_mutex_t myMutexLock;
};    

#endif //_SEMAPHOREIOS_H_
