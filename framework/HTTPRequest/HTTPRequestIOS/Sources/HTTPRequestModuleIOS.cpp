#include "HTTPRequestModuleIOS.h"
#include "HTTPAsyncRequestIOS.h"
#include "HTTPConnectIOS.h"

//#define USE_DEBUG_PRINT
#ifdef USE_DEBUG_PRINT
#define DEBUG_PRINT(...) fprintf(stderr,__VA_ARGS__)
#else
#define DEBUG_PRINT(...) 
#endif


IMPLEMENT_CLASS_INFO(HTTPRequestModuleIOS)

//! constructor
HTTPRequestModuleIOS::HTTPRequestModuleIOS(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	
}


//! destructor
HTTPRequestModuleIOS::~HTTPRequestModuleIOS()
{
}    

//! module init, register FilePathManager
void HTTPRequestModuleIOS::Init(Core* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"HTTPRequestModuleIOS",params);

	DECLARE_FULL_CLASS_INFO(core, HTTPAsyncRequestIOS, HTTPAsyncRequest, HTTPRequestModule)
	DECLARE_FULL_CLASS_INFO(core, HTTPConnectIOS, HTTPConnect, HTTPRequestModule)
}

//! module close
void HTTPRequestModuleIOS::Close()
{
	BaseClose();
}    

//! module update     
void HTTPRequestModuleIOS::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer, addParam);
}    

ModuleBase* MODULEINITFUNC(Core* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	Core::ModuleStaticInit(core);
 
	ModuleBase* gInstanceModuleHTTPRequestIOS = new HTTPRequestModuleIOS("HTTPRequestModuleIOS");
	gInstanceModuleHTTPRequestIOS->Init(core, params);
	return gInstanceModuleHTTPRequestIOS;
}    

