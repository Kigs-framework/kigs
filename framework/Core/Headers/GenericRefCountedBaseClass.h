#ifndef _GENERICREFCOUNTEDBASECLASS_H_
#define _GENERICREFCOUNTEDBASECLASS_H_

#include <mutex>

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

	virtual std::lock_guard<std::recursive_mutex>* lockForDestroy();
	void unlockForDestroy(std::lock_guard<std::recursive_mutex>* lk);

	// if true is returned then don't do final delete
	virtual bool checkDestroy()
	{
		return false;
	}

public:

	GenericRefCountedBaseClass() : myRefCounter(1)
	{}


	/**
	* \fn 		virtual void    Destroy();
	* \brief	destructor (Design Patern smart pointer)
	*
	* destroy the object if there is no pointer to it
	*/
	void			Destroy();

	/**
	* \fn		void            GetRef();
	* \brief	increment reference count
	*/

#if defined (_DEBUG) && ( defined(WIN32) || defined(WUP))
	virtual // GetRef is virtual only for debug purpose
#endif
	void            GetRef();

	//! get reference count
	/**
	* \fn		 int getRefCount() {return myRefCounter;}
	* \brief	 get reference count
	* \return	 the reference count
	*/
	inline int          getRefCount() { return myRefCounter; }

protected:

	//! reference counter
	int             myRefCounter;

	virtual         ~GenericRefCountedBaseClass()
	{
		// nothing more here
	}
};

#endif //_GENERICREFCOUNTEDBASECLASS_H_
