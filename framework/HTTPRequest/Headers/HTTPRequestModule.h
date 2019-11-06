#ifndef _HTTPREQUESTMODULE_H_
#define _HTTPREQUESTMODULE_H_

#include "ModuleBase.h"
#include "Core.h"


// ****************************************
// * HTTPRequestModule class
// * --------------------------------------
/*!  \class HTTPRequestModule
    base class for generic SQLite access. 
*/
// ****************************************

class HTTPRequestModule : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(HTTPRequestModule,ModuleBase,HTTPRequestModule)

	//! module constructor 
	HTTPRequestModule(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	     
	//! module init
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;

	//! module close
	void Close() override;
	             
	//! module update
	void Update(const Timer& timer, void* addParam) override;

protected:
	//! destructor
    virtual ~HTTPRequestModule();    
}; 

#endif //_HTTPREQUESTMODULE_H_
