#include "PrecompiledHeaders.h"
#include "ModuleThreadWin32.h"
#include "Core.h"
#include "ThreadRunMethodWin32.h"
#include "SemaphoreWin32.h"
#include "ThreadEventWin32.h"
#include "ThreadLocalStorageManagerWin32.h"

ModuleThreadWin32* gInstanceThreadWin32=0;

IMPLEMENT_CLASS_INFO(ModuleThreadWin32)

ModuleThreadWin32::ModuleThreadWin32(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
}
    
ModuleThreadWin32::~ModuleThreadWin32()
{
    
}    


void ModuleThreadWin32::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"ModuleThreadWin32",params);
    DECLARE_FULL_CLASS_INFO(core,ThreadRunMethodWin32,ThreadRunMethod,Thread)
	DECLARE_FULL_CLASS_INFO(core,SemaphoreWin32,Semaphore,Thread)
	DECLARE_FULL_CLASS_INFO(core, ThreadEventWin32, ThreadEvent, Thread)
	DECLARE_FULL_CLASS_INFO(core, ThreadLocalStorageManagerWin32, ThreadLocalStorageManager, Thread)
}       

void ModuleThreadWin32::Close()
{
    BaseClose();
}    

void ModuleThreadWin32::Update(const Timer& timer, void* addParam)
{

}    

ModuleBase*  MODULEINITFUNC(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
 
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(ModuleThreadWin32,"ModuleThreadWin32");
    ModuleBase* gInstanceThreadWin32=new ModuleThreadWin32("ModuleThreadWin32");
    gInstanceThreadWin32->Init(core,params);
    return gInstanceThreadWin32;    
}    
