#include "HTTPRequestModuleAndroid.h"
#include "HTTPAsyncRequestAndroid.h"
#include "HTTPConnectAndroid.h"


HTTPRequestModuleAndroid* gInstanceModuleHTTPRequestAndroid=0;

IMPLEMENT_CLASS_INFO(HTTPRequestModuleAndroid)

//! constructor
HTTPRequestModuleAndroid::HTTPRequestModuleAndroid(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{	
}

//! destructor
HTTPRequestModuleAndroid::~HTTPRequestModuleAndroid()
{
}    

//! module init, register FilePathManager
void HTTPRequestModuleAndroid::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"HTTPRequestModuleAndroid",params);

	DECLARE_FULL_CLASS_INFO(core, HTTPAsyncRequestAndroid, HTTPAsyncRequest, HTTPRequestModule)
	DECLARE_FULL_CLASS_INFO(core, HTTPConnectAndroid, HTTPConnect, HTTPRequestModule)
	
	// manage jni method ID
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	KigsJavaIDManager::RegisterClass(g_env, "com/assoria/httprequest/AsyncHTTPRequest");	
	
	//KigsJavaIDManager::GetStaticMethod(g_env, "com/assoria/httprequest/AsyncHTTPRequest", "Query", "(Ljava/lang/String;)V");	
	//KigsJavaIDManager::GetStaticMethod(g_env, "com/assoria/httprequest/AsyncHTTPRequest", "GetSize",  "()Ljava/lang/String;");
	//KigsJavaIDManager::GetStaticMethod(g_env, "com/assoria/httprequest/AsyncHTTPRequest", "GetContentEncoding", "()Ljava/lang/String;");	
	//KigsJavaIDManager::GetStaticMethod(g_env, "com/assoria/httprequest/AsyncHTTPRequest", "GetCharsetEncoding", "()Ljava/lang/String;");	
	//KigsJavaIDManager::GetStaticMethod(g_env, "com/assoria/httprequest/AsyncHTTPRequest", "GetResult", "()Ljava/lang/String;");	
}

//! module close
void HTTPRequestModuleAndroid::Close()
{
	BaseClose();
}    

//! module update     
void HTTPRequestModuleAndroid::Update(const Timer& timer,void* addParam)
{
	BaseUpdate(timer, addParam);
}    

ModuleBase* MODULEINITFUNC(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
 
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(HTTPRequestModuleAndroid,"HTTPRequestModuleAndroid");
	gInstanceModuleHTTPRequestAndroid = new HTTPRequestModuleAndroid("HTTPRequestModuleAndroid");
	gInstanceModuleHTTPRequestAndroid->Init(core, params);
	return gInstanceModuleHTTPRequestAndroid;
}    