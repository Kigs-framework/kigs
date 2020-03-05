#include "ModuleThreadIOS.h"
#include "Core.h"

#include "ThreadRunMethodIOS.h"
#include "SemaphoreIOS.h"


IMPLEMENT_CLASS_INFO(ModuleThreadIOS)

ModuleThreadIOS::ModuleThreadIOS(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
}
    
ModuleThreadIOS::~ModuleThreadIOS()
{
    
}    


void ModuleThreadIOS::Init(Core* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"ModuleThreadIOS",params);
    DECLARE_FULL_CLASS_INFO(core,ThreadRunMethodIOS,ThreadRunMethod,Thread)
	DECLARE_FULL_CLASS_INFO(core,SemaphoreIOS,Semaphore,Thread)
}       

void ModuleThreadIOS::Close()
{
    BaseClose();
}    

void ModuleThreadIOS::Update(const Timer& timer, void* addParam)
{

}    

ModuleBase* MODULEINITFUNC(Core* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	Core::ModuleStaticInit(core);
 
	ModuleBase* gInstanceThreadIOS = new ModuleThreadIOS("ModuleThreadIOS");
	gInstanceThreadIOS->Init(core, params);
	return gInstanceThreadIOS; 
}    
