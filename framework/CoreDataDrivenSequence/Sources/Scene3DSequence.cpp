#include "Scene3DSequence.h"
#include "ModuleBase.h"

IMPLEMENT_CLASS_INFO(Scene3DSequence)

Scene3DSequence::Scene3DSequence(const kstl::string& name, CLASS_NAME_TREE_ARG)
	: BaseDDSequence(name, PASS_CLASS_NAME_TREE_ARG)
{
}

Scene3DSequence::~Scene3DSequence()
{
}

void	Scene3DSequence::InitModifiable()
{
	BaseDDSequence::InitModifiable();

/*	kstl::set<CoreModifiable*>	instances;
	GetSonInstancesByType("Scene3D", instances);


	ModuleBase* scenegraph = Core::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);

	kstl::set<CoreModifiable*>::iterator	it = instances.begin();
	kstl::set<CoreModifiable*>::iterator	itend = instances.end();

	for (; it != itend; ++it)
	{
		scenegraph->addItem(*it);
	}*/
}

void    Scene3DSequence::ProtectedDestroy()
{

/*
	kstl::set<CoreModifiable*>	instances;
	GetSonInstancesByType("Scene3D", instances);

	ModuleBase* scenegraph = Core::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);

	kstl::set<CoreModifiable*>::iterator	it = instances.begin();
	kstl::set<CoreModifiable*>::iterator	itend = instances.end();

	for (; it != itend; ++it)
	{
		scenegraph->removeItem(*it);
	}*/

	BaseDDSequence::ProtectedDestroy();
}