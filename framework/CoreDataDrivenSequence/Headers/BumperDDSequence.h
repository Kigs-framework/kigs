#ifndef _BUMPERDDSEQUENCE_H_
#define _BUMPERDDSEQUENCE_H_

#include "BaseDDSequence.h"
#include "maUSString.h"

// ****************************************
// * BumperDDSequence class
// * --------------------------------------
/**
 * \file	BumperDDSequence.h
 * \class	BumperDDSequence
 * \ingroup DataDrivenApplication
 * \brief	Splash screen
 *
 * ?? Obsolete ??
 */
 // ****************************************

class BumperDDSequence : public BaseDDSequence
{
public:
	DECLARE_CLASS_INFO(BumperDDSequence, BaseDDSequence, CoreDataDrivenSequence)
	BumperDDSequence(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~BumperDDSequence();

protected:
	void Update(const Timer&  timer, void* /*addParam*/) override;

	maUSString mNextSequence;
	maFloat mDuration;

	kdouble mStartTime;
};
#endif //_BUMPERDDSEQUENCE_H_
