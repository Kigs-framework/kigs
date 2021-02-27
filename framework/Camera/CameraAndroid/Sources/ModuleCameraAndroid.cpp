#include "PrecompiledHeaders.h"
#include "ModuleCameraAndroid.h"
#include "CameraAndroid.h"


ModuleCameraAndroid* gInstanceModuleCameraAndroid=0;


IMPLEMENT_CLASS_INFO(ModuleCameraAndroid)

//! constructor
ModuleCameraAndroid::ModuleCameraAndroid(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	
}


//! destructor
ModuleCameraAndroid::~ModuleCameraAndroid()
{
}    

//! module init, register FilePathManager
void ModuleCameraAndroid::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"ModuleCameraAndroid",params);
	 	 
	DECLARE_FULL_CLASS_INFO(core,CameraAndroid,WebCamera,CameraModule)   
}

//! module close
void ModuleCameraAndroid::Close()
{
    BaseClose();
}    

//! module update     
void ModuleCameraAndroid::Update(const Timer& timer,void* addParam)
{
	BaseUpdate(timer, addParam);
}    


ModuleBase* MODULEINITFUNC(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
	
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(ModuleCameraAndroid, "ModuleCameraAndroid");
	ModuleBase* gInstanceModuleCameraAndroid=new ModuleCameraAndroid("ModuleCameraAndroid");
    gInstanceModuleCameraAndroid->Init(core,params);
    return     gInstanceModuleCameraAndroid;
}    
