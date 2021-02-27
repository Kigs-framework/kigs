#ifndef _MODULECAMERAANDROID_H_
#define _MODULECAMERAANDROID_H_

#include "ModuleBase.h"
#include "Core.h"

class ModuleCameraAndroid;
extern ModuleCameraAndroid* gInstanceModuleCameraAndroid;


#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformCameraModuleInit
extern ModuleBase* PlatformCameraModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif


// ****************************************
// * ModuleCameraAndroid class
// * --------------------------------------
/*!  \class ModuleCameraAndroid
     this class is the module manager class. 
*/
// ****************************************

class ModuleCameraAndroid : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleCameraAndroid,ModuleBase,ModuleCameraAndroid)

	//! module constructor 
	ModuleCameraAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	     
	//! module init
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;

	//! module close
	void Close() override;
	             
	//! module update
	void Update(const Timer& timer,void* /*addParam*/) override;

protected:
	//! destructor
    virtual ~ModuleCameraAndroid();    
    
}; 

#endif //_MODULECAMERAANDROID_H_
