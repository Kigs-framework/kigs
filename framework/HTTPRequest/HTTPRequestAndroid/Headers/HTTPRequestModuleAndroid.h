#ifndef _HTTPREQUESTMODULEANDROID_H_
#define _HTTPREQUESTMODULEANDROID_H_

#include "ModuleBase.h"
#include "Core.h"

class HTTPRequestModuleAndroid;
extern HTTPRequestModuleAndroid* gInstanceModuleHTTPRequestAndroid;


#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformHTTPRequestModuleInit
extern ModuleBase* PlatformHTTPRequestModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

// ****************************************
// * HTTPRequestModuleAndroid class
// * --------------------------------------
/*!  \class HTTPRequestModuleAndroid
    Windows HTTP request manager 
*/
// ****************************************

class HTTPRequestModuleAndroid : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(HTTPRequestModuleAndroid, ModuleBase, HTTPRequestModule)

	//! module constructor 
	HTTPRequestModuleAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	     
	//! module init
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;

	//! module close
	void Close() override;

	//! module update
	void Update(const Timer& timer,void* /*addParam*/) override;


protected:
	//! destructor
    virtual ~HTTPRequestModuleAndroid();    
}; 

#endif //_HTTPREQUESTMODULEANDROID_H_
