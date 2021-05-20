
#ifndef _MODULEGUIWINDOWS_H_
#define _MODULEGUIWINDOWS_H_

#include "ModuleBase.h"

class ModuleGUIWindows;

#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformGUIModuleInit
extern SP<ModuleBase> PlatformGUIModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

// ****************************************
// * ModuleGUIWindows class
// * --------------------------------------
/**
* \file	ModuleGUIWindows.h
* \class	DisplayDeviceCaps
* \ingroup GUIModule
* \ingroup Module
* \brief	Specific Win32 GUI module.
*/
// ****************************************

class ModuleGUIWindows : public ModuleBase
{
public:
	DECLARE_CLASS_INFO(ModuleGUIWindows,ModuleBase,GUI)
	ModuleGUIWindows(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~ModuleGUIWindows();

	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params); 
	void Close();
	virtual void Update(const Timer& timer, void* addParam);
};

#endif //_MODULEGUIWINDOWS_H_
