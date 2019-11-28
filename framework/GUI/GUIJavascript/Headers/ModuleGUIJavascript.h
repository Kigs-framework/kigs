// Doxygen group defines

 /*! \defgroup GUIModuleJavascript  GUI IPhone module 
  *  \ingroup GUIModule
  *  GUI Module Classes for Javascript (HTML5 canvas)
  */


#ifndef _MODULEGUIJAVASCRIPT_H_
#define _MODULEGUIJAVASCRIPT_H_

#include "ModuleBase.h"

class ModuleGUIJavascript;
extern ModuleGUIJavascript* gInstanceModuleGUIJavascript;


#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformGUIModuleInit
extern ModuleBase* PlatformGUIModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

// ****************************************
// * ModuleGUIJavascript class
// * --------------------------------------
/*!  \class ModuleGUIJavascript
     GUI module using Javascript
     \ingroup GUIModuleJavascript
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
    
protected:
	
   virtual ~ModuleGUIJavascript();

}; 

#endif //_MODULEGUIJAVASCRIPT_H_
