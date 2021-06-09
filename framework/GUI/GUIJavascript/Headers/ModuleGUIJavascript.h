
#ifndef _MODULEGUIJAVASCRIPT_H_
#define _MODULEGUIJAVASCRIPT_H_

#include "ModuleBase.h"

class ModuleGUIJavascript;
extern ModuleGUIJavascript* gInstanceModuleGUIJavascript;


#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformGUIModuleInit
extern SP<ModuleBase> PlatformGUIModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

// ****************************************
// * ModuleGUIJavascript class
// * --------------------------------------
/**
* \file	ModuleGUIJavascript.h
* \class	ModuleGUIJavascript
* \ingroup GUIModule
* \ingroup Module
* \brief Specific Emscripten GUI module
*
*/
// ****************************************

class ModuleGUIJavascript : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleGUIJavascript,ModuleBase,GUI)
             
	ModuleGUIJavascript(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override; 
    void Close() override;
	void Update(const Timer& timer, void* addParam) override;
     virtual ~ModuleGUIJavascript();
 
protected:
	
 
}; 

#endif //_MODULEGUIJAVASCRIPT_H_
