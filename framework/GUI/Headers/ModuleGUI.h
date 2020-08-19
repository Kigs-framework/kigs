// Doxygen group defines

/*! \defgroup GUIModule GUI base module
*  GUIModule Classes
*/

#ifndef _MODULEGUI_H_
#define _MODULEGUI_H_

#include "ModuleBase.h"

ModuleBase* PlatformGUIModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);

// ****************************************
// * ModuleGUI class
// * --------------------------------------
/**
* \file	ModuleGUI.h
* \class	ModuleGUI
* \ingroup GUIModule
* \ingroup Module
* \brief Generic Module for GUI classes ( Windows management )
*
*/
// ****************************************

class ModuleGUI : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleGUI,ModuleBase,GUI)

	//! constructor
	ModuleGUI(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	//! init module
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;

	//! close module
	void Close() override;

	//! update module
	void Update(const Timer& timer, void* addParam) override;

protected:

	//! destructor
	virtual ~ModuleGUI();

}; 

#endif //_MODULEGUI_H_
