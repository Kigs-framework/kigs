#ifndef _MODULETIMER_H_
#define _MODULETIMER_H_

#include "ModuleBase.h"
/*! \defgroup TimerModule manage time
*/

// ****************************************
// * ModuleTimer class
// * --------------------------------------
/**
 * \file	ModuleTimer.h
 * \class	ModuleTimer
 * \ingroup TimerModule
 * \ingroup Module
 * \brief	Generic module for time management.
 */
 // ****************************************
class ModuleTimer : public ModuleBase
{
public:
	DECLARE_CLASS_INFO(ModuleTimer,ModuleBase,Timer)
    ModuleTimer(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;
    void Close() override;
	void Update(const Timer& timer, void* addParam) override;
    virtual ~ModuleTimer();
protected:
}; 

#endif //_MODULETIMER_H_
