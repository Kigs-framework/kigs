#include "PrecompiledHeaders.h"
#include "ModuleThreadWUP.h"
#include "Core.h"
#include "ThreadRunMethodWUP.h"
#include "SemaphoreWUP.h"
#include "ThreadEventWUP.h"
#include "ThreadLocalStorageManagerWUP.h"

ModuleThreadWUP* gInstanceThreadWUP=0;

IMPLEMENT_CLASS_INFO(ModuleThreadWUP)

ModuleThreadWUP::ModuleThreadWUP(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
}
    
ModuleThreadWUP::~ModuleThreadWUP()
{
    
}    


void ModuleThreadWUP::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"ModuleThreadWUP",params);
    DECLARE_FULL_CLASS_INFO(core,ThreadRunMethodWUP,ThreadRunMethod,Thread)
	DECLARE_FULL_CLASS_INFO(core,SemaphoreWUP,Semaphore,Thread)
	DECLARE_FULL_CLASS_INFO(core, ThreadEventWUP, ThreadEvent, Thread)
	DECLARE_FULL_CLASS_INFO(core, ThreadLocalStorageManagerWUP, ThreadLocalStorageManager, Thread)
}       

void ModuleThreadWUP::Close()
{
    BaseClose();
}    

void ModuleThreadWUP::Update(const Timer& timer, void* addParam)
{

}    

ModuleBase*  MODULEINITFUNC(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
 
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(ModuleThreadWUP,"ModuleThreadWUP");
    ModuleBase* gInstanceThreadWUP=new ModuleThreadWUP("ModuleThreadWUP");
    gInstanceThreadWUP->Init(core,params);
    return gInstanceThreadWUP;    
}    
