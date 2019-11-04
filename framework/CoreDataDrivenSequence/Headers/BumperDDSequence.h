#ifndef _BUMPERDDSEQUENCE_H_
#define _BUMPERDDSEQUENCE_H_

#include "BaseDDSequence.h"
#include "maUSString.h"

class BumperDDSequence : public BaseDDSequence
{
public:
	DECLARE_CLASS_INFO(BumperDDSequence, BaseDDSequence, CoreDataDrivenSequence)

	//! constructor
	BumperDDSequence(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

protected:
	//! destructor
	virtual ~BumperDDSequence();

	void Update(const Timer&  timer, void* /*addParam*/) override;

	maUSString myNextSequence;
	maFloat myDuration;

	kdouble mStartTime;
};
#endif //_BUMPERDDSEQUENCE_H_
