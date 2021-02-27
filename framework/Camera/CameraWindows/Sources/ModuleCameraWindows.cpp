#include "PrecompiledHeaders.h"
#include "ModuleCameraWindows.h"
#include "CameraWindows.h"
#include "ShrinkerWindows.h"
//#include "BodyTracking.h"
//#include "DrawBody.h"

ModuleCameraWindows* gInstanceModuleCameraWindows=0;

IMPLEMENT_CLASS_INFO(ModuleCameraWindows)

//! constructor
ModuleCameraWindows::ModuleCameraWindows(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	
}


//! destructor
ModuleCameraWindows::~ModuleCameraWindows()
{
}    

//! module init, register FilePathManager
void ModuleCameraWindows::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"ModuleCameraWindows",params);
	 
	DECLARE_FULL_CLASS_INFO(core, CameraWindows,WebCamera,CameraModule)    
	DECLARE_FULL_CLASS_INFO(core, ShrinkerWindows, Shrinker, CameraModule)  

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (FAILED(hr))
	{
		// error
		return;
	}

//	DECLARE_FULL_CLASS_INFO(core, BodyTracking, BodyTracking, CameraModule)
//	DECLARE_FULL_CLASS_INFO(core, DrawBody, DrawBody, CameraModule)
}

//! module close
void ModuleCameraWindows::Close()
{

    BaseClose();
}    

//! module update     
void ModuleCameraWindows::Update(const Timer& timer, void* addParam)
{

	BaseUpdate(timer,addParam);
}    


ModuleBase* PlatformCameraModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
 
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(ModuleCameraWindows,"ModuleCameraWindows");
	gInstanceModuleCameraWindows=new ModuleCameraWindows("ModuleCameraWindows");
	gInstanceModuleCameraWindows->Init(core,params);
	return gInstanceModuleCameraWindows;
}    
