#ifndef _MODULEDDSEQUENCE_H_
#define _MODULEDDSEQUENCE_H_

#include "ModuleBase.h"
#include "CoreMap.h"

/*! \defgroup DataDrivenApplication
 *
*/

// ****************************************
// * ModuleDDSequence class
// * --------------------------------------
/**
 * \file	ModuleDDSequence.h
 * \class	ModuleDDSequence
 * \ingroup DataDrivenApplication
 * \ingroup Module
 * \brief	Manage Data Driven Sequences for Data Driven Application.
 */
 // ****************************************


class ModuleDDSequence : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleDDSequence, ModuleBase, CoreDataDrivenSequence)

	//! constructor
	ModuleDDSequence(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
         
	//! init module
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
	//! close module
    void Close() override;
            
	//! update module
	void Update(const Timer& timer, void* addParam) override;

}; 

#endif //_MODULEDDSEQUENCE_H_
