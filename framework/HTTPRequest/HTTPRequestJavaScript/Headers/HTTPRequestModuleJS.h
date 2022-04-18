#ifndef _HTTPREQUESTMODULEJS_H_
#define _HTTPREQUESTMODULEJS_H_

#include "ModuleBase.h"
#include "Core.h"

class HTTPRequestModuleJS;
extern HTTPRequestModuleJS* gInstanceModuleHTTPRequestJS;


#ifdef _KIGS_ONLY_STATIC_LIB_
#define MODULEINITFUNC			PlatformHTTPRequestModuleInit
extern SP<ModuleBase> PlatformHTTPRequestModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#else
#define MODULEINITFUNC			ModuleInit
#endif

// ****************************************
// * HTTPRequestModuleJS class
// * --------------------------------------
/*!  \class HTTPRequestModuleJS
    Windows HTTP request manager 
*/
// ****************************************

class HTTPRequestModuleJS : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(HTTPRequestModuleJS, ModuleBase, HTTPRequestModule)

	//! module constructor 
	HTTPRequestModuleJS(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 	//! destructor
    virtual ~HTTPRequestModuleJS();    
    
	//! module init
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);  

	//! module close
	void Close();         
	             
	//! module update
	virtual void Update(const Timer& timer, void* addParam);      

protected:

}; 

#endif //_HTTPREQUESTMODULEJS_H_
