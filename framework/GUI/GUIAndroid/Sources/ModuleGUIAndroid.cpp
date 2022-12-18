#include "PrecompiledHeaders.h"
#include "Core.h"
#include "ModuleGUIAndroid.h"
#include "WindowAndroid.h"
#include "DisplayDeviceCapsAndroid.h"


IMPLEMENT_CLASS_INFO(ModuleGUIAndroid)

ModuleGUIAndroid::ModuleGUIAndroid(const std::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{

}
    
ModuleGUIAndroid::~ModuleGUIAndroid()
{
   
}    

void ModuleGUIAndroid::Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"GUIIAndroid",params);
	//! declare WindowAndroid to be the current implementation of Window
    DECLARE_FULL_CLASS_INFO(core,WindowAndroid,Window,GUI)
	DECLARE_FULL_CLASS_INFO(core,DisplayDeviceCapsAndroid,DisplayDeviceCaps,GUI)
	
	
}       

void ModuleGUIAndroid::Close()
{
    BaseClose();
}    

void ModuleGUIAndroid::Update(const Timer& timer,void* /*addParam*/)
{

}    

ModuleBase* PlatformGUIModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);

	DECLARE_CLASS_INFO_WITHOUT_FACTORY(ModuleGUIAndroid, "ModuleGUIAndroid");
    ModuleBase* gInstanceModuleGUIAndroid=new ModuleGUIAndroid("ModuleGUIAndroid");
    gInstanceModuleGUIAndroid->Init(core,params);
    
	return     gInstanceModuleGUIAndroid;
}    
