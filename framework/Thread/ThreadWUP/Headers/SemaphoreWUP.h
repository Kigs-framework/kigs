#ifndef _SEMAPHOREWUP_H_
#define _SEMAPHOREWUP_H_

#include <windows.h>
#include "Semaphore.h"


// ****************************************
// * SemaphoreWUP class
// * --------------------------------------
/*!  \class SemaphoreWUP
      Windows semaphore class
	 \ingroup ThreadWindows
*/
// ****************************************

class SemaphoreWUP : public Semaphore
{
public:
    DECLARE_CLASS_INFO(SemaphoreWUP,Semaphore,Thread)

    SemaphoreWUP(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	/*! \brief addItem
		overload from CoreModifiable
	*/
	virtual bool				addItem(CoreModifiable *item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME);

	/*! \brief removeItem
		overload from CoreModifiable
	*/
	virtual bool				removeItem(CoreModifiable *item DECLARE_DEFAULT_LINK_NAME);
protected:
   
	//! destructor
	virtual ~SemaphoreWUP();

	HANDLE	myHandle;

};    

#endif //_SEMAPHOREWUP_H_
