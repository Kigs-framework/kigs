#include "PrecompiledHeaders.h"
#include "CameraModule.h"

#include "FixedImageBufferStream.h"

IMPLEMENT_CLASS_INFO(CameraModule)

//! constructor
CameraModule::CameraModule(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	
}


//! destructor
CameraModule::~CameraModule()
{
}    

//! module init, register FilePathManager
void CameraModule::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"Camera",params);
	RegisterDynamic(PlatformCameraModuleInit(core,params));
	DECLARE_FULL_CLASS_INFO(core, FixedImageBufferStream, FixedImageBufferStream, CameraModule)
}

//! module close
void CameraModule::Close()
{
	BaseClose();
}    

//! module update     
void CameraModule::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);
}    
