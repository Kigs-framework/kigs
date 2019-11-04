#include "ModuleDDSequence.h"
#include "BaseDDSequence.h"
#include "BumperDDSequence.h"
#include "GenericDDSequence.h"
#include "Scene3DSequence.h"

IMPLEMENT_CLASS_INFO(ModuleDDSequence);


ModuleDDSequence::ModuleDDSequence(const kstl::string& name, CLASS_NAME_TREE_ARG)
	: ModuleBase(name, PASS_CLASS_NAME_TREE_ARG)
{
}


void ModuleDDSequence::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	BaseInit(core, "ModuleDDSequence", params); 

	// Init system independant classes
	DECLARE_FULL_CLASS_INFO(core, BaseDDSequence, BaseDDSequence, CoreDataDrivenSequence)
	DECLARE_FULL_CLASS_INFO(core, Scene3DSequence, Scene3DSequence, CoreDataDrivenSequence)
	DECLARE_FULL_CLASS_INFO(core, BumperDDSequence, BumperDDSequence, CoreDataDrivenSequence)
	DECLARE_FULL_CLASS_INFO(core, GenericDDSequence, GenericDDSequence, CoreDataDrivenSequence)

}

void ModuleDDSequence::Update(const Timer& timer, void* addParam)
{
}

void ModuleDDSequence::Close()
{
	BaseClose();
}