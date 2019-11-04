#include "PrecompiledHeaders.h"
#include "HTTPRequestModuleWUP.h"
#include "HTTPAsyncRequestWUP.h"
#include "HTTPConnectWUP.h"

//#define USE_DEBUG_PRINT
#ifdef USE_DEBUG_PRINT
#define DEBUG_PRINT(...) fprintf(stderr,__VA_ARGS__)
#else
#define DEBUG_PRINT(...) 
#endif



HTTPRequestModuleWUP* gInstanceModuleHTTPRequestWUP = nullptr;

IMPLEMENT_CLASS_INFO(HTTPRequestModuleWUP)


//! destructor
HTTPRequestModuleWUP::~HTTPRequestModuleWUP()
{
}    

void HTTPRequestModuleWUP::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"HTTPRequestModuleWUP",params);

	DECLARE_FULL_CLASS_INFO(core, HTTPAsyncRequestWUP, HTTPAsyncRequest, HTTPRequestModule)
	DECLARE_FULL_CLASS_INFO(core, HTTPConnectWUP, HTTPConnect, HTTPRequestModule)
}

//! module close
void HTTPRequestModuleWUP::Close()
{
	BaseClose();
}    

//! module update     
void HTTPRequestModuleWUP::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);
}    

ModuleBase* PlatformHTTPRequestModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);

	DECLARE_CLASS_INFO_WITHOUT_FACTORY(HTTPRequestModuleWUP, "HTTPRequestModuleWindows");
	gInstanceModuleHTTPRequestWUP = new HTTPRequestModuleWUP("HTTPRequestModuleWindows");
	gInstanceModuleHTTPRequestWUP->Init(core, params);
	return gInstanceModuleHTTPRequestWUP;
}    
