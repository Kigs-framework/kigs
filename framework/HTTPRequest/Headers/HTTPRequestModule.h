#ifndef _HTTPREQUESTMODULE_H_
#define _HTTPREQUESTMODULE_H_

#include "ModuleBase.h"
#include "Core.h"


/*! \defgroup HTTPRequest HTTPRequest module
*/

// ****************************************
// * HTTPRequestModule class
// * --------------------------------------
/**
* \file	HTTPRequestModule.h
* \class	HTTPRequestModule
* \ingroup HTTPRequest
* \ingroup Module
* \brief Generic Module for HTTP Request classes
*
*/
// ****************************************
class HTTPRequestModule : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(HTTPRequestModule,ModuleBase,HTTPRequestModule)

	//! module constructor 
	HTTPRequestModule(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~HTTPRequestModule();

	//! module init
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;

	//! module close
	void Close() override;
	             
	//! module update
	void Update(const Timer& timer, void* addParam) override;
}; 

#endif //_HTTPREQUESTMODULE_H_
