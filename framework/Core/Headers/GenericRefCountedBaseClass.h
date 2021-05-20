#ifndef _GENERICREFCOUNTEDBASECLASS_H_
#define _GENERICREFCOUNTEDBASECLASS_H_


#include <atomic>
#include <vector>
#include <memory>

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

#define GenericRefCountedBaseClassLeakCheck
#ifdef GenericRefCountedBaseClassLeakCheck
#include <unordered_set>
#include <shared_mutex>
class GenericRefCountedBaseClass;
inline std::shared_mutex AllObjectsMutex;
inline std::unordered_set<GenericRefCountedBaseClass*> AllObjects;
#endif

class GenericRefCountedBaseClass : public std::enable_shared_from_this<GenericRefCountedBaseClass>
{
public:
	typedef bool (GenericRefCountedBaseClass::* ModifiableMethod)(CoreModifiable* sender, std::vector<CoreModifiableAttribute*>&, void* privateParams);
	GenericRefCountedBaseClass() 
	{
#ifdef GenericRefCountedBaseClassLeakCheck
		std::lock_guard<std::shared_mutex> lk{ AllObjectsMutex };
		AllObjects.insert(this);
#endif
	}

	virtual ~GenericRefCountedBaseClass() 
	{
#ifdef GenericRefCountedBaseClassLeakCheck
		std::lock_guard<std::shared_mutex> lk{ AllObjectsMutex };
		AllObjects.erase(this);
#endif
	};
};

#endif //_GENERICREFCOUNTEDBASECLASS_H_
