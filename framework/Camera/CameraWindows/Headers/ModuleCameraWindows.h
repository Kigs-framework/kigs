#ifndef _MODULECAMERAWINDOWS_H_
#define _MODULECAMERAWINDOWS_H_

#include "ModuleBase.h"
#include "Core.h"
#include "CameraModule.h"

class ModuleCameraWindows;
extern ModuleCameraWindows* gInstanceModuleCameraWindows;

// ****************************************
// * ModuleCameraWindows class
// * --------------------------------------
/*!  \class ModuleCameraWindows
     this class is the module manager class. 
*/
// ****************************************

class ModuleCameraWindows : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleCameraWindows,ModuleBase,ModuleCameraWindows)

	//! module constructor 
	ModuleCameraWindows(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	     
	//! module init
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);

	//! module close
	void Close();         
	             
	//! module update
	virtual void Update(const Timer& timer, void* addParam);

protected:
	//! destructor
    virtual ~ModuleCameraWindows();    
}; 

#endif //_MODULECAMERAWINDOWS_H_
