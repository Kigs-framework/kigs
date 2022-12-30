#include "PrecompiledHeaders.h"
#include "Core.h"
#include "ModuleGUI.h"
#include "ModuleGUIWindows.h"
#include "WindowWin32.h"
#include "DisplayDeviceCapsWin32.h"

using namespace Kigs::Gui;

IMPLEMENT_CLASS_INFO(ModuleGUIWindows)

ModuleGUIWindows::ModuleGUIWindows(const std::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
}

ModuleGUIWindows::~ModuleGUIWindows()
{

}    

void ModuleGUIWindows::Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
	BaseInit(core,"GUIWindows",params);
	//! declare WindowWin32 to be the current implementation of Window
	DECLARE_FULL_CLASS_INFO(core,WindowWin32,Window,GUI)

	DECLARE_FULL_CLASS_INFO(core,DisplayDeviceCapsWin32,DisplayDeviceCaps,GUI)
}

void ModuleGUIWindows::Close()
{
	BaseClose();
}

void ModuleGUIWindows::Update(const Timer& timer, void* addParam)
{
}

SP<ModuleBase> Kigs::Gui::PlatformGUIModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(ModuleGUIWindows, "ModuleGUIWindows");
	auto gInstanceModuleGUIWindows = MakeRefCounted<ModuleGUIWindows>("ModuleGUIWindows");
	gInstanceModuleGUIWindows->Init(core,params);
	return gInstanceModuleGUIWindows;
}