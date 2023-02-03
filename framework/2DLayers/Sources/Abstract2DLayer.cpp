#include "PrecompiledHeaders.h"
#include "Abstract2DLayer.h"
#include "RenderingScreen.h"
#include "ModuleFileManager.h"
#include "TextureFileManager.h"
#include "NotificationCenter.h"
#include "Texture.h"
#include "UI/UIItem.h"
#include "InputIncludes.h"
#include "MultiTouchDevice.h"
#include "CoreBaseApplication.h"
#include "ModuleSceneGraph.h"
#include "Core.h"
#include "ModuleRenderer.h"

#include "TimeProfiler.h"

using namespace Kigs::Draw2D;
using namespace Kigs::Scene;

IMPLEMENT_CLASS_INFO(Abstract2DLayer);

///////////////////////////////////////////
//
//			Abstract2DLayer
//
///////////////////////////////////////////
Abstract2DLayer::Abstract2DLayer(const std::string& name, CLASS_NAME_TREE_ARG)
	: Scene3D(name, PASS_CLASS_NAME_TREE_ARG)
	, mRenderingScreen(*this, "RenderingScreen")
	, mManager(*this, "SequenceManager", "DataDrivenSequenceManager:AppSequenceManager") // default is app
{
}

void Abstract2DLayer::InitModifiable()
{
	Scene3D::InitModifiable();
	if (IsInit())
	{
		// uninit CoreModifiable if the RenderingScreen ref is not valid
		if (((CoreModifiable*)mRenderingScreen) == 0)
		{
			UninitModifiable();
			return;
		}

		// Use the RenderingScreen size if one of the size is -1
		if (mSize[0] <= -1.0f || mSize[1] <= -1.0f)
		{
			float sX, sY;
			GetRenderingScreen()->GetDesignSize(sX, sY);

			mSize = v2f(sX,sY);
		}
		
		//ModuleSpecificRenderer* renderer = ((ModuleRenderer*)KigsCore::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
	}
}

//! set rendering screen active
bool Abstract2DLayer::StartDrawing(Scene::TravState* state)
{
	KIGS_ASSERT(GetRenderingScreen());
	return GetRenderingScreen()->SetActive(state);

}

//! close rendering screen
void Abstract2DLayer::EndDrawing(Scene::TravState* state)
{
	KIGS_ASSERT(GetRenderingScreen());
	GetRenderingScreen()->Release(state);
}
///////////////////////////////////////////
