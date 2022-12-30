#include "PrecompiledHeaders.h"

#include "TravState.h"
#include "SceneGraphIncludes.h"
#include "Core.h"

#include "ModernMesh.h"
#include "ModuleRenderer.h"

using namespace Kigs::Scene;

//IMPLEMENT_AND_REGISTER_CLASS_INFO(TravState, TravState, SceneGraph);
IMPLEMENT_CLASS_INFO(TravState)

TravState::TravState(const std::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, mPath(0)
, mCurrentMaterial(0)
, mLights(0)
, mCamera(0)
, mRenderer(0)
, mSceneGraph(0)
, mCurrentScene(0)
//, pDrawableSorter(0)
, mManageFrontToBackStruct(0)
{
	mCullingObject = KigsCore::GetInstanceOf("myCullingObject", "CullingObject");
	mVisibilityFrame = 0;
	mFrame = 0;
	mCurrentScale = 1.0f;
}

TravState::~TravState()
{
	
}

void TravState::DrawInstances()
{
	GetRenderer()->DrawPendingInstances(this);
}