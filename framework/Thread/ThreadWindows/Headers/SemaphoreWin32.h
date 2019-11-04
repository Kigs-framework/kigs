#ifndef _SEMAPHOREWIN32_H_
#define _SEMAPHOREWIN32_H_

#include <windows.h>
#include "Semaphore.h"


// ****************************************
// * SemaphoreWin32 class
// * --------------------------------------
/*!  \class SemaphoreWin32
      Windows semaphore class
	 \ingroup ThreadWindows
*/
// ****************************************

class SemaphoreWin32 : public Semaphore
{
public:
    DECLARE_CLASS_INFO(SemaphoreWin32,Semaphore,Thread)

    SemaphoreWin32(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
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
	virtual ~SemaphoreWin32();

	HANDLE	myHandle;

};    

#endif //_SEMAPHOREWIN32_H_
