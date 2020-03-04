// Doxygen group defines

 /*! \defgroup GUIAndroidModule  GUI IPhone module 
  *  \ingroup GUIModule
  *  GUI Module Classes for IPhone
  */


#ifndef _MODULEGUIANDROID_H_
#define _MODULEGUIANDROID_H_

#include "ModuleBase.h"

class ModuleGUIAndroid;

ModuleBase* PlatformGUIModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);

// ****************************************
// * ModuleGUIAndroid class
// * --------------------------------------
/*!  \class ModuleGUIAndroid
     GUI module using Android API
     \ingroup GUIAndroidModule
*/
// ****************************************

class ModuleGUIAndroid : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleGUIAndroid,ModuleBase,GUI)
             
	ModuleGUIAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
    void Close() override;
    void Update(const Timer& timer,void* /*addParam*/) override;
    
protected:
	
   virtual ~ModuleGUIAndroid();

}; 

#endif //_MODULEGUIANDROID_H_
