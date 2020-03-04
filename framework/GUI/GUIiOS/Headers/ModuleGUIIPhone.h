// Doxygen group defines

 /*! \defgroup GUIIPhoneModule  GUI IPhone module 
  *  \ingroup GUIModule
  *  GUI Module Classes for IPhone
  */


#ifndef _MODULEGUIIPHONE_H_
#define _MODULEGUIIPHONE_H_

#include "ModuleBase.h"

class ModuleGUIIPhone;


#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformGUIModuleInit
extern ModuleBase* PlatformGUIModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

// ****************************************
// * ModuleGUIIPhone class
// * --------------------------------------
/*!  \class ModuleGUIIPhone
     GUI module using IPhone API
     \ingroup GUIIPhoneModule
*/
// ****************************************

class ModuleGUIIPhone : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleGUIIPhone,ModuleBase,GUI)
             
	ModuleGUIIPhone(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params); 
    void Close();
    virtual void Update(const Timer& timer, void* addParam);
    
protected:
	
   virtual ~ModuleGUIIPhone();

}; 

#endif //_MODULEGUIIPHONE_H_
