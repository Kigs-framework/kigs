#include "ModuleCameraWUP.h"
#include "CameraWUP.h"
#include "ShrinkerWUP.h"

ModuleCameraWUP* gInstanceModuleCameraWUP=0;

IMPLEMENT_CLASS_INFO(ModuleCameraWUP)

//! constructor
ModuleCameraWUP::ModuleCameraWUP(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	
}


//! destructor
ModuleCameraWUP::~ModuleCameraWUP()
{
}    

//! module init, register FilePathManager
void ModuleCameraWUP::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	BaseInit(core,"ModuleCameraWUP",params);
	 
	DECLARE_FULL_CLASS_INFO(core, CameraWUP,WebCamera,CameraModule)    
	DECLARE_FULL_CLASS_INFO(core, ShrinkerWUP, Shrinker, CameraModule)  
	
//	DECLARE_FULL_CLASS_INFO(core, BodyTracking, BodyTracking, CameraModule)
//	DECLARE_FULL_CLASS_INFO(core, DrawBody, DrawBody, CameraModule)
}

//! module close
void ModuleCameraWUP::Close()
{
	BaseClose();
}    

//! module update     
void ModuleCameraWUP::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);
}    


ModuleBase* MODULEINITFUNC(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
 
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(ModuleCameraWUP,"ModuleCameraWUP");
	gInstanceModuleCameraWUP=new ModuleCameraWUP("ModuleCameraWUP");
	gInstanceModuleCameraWUP->Init(core,params);
	return gInstanceModuleCameraWUP;
}    
