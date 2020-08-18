#ifndef _GENERICREFCOUNTEDBASECLASS_H_
#define _GENERICREFCOUNTEDBASECLASS_H_


#include <atomic>
#include <vector>

class CoreModifiable;
class CoreModifiableAttribute;

// ****************************************
// * GenericRefCountedBaseClass class
// * --------------------------------------
/**
* \file	GenericRefCountedBaseClass.h
* \class	GenericRefCountedBaseClass
* \ingroup Core
* \brief	Base class for refcounting classes, no other dependencies
*/
// ****************************************

#ifdef KIGS_TOOLS
#define TRACEREF_VIRTUAL virtual
#else
#define TRACEREF_VIRTUAL 
#endif



class GenericRefCountedBaseClass
{
public:

	typedef bool (GenericRefCountedBaseClass::* ModifiableMethod)(CoreModifiable* sender, std::vector<CoreModifiableAttribute*>&, void* privateParams);
	GenericRefCountedBaseClass() {}
	
	TRACEREF_VIRTUAL void GetRef();
	TRACEREF_VIRTUAL bool TryGetRef();
	TRACEREF_VIRTUAL void Destroy();

	inline int getRefCount() { return mRefCounter; }

protected:
	std::atomic_int	mRefCounter{1};

	// if true is returned then don't do final delete
	virtual bool checkDestroy()
	{
		return false;
	}
	virtual ~GenericRefCountedBaseClass() {};
};

#endif //_GENERICREFCOUNTEDBASECLASS_H_
