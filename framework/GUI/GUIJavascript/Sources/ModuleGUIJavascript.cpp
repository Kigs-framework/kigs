#include "Core.h"
#include "ModuleGUIJavascript.h"
#include "WindowJavascript.h"
#include "DisplayDeviceCapsJavascript.h"


IMPLEMENT_CLASS_INFO(ModuleGUIJavascript);

ModuleGUIJavascript::ModuleGUIJavascript(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
}

ModuleGUIJavascript::~ModuleGUIJavascript()
{
}

void ModuleGUIJavascript::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	BaseInit(core,"GUIJavascript",params);

	//! declare WindowJavascript to be the current implementation of Window
	DECLARE_FULL_CLASS_INFO(core,WindowJavascript,Window,GUI)

	DECLARE_FULL_CLASS_INFO(core,DisplayDeviceCapsJavascript,DisplayDeviceCaps,GUI)
}

void ModuleGUIJavascript::Close()
{
	BaseClose();
}

void ModuleGUIJavascript::Update(const Timer& timer, void* addParam)
{
}

SP<ModuleBase> MODULEINITFUNC(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(ModuleGUIJavascript, "ModuleGUIJavascript");
	auto ptr = MakeRefCounted<ModuleGUIJavascript>("ModuleGUIJavascript");
	ptr->Init(core,params);
	return ptr;
}
