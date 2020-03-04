#include "ModuleThreadAndroid.h"
#include "Core.h"
#include "ThreadRunMethodAndroid.h"
#include "SemaphoreAndroid.h"
#include "ThreadEventAndroid.h"
#include "ThreadLocalStorageManagerAndroid.h"


IMPLEMENT_CLASS_INFO(ModuleThreadAndroid)

ModuleThreadAndroid::ModuleThreadAndroid(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
}
    
ModuleThreadAndroid::~ModuleThreadAndroid()
{
    
}    


void ModuleThreadAndroid::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"ModuleThreadAndroid",params);
    DECLARE_FULL_CLASS_INFO(core,ThreadRunMethodAndroid,ThreadRunMethod,Thread)
	DECLARE_FULL_CLASS_INFO(core,SemaphoreAndroid,Semaphore,Thread)
	DECLARE_FULL_CLASS_INFO(core,ThreadEventAndroid,ThreadEvent,Thread)
	DECLARE_FULL_CLASS_INFO(core,ThreadLocalStorageManagerAndroid,ThreadLocalStorageManager,Thread)
}       

void ModuleThreadAndroid::Close()
{
    BaseClose();
}    

void ModuleThreadAndroid::Update(const Timer& timer,void* /*addParam*/)
{

}    

ModuleBase*  MODULEINITFUNC(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KigsCore::ModuleStaticInit(core);
 
	DECLARE_CLASS_INFO_WITHOUT_FACTORY(ModuleThreadAndroid, "ModuleThreadAndroid");
    ModuleBase* gInstanceThreadAndroid=new ModuleThreadAndroid("ModuleThreadAndroid");
    gInstanceThreadAndroid->Init(core,params);
    return gInstanceThreadAndroid;    
}    
