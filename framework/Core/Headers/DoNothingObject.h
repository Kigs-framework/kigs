#ifndef _DONOTHINGOBJJECT_H_
#define _DONOTHINGOBJJECT_H_


#include "CoreModifiable.h"
#include "Core.h"

// ****************************************
// * DoNothingObject class
// * --------------------------------------
/**
 * \file	DoNothingObject.h
 * \class	DoNothingObject
 * \ingroup Core
 * \brief	Object which do nothing
 * \author	ukn
 * \version ukn
 * \date	ukn
 *
 * well! an instanciable modifiable that do nothing
 * used to create an instance of unknown object instead of returning a NULL pointer
 */
// ****************************************
class DoNothingObject : public CoreModifiable
{
public:

	// instanciable class derived from CoreModifiable
	DECLARE_CLASS_INFO(DoNothingObject,CoreModifiable,KigsCore)

	/**
	 * \brief	constructor
	 * \fn 		DoNothingObject(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	DoNothingObject(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/**
	 * \fn 		virtual ~DoNothingObject(){};
	 * \brief	destructor
	 * 
	 * Do nothing
	 */
	virtual ~DoNothingObject(){};
};

#endif //_DONOTHINGOBJJECT_H_
