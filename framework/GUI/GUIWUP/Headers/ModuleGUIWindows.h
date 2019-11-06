// Doxygen group defines

/*! \defgroup GUIWindowsModule  GUI Windows module 
*  \ingroup GUIModule
*  GUI Module Classes for windows
*/


#ifndef _MODULEGUIWINDOWS_H_
#define _MODULEGUIWINDOWS_H_

#include "ModuleBase.h"
#include "ModuleGUI.h"

class ModuleGUIWindows;

// ****************************************
// * ModuleGUIWindows class
// * --------------------------------------
/*!  \class ModuleGUIWindows
GUI module using WIN32 API
\ingroup GUIWindowsModule
*/
// ****************************************

class ModuleGUIWindows : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleGUIWindows,ModuleBase,GUI)

	ModuleGUIWindows(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params); 
	void Close();
	virtual void Update(const Timer& timer, void* addParam);
	
protected:

	virtual ~ModuleGUIWindows();

};

#endif //_MODULEGUIWINDOWS_H_
