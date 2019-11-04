#ifndef _GENERICREFCOUNTEDBASECLASS_H_
#define _GENERICREFCOUNTEDBASECLASS_H_

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
public:

	GenericRefCountedBaseClass() : myRefCounter(1)
	{}


	/**
	* \fn 		virtual void    Destroy();
	* \brief	destructor (Design Patern smart pointer)
	*
	* destroy the object if there is no pointer to it
	*/
	virtual void			Destroy();

	/**
	* \fn		void            GetRef();
	* \brief	increment reference count
	*/
	virtual void            GetRef();

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
