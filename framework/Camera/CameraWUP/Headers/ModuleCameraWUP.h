#ifndef _ModuleCameraWUP_H_
#define _ModuleCameraWUP_H_

#include "ModuleBase.h"
#include "Core.h"

class ModuleCameraWUP;
extern ModuleCameraWUP* gInstanceModuleCameraWUP;


#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformCameraModuleInit
extern ModuleBase* PlatformCameraModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif


// ****************************************
// * ModuleCameraWUP class
// * --------------------------------------
/**
* \file	ModuleCameraWUP.h
* \class	ModuleCameraWUP
* \ingroup Camera
* \ingroup Module
* \brief   Specific UWP camera module.
*/
// ****************************************

class ModuleCameraWUP : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(ModuleCameraWUP,ModuleBase,ModuleCameraWUP)

	//! module constructor 
	ModuleCameraWUP(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
		 
	//! module init
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);

	//! module close
	void Close();         
				 
	//! module update
	virtual void Update(const Timer& timer, void* addParam);

protected:
	//! destructor
	virtual ~ModuleCameraWUP();    
}; 

#endif //_ModuleCameraWUP_H_
