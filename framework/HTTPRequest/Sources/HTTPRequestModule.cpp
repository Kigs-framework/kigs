#include "PrecompiledHeaders.h"
#include "HTTPRequestModule.h"
#include "ResourceDownloader.h"

SP<ModuleBase> PlatformHTTPRequestModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);

IMPLEMENT_CLASS_INFO(HTTPRequestModule)

//! constructor
HTTPRequestModule::HTTPRequestModule(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	
}


//! destructor
HTTPRequestModule::~HTTPRequestModule()
{
}    

//! module init, register FilePathManager
void HTTPRequestModule::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"HTTPRequestModule",params);
	DECLARE_FULL_CLASS_INFO(core, ResourceDownloader, ResourceDownloader, HTTPRequestModule);

	RegisterDynamic(PlatformHTTPRequestModuleInit(core,params)); 	    
}

//! module close
void HTTPRequestModule::Close()
{
	BaseClose();
}    

//! module update     
void HTTPRequestModule::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);
}    
