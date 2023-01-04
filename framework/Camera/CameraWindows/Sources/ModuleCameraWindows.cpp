#include "PrecompiledHeaders.h"
#include "ModuleCameraWindows.h"
#include "CameraWindows.h"
#include "ShrinkerWindows.h"
//#include "BodyTracking.h"
//#include "DrawBody.h"

using namespace Kigs::Camera;

ModuleCameraWindows* Kigs::Camera::gInstanceModuleCameraWindows=0;

IMPLEMENT_CLASS_INFO(ModuleCameraWindows)

//! constructor
ModuleCameraWindows::ModuleCameraWindows(const std::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	
}


//! destructor
ModuleCameraWindows::~ModuleCameraWindows()
{
}    

//! module init, register FilePathManager
void ModuleCameraWindows::Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
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
void ModuleCameraWindows::Update(const Time::Timer& timer, void* addParam)
{

	BaseUpdate(timer,addParam);
}    


SP<ModuleBase> Kigs::Camera::PlatformCameraModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
 
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(ModuleCameraWindows,"ModuleCameraWindows");
	auto ptr=MakeRefCounted<ModuleCameraWindows>("ModuleCameraWindows");
	gInstanceModuleCameraWindows= ptr.get();
	gInstanceModuleCameraWindows->Init(core,params);
	return ptr;
}    
