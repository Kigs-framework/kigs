#ifndef _CAMERAMODULE_H_
#define _CAMERAMODULE_H_

#include "ModuleBase.h"
#include "Core.h"

/*! \defgroup Camera 
*  Webcam / video management
*/

ModuleBase* PlatformCameraModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);

// ****************************************
// * CameraModule class
// * --------------------------------------
/**
* \file	CameraModule.h
* \class	CameraModule
* \ingroup Camera
* \ingroup Module
* \brief Generic Camera management module

*/
// ****************************************

class CameraModule : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(CameraModule,ModuleBase,CameraModule)

	//! module constructor 
	CameraModule(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	     
	//! module init
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;  

	//! module close
	void Close() override;
	             
	//! module update
	void Update(const Timer& timer, void* addParam) override;

protected:
	//! destructor
    virtual ~CameraModule();    
}; 

#endif //_CAMERAMODULE_H_
