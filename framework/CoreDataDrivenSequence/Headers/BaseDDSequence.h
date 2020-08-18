#ifndef _BASEDDSEQUENCE_H_
#define _BASEDDSEQUENCE_H_

#include "DataDrivenBaseApplication.h"

class CoreSequence;
// ****************************************
// * BaseDDSequence class
// * --------------------------------------
/**
 * \file	BaseDDSequence.h
 * \class	BaseDDSequence
 * \ingroup DataDrivenApplication
 * \brief	A data driven sequence.
 * 
 * ?? Obsolete ?? just a DataDrivenSequence ? 
 */
 // ****************************************

class BaseDDSequence : public DataDrivenSequence
{
public:
	DECLARE_CLASS_INFO(BaseDDSequence, DataDrivenSequence, CoreDataDrivenSequence)

	//! constructor
	BaseDDSequence(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

protected:
	//! destructor
	virtual ~BaseDDSequence();

	maString mTransitionData;
};

#endif //_BASEDDSEQUENCE_H_
