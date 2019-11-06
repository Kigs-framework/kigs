#include "ModuleSoundManagerJS.h"

#include "AudioBufferJS.h"


ModuleSoundManagerJS* gInstanceModuleSoundManagerJS = 0;

IMPLEMENT_CLASS_INFO(ModuleSoundManagerJS)

//! constructor
ModuleSoundManagerJS::ModuleSoundManagerJS(const kstl::string& name, CLASS_NAME_TREE_ARG) : ModuleBase(name, PASS_CLASS_NAME_TREE_ARG)
{
}


//! destructor
ModuleSoundManagerJS::~ModuleSoundManagerJS()
{
}    

//! module init, register FilePathManager
void ModuleSoundManagerJS::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"ModuleSoundManagerJS",params);
	DECLARE_FULL_CLASS_INFO(core, AudioBufferJS, AudioBuffer, SoundManager)
	/*DECLARE_FULL_CLASS_INFO(core, HTTPAsyncRequestJS, HTTPAsyncRequest, ModuleSoundManagerJS)
	DECLARE_FULL_CLASS_INFO(core, HTTPRequestJS, HTTPRequest, ModuleSoundManagerJS)
	DECLARE_FULL_CLASS_INFO(core, HTTPConnectJS, HTTPConnect, ModuleSoundManagerJS)*/
}

//! module close
void ModuleSoundManagerJS::Close()
{
	BaseClose();	
}    

//! module update     
void ModuleSoundManagerJS::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);
}    

ModuleBase* MODULEINITFUNC(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(ModuleSoundManagerJS, "ModuleSoundManagerJS");
	gInstanceModuleSoundManagerJS = new ModuleSoundManagerJS("HTTPRequestModuleJS");
	gInstanceModuleSoundManagerJS->Init(core, params);
	return gInstanceModuleSoundManagerJS;
}    