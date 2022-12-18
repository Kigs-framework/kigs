
#ifndef _MODULEGUIANDROID_H_
#define _MODULEGUIANDROID_H_

#include "ModuleBase.h"

class ModuleGUIAndroid;

ModuleBase* PlatformGUIModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);


// ****************************************
// * ModuleGUIAndroid class
// * --------------------------------------
/**
* \file	ModuleGUIAndroid.h
* \class	ModuleGUIAndroid
* \ingroup GUIModule
* \ingroup Module
* \brief Specific module for GUI classes ( Windows management )
*
*/
// ****************************************
class ModuleGUIAndroid : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleGUIAndroid,ModuleBase,GUI)
             
	ModuleGUIAndroid(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
    void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;
    void Close() override;
    void Update(const Timer& timer,void* /*addParam*/) override;
    
protected:
	
   virtual ~ModuleGUIAndroid();

}; 

#endif //_MODULEGUIANDROID_H_
