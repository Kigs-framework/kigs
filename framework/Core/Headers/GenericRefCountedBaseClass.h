#ifndef _GENERICREFCOUNTEDBASECLASS_H_
#define _GENERICREFCOUNTEDBASECLASS_H_


#include <atomic>

// ****************************************
// * GenericRefCountedBaseClass class
// * --------------------------------------
/**
* \file	GenericRefCountedBaseClass.h
* \class	GenericRefCountedBaseClass
* \ingroup KigsCore
* \brief	Base class for refcounting classes, no other dependencies
* \author	ukn
* \version ukn
* \date	ukn
*/
// ****************************************
class GenericRefCountedBaseClass
{
protected:
	// if true is returned then don't do final delete
	virtual bool checkDestroy()
	{
		return false;
	}

public:

	GenericRefCountedBaseClass()
	{
	}

	void Destroy();

#if defined (_DEBUG) && ( defined(WIN32) || defined(WUP))
	virtual // GetRef is virtual only for debug purpose
#endif
	void GetRef();

	bool TryGetRef();

	inline int getRefCount() { return myRefCounter; }

protected:
	std::atomic_int	myRefCounter{1};

	virtual ~GenericRefCountedBaseClass() {};
};

#endif //_GENERICREFCOUNTEDBASECLASS_H_
