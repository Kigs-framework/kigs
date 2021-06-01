#ifndef _MODULEINPUTJAVASCRIPT_H_
#define _MODULEINPUTJAVASCRIPT_H_

#include "ModuleBase.h"

class ModuleInputJavascript;
extern ModuleInputJavascript* gInstanceModuleInputJavascript;



#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformInputModuleInit
extern SP<ModuleBase> PlatformInputModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

// ****************************************
// * ModuleInputJavascript class
// * --------------------------------------
/**
 * \class	ModuleInputJavascript
 * \file	ModuleInputJavascript.h
 * \ingroup Input
 * \ingroup Module
 * \brief	Javascript specific input module.
 */
 // ****************************************

class ModuleInputJavascript : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleInputJavascript,ModuleBase,Input)
             
	ModuleInputJavascript(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
    void Close() override;
	void Update(const Timer& timer, void* addParam) override;

	bool	isInit(){return mIsInitOK;}

	unsigned int	getJoystickCount(){return mJoystickCount;}

	bool addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
   virtual ~ModuleInputJavascript();

protected:
 
	bool					mIsInitOK;
	unsigned int			mJoystickCount;

}; 

#endif //_MODULEINPUTJAVASCRIPT_H_
