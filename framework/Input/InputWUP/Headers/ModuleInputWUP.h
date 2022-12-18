#ifndef _MODULEINPUTWUP_H_
#define _MODULEINPUTWUP_H_

#include "ModuleBase.h"
#include "ModuleInput.h"

// ****************************************
// * ModuleInputWUP class
// * --------------------------------------
/**
 * \class	ModuleInputWUP
 * \file	ModuleInputWUP.h
 * \ingroup Input
 * \ingroup Module
 * \brief	UWP specific input module.
 */
 // ****************************************


class ModuleInputWUP : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleInputWUP,ModuleBase,Input)
	ModuleInputWUP(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~ModuleInputWUP();

    void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params); 
    void Close();
	virtual void Update(const Timer& timer, void* addParam);
	    
	bool	isInit(){return mIsInitOK;}

	unsigned int	getJoystickCount(){return mJoystickCount;}

	virtual bool	addItem(const CMSP& item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME) override;

protected:
	bool					mIsInitOK;
	unsigned int			mJoystickCount;

}; 

#endif //_MODULEINPUTWUP_H_
