#include "PrecompiledHeaders.h"
#include "XMLModule.h"

IMPLEMENT_CLASS_INFO(XMLModule)

XMLModule::XMLModule(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
}

XMLModule::~XMLModule()
{
}    


void XMLModule::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"XML",params);
}

void XMLModule::Close()
{
    BaseClose();
}    

void XMLModule::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);
}    
