#include "PrecompiledHeaders.h"

#include "ModuleBase.h"
#include "Core.h"
#include "XMLIncludes.h"
#include "ModuleFileManager.h"
#include "Timer.h"

using namespace Kigs::Core;
using namespace Kigs::Xml;

//! implement static members for ModuleBase
IMPLEMENT_CLASS_INFO(ModuleBase);

void    ModuleBase::RegisterPlatformSpecific(SP<ModuleBase> platform)
{
	mPlatformModuleList.push_back(platform);
}


//! init for a module : find the XML description file, and try to load and init all associated dll
void    ModuleBase::BaseInit(KigsCore* core,const std::string& moduleName, const std::vector<CoreModifiableAttribute*>* params)
{
	KIGS_MESSAGE("Init Base Module : "+moduleName);
	mCore = core;
	KigsCore::ModuleInit(core,this);
	RegisterToCore();

}

//! call all dll "ModuleClose" method and free all dll loaded
void    ModuleBase::BaseClose()
{
	EmptyItemList();
	for (auto m : mPlatformModuleList)
	{
		m->Close();
	}
	mPlatformModuleList.clear();
}

//! call specific module update
void    ModuleBase::BaseUpdate(const Time::Timer& timer, void* addParam)
{
	for (auto m : mPlatformModuleList)
	{
		m->CallUpdate(timer, addParam);
	}
}
