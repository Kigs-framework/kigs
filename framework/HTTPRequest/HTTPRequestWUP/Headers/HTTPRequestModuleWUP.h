#pragma once

#include "ModuleBase.h"

class KigsCore;
class HTTPRequestModuleWUP;


extern HTTPRequestModuleWUP* gInstanceModuleHTTPRequestWUP;
extern SP<ModuleBase> PlatformHTTPRequestModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);

// ****************************************
// * HTTPRequestModuleWUP class
// * --------------------------------------
/**
* \file	HTTPRequestModuleWUP.h
* \class	HTTPRequestModuleWUP
* \ingroup HTTPRequest
* \ingroup Module
* \brief UWP specific HTTP Request module
*
*/
// ****************************************

class HTTPRequestModuleWUP : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(HTTPRequestModuleWUP, ModuleBase, HTTPRequestModule)
	DECLARE_INLINE_CONSTRUCTOR(HTTPRequestModuleWUP){}
	virtual ~HTTPRequestModuleWUP();

	//! module init
	void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);

	//! module close
	void Close();         
	             
	//! module update
	virtual void Update(const Timer& timer, void* addParam);
}; 


