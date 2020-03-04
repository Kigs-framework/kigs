#ifndef _HTTPREQUESTMODULEWINDOWS_H_
#define _HTTPREQUESTMODULEWINDOWS_H_

#include "ModuleBase.h"
#include "Core.h"

class HTTPRequestModuleIOS;


#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformHTTPRequestModuleInit
extern ModuleBase* PlatformHTTPRequestModuleInit(Core* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

// ****************************************
// * HTTPRequestModuleIOS class
// * --------------------------------------
/*!  \class HTTPRequestModuleIOS
    IOS HTTP request manager
*/
// ****************************************

class HTTPRequestModuleIOS : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(HTTPRequestModuleIOS, ModuleBase, HTTPRequestModule)

	//! module constructor 
	HTTPRequestModuleIOS(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	     
	//! module init
	void Init(Core* core, const kstl::vector<CoreModifiableAttribute*>* params);  

	//! module close
	void Close();         
	             
	//! module update
	virtual void Update(const Timer& timer, void* addParam);

protected:
	//! destructor
    virtual ~HTTPRequestModuleIOS();
}; 

#endif //_HTTPREQUESTMODULEIOS_H_
