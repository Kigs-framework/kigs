#include "HTTPRequestModuleJS.h"
#include "HTTPAsyncRequestJS.h"
#include "HTTPConnectJS.h"



HTTPRequestModuleJS* gInstanceModuleHTTPRequestJS = 0;

IMPLEMENT_CLASS_INFO(HTTPRequestModuleJS)

//! constructor
HTTPRequestModuleJS::HTTPRequestModuleJS(const kstl::string& name, CLASS_NAME_TREE_ARG) : ModuleBase(name, PASS_CLASS_NAME_TREE_ARG)
{
	
}


//! destructor
HTTPRequestModuleJS::~HTTPRequestModuleJS()
{
}    

//! module init, register FilePathManager
void HTTPRequestModuleJS::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"HTTPRequestModuleJS",params);

	DECLARE_FULL_CLASS_INFO(core, HTTPAsyncRequestJS, HTTPAsyncRequest, HTTPRequestModule)
	DECLARE_FULL_CLASS_INFO(core, HTTPConnectJS, HTTPConnect, HTTPRequestModule)
}

//! module close
void HTTPRequestModuleJS::Close()
{
	BaseClose();	
}    

//! module update     
void HTTPRequestModuleJS::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);
}    

SP<ModuleBase>  MODULEINITFUNC(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
 	DECLARE_CLASS_INFO_WITHOUT_FACTORY(HTTPRequestModuleJS,"HTTPRequestModuleJS");
	auto ptr = MakeRefCounted<HTTPRequestModuleJS>("HTTPRequestModuleJS");

	gInstanceModuleHTTPRequestJS = ptr.get();
	gInstanceModuleHTTPRequestJS->Init(core, params);
	return ptr;
}    