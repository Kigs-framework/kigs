#include "PrecompiledHeaders.h"
#include "CollisionModule.h"
#include "CollisionManager.h"
#include "BSphere.h"
#include "BCylinder.h"
#include "BoxCollider.h"
#include "Plane.h"
#include "Timer.h"

using namespace Kigs::Collide;

IMPLEMENT_CLASS_INFO(CollisionModule);

//! constructor
CollisionModule::CollisionModule(const std::string& name, CLASS_NAME_TREE_ARG) : ModuleBase(name, PASS_CLASS_NAME_TREE_ARG)
{
}


//! destructor
CollisionModule::~CollisionModule()
{
}

//! module init, register CollisionManager and BSphere objects
void CollisionModule::Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
	BaseInit(core, "Collision", params);

	DECLARE_FULL_CLASS_INFO(core, CollisionManager, CollisionManager, Collision)
	DECLARE_FULL_CLASS_INFO(core, BSphere, BSphere, Collision)
	DECLARE_FULL_CLASS_INFO(core, BCylinder, BCylinder, Collision)
	DECLARE_FULL_CLASS_INFO(core, Plane, Plane, Collision)
	DECLARE_FULL_CLASS_INFO(core, Panel, Panel, Collision)
	DECLARE_FULL_CLASS_INFO(core, BoxCollider, BoxCollider, Collision)
		
}

//! module close
void CollisionModule::Close()
{
	BaseClose();
}

//! module update
void CollisionModule::Update(const Time::Timer& timer, void* addParam)
{
	BaseUpdate(timer, addParam);
}
