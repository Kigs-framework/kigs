#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include "CoreModifiable.h"

// ****************************************
// * Semaphore class
// * --------------------------------------
/**
 * \file	Semaphore.h
 * \class	Semaphore
 * \ingroup Thread
 * \brief	Thread management.
 */
 // ****************************************
class Semaphore : public CoreModifiable
{
public:

    DECLARE_CLASS_INFO(Semaphore,CoreModifiable,Thread)
    Semaphore(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~Semaphore();

	auto& GetPrivateMutex() { return mPrivateMutex; }

protected:
	// CoreModifiable mutex is recursive... Here we want a non recursive semaphore
	std::mutex	mPrivateMutex;
};

#endif //_SEMAPHORE_H_
