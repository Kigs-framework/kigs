#include "PrecompiledHeaders.h"
#include "Window.h"
#include "Timer.h"

#include "ModuleGUI.h"
#include "Core.h"

using namespace Kigs::Gui;

IMPLEMENT_CLASS_INFO(ModuleGUI)

ModuleGUI::ModuleGUI(const std::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
}

ModuleGUI::~ModuleGUI()
{
}    


void ModuleGUI::Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
	BaseInit(core,"GUI",params);

	core->RegisterMainModuleList(this,GUIModuleCoreIndex);

	RegisterPlatformSpecific(PlatformGUIModuleInit(core,params));
}

void ModuleGUI::Close()
{

	BaseClose();
}    

//! update all windows
void ModuleGUI::Update(const Time::Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);

	std::vector<ModifiableItemStruct>::const_iterator it;

	for (it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).mItem->isSubType(Window::mClassID))
		{
			it->mItem->CallUpdate(timer, addParam);
		}
	}            
}    
