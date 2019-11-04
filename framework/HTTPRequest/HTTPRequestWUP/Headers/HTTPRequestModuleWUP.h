#pragma once

#include "ModuleBase.h"

class KigsCore;
class HTTPRequestModuleWUP;


extern HTTPRequestModuleWUP* gInstanceModuleHTTPRequestWUP;
extern ModuleBase* PlatformHTTPRequestModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);

class HTTPRequestModuleWUP : public ModuleBase
{
public:

	DECLARE_CLASS_INFO(HTTPRequestModuleWUP, ModuleBase, HTTPRequestModule)
	DECLARE_INLINE_CONSTRUCTOR(HTTPRequestModuleWUP){}
	     
	//! module init
	void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);

	//! module close
	void Close();         
	             
	//! module update
	virtual void Update(const Timer& timer, void* addParam);


protected:
    virtual ~HTTPRequestModuleWUP();    
}; 


