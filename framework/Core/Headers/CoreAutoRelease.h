#ifndef _COREAUTORELEASE_H
#define _COREAUTORELEASE_H

#include "RefCountedBaseClass.h"
#include "kstlset.h"

// ****************************************
// * CoreAutoRelease class
// * --------------------------------------
/**
* \class	CoreAutoRelease
* \ingroup KigsCore
* \brief	finaly kill instances with refcount == 0
* \author	ukn
* \version ukn
* \date	ukn
*/
// ****************************************

class CoreAutoRelease
{
public:
	CoreAutoRelease();
	virtual ~CoreAutoRelease();

	// destroy instances in list
	void	doAutoRelease();

	void	addInstance(RefCountedBaseClass* torelease);
	void	removeInstance(RefCountedBaseClass* torelease);

protected:
	bool							myInAutoReleaseLoop;

	kstl::set<RefCountedBaseClass*>	myReleaseSet;
};

#endif // _COREAUTORELEASE_H