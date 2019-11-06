// Doxygen group defines

/*! \defgroup GUIModule GUI base module
*  GUIModule Classes
*/

#ifndef _MODULEGUI_H_
#define _MODULEGUI_H_

#include "ModuleBase.h"
//#include "kstlvector.h"

// ****************************************
// * ModuleGUI class
// * --------------------------------------
/*!  \class ModuleGUI
base class for GUI module
\ingroup GUIModule
*/
// ****************************************

ModuleBase* PlatformGUIModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);

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
