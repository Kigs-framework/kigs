#include "PrecompiledHeaders.h"

#include "TravState.h"
#include "SceneGraphIncludes.h"
#include "Core.h"

#include "ModernMesh.h"
#include "ModuleRenderer.h"


//IMPLEMENT_AND_REGISTER_CLASS_INFO(TravState, TravState, SceneGraph);
IMPLEMENT_CLASS_INFO(TravState)

TravState::TravState(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, myPath(0)
, myCurrentMaterial(0)
, myLights(0)
, myCamera(0)
, myRenderer(0)
, mySceneGraph(0)
, myCurrentScene(0)
//, pDrawableSorter(0)
, pManageFrontToBackStruct(0)
{
	myCullingObject = (CullingObject*)(KigsCore::GetInstanceOf("myCullingObject", "CullingObject"));
	myVisibilityFrame = 0;
	myFrame = 0;
	myCurrentScale = 1.0f;
}

TravState::~TravState()
{
	myCullingObject->Destroy();
}

void TravState::DrawInstances()
{
	GetRenderer()->DrawPendingInstances(this);
}