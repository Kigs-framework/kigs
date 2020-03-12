#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include "CoreModifiable.h"

// ****************************************
// * Semaphore class
// * --------------------------------------
/*!  \class Semaphore
      base class for semaphores
	 \ingroup Thread
*/
// ****************************************

class Semaphore : public CoreModifiable
{
public:

    DECLARE_CLASS_INFO(Semaphore,CoreModifiable,Thread)

	//! constructor
    Semaphore(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	auto& GetPrivateMutex() {return myPrivateMutex;}

protected:

	//! destructor
    virtual ~Semaphore();

	// CoreModifiable mutex is recursive... Here we want a non recursive semaphore
	std::mutex	myPrivateMutex;
};

#endif //_SEMAPHORE_H_
