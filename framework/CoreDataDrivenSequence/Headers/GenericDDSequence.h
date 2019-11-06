#ifndef _GENERICDDSEQUENCE_H_
#define _GENERICDDSEQUENCE_H_

#include "BaseDDSequence.h"
#include "maUSString.h"

class GenericDDSequence : public BaseDDSequence
{
public:
	DECLARE_CLASS_INFO(GenericDDSequence, BaseDDSequence, CoreDataDrivenSequence)

	//! constructor
	GenericDDSequence(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

protected:
	//! destructor
	virtual ~GenericDDSequence();

	maUSString myNextSequence;
	maFloat myDuration;

	kdouble mStartTime;
};
#endif //_GENERICDDSEQUENCE_H_
