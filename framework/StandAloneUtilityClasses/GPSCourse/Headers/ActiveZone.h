#ifndef _ACTIVEZONE_H_
#define _ACTIVEZONE_H_

#include "CoreModifiable.h"

class ActiveZone : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(ActiveZone, CoreModifiable, Core)

	/**
	* \brief	constructor
	* \fn 		EphebeApp(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	ActiveZone(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual											~ActiveZone(){}

protected:
};

#endif //_DataDrivenAppEditor_H_