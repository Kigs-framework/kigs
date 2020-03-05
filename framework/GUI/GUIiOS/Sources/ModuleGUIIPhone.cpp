#include "PrecompiledHeaders.h"
#include "Core.h"
#include "ModuleGUIIPhone.h"
#include "WindowIPhone.h"
#include "DisplayDeviceCapsIPhone.h"


IMPLEMENT_CLASS_INFO(ModuleGUIIPhone)

ModuleGUIIPhone::ModuleGUIIPhone(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{

}
    
ModuleGUIIPhone::~ModuleGUIIPhone()
{
   
}    

void ModuleGUIIPhone::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"GUIIPhone",params);
	//! declare WindowIPhone to be the current implementation of Window
    DECLARE_FULL_CLASS_INFO(core,DisplayDeviceCapsIPhone,DisplayDeviceCaps,GUI)
	DECLARE_FULL_CLASS_INFO(core,WindowIPhone,Window,GUI)
	
	
}       

void ModuleGUIIPhone::Close()
{
    BaseClose();
}    

void ModuleGUIIPhone::Update(const Timer& timer, void* addParam)
{

}    

ModuleBase* MODULEINITFUNC(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
 
   ModuleBase*     gInstanceModuleGUIIPhone=new ModuleGUIIPhone("ModuleGUIIPhone");
   gInstanceModuleGUIIPhone->Init(core,params);
     
	return gInstanceModuleGUIIPhone;
}    

