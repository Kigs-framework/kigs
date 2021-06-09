#ifndef _MODULEINPUTANDROID_H_
#define _MODULEINPUTANDROID_H_

#include "ModuleBase.h"



#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformInputModuleInit
extern SP<ModuleBase> PlatformGUIModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

// ****************************************
// * ModuleInputAndroid class
// * --------------------------------------
/**
 * \file	ModuleInputAndroid.h
 * \class	ModuleInputAndroid
 * \ingroup Input
 * \ingroup Module
 * \brief	Specific module for android input management.
 */
 // ****************************************

class ModuleInputAndroid : public ModuleBase
{
public:
	DECLARE_CLASS_INFO(ModuleInputAndroid,ModuleBase,Input)
	DECLARE_CONSTRUCTOR(ModuleInputAndroid);
	
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
    void Close() override;
    void Update(const Timer& timer,void* /*addParam*/) override;

	bool	isInit(){return mIsInitOK;}

	unsigned int	getJoystickCount(){return mJoystickCount;}

	bool	addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;

protected:
	bool					mIsInitOK;
	unsigned int			mJoystickCount;

}; 

#endif //_MODULEINPUTANDROID_H_
