#ifndef _SEMAPHOREANDROID_H_
#define _SEMAPHOREANDROID_H_

#include "Semaphore.h"
#include <pthread.h>


// ****************************************
// * SemaphoreAndroid class
// * --------------------------------------
/*!  \class SemaphoreAndroid
      Android semaphore class
	 \ingroup Thread
*/
// ****************************************

class SemaphoreAndroid : public Semaphore
{
public:
    DECLARE_CLASS_INFO(SemaphoreAndroid,Semaphore,Thread)

    SemaphoreAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	/*! \brief addItem
		overload from CoreModifiable
	*/
	bool addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;

	/*! \brief removeItem
		overload from CoreModifiable
	*/
	bool removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;
protected:
   
	//! destructor
	virtual ~SemaphoreAndroid();

	pthread_mutex_t mutexlock;
	pthread_mutexattr_t mutexAttr;

};    

#endif //_SEMAPHOREANDROID_H_
