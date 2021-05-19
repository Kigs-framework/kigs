#include "PrecompiledHeaders.h"
#include "Base2DLayer.h"
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

IMPLEMENT_CLASS_INFO(Abstract2DLayer);

///////////////////////////////////////////
//
//			Abstract2DLayer
//
///////////////////////////////////////////
Abstract2DLayer::Abstract2DLayer(const kstl::string& name, CLASS_NAME_TREE_ARG)
	: Scene3D(name, PASS_CLASS_NAME_TREE_ARG)
	, mRenderingScreen(*this, true, LABEL_AND_ID(RenderingScreen))
	, mManager(*this, true, LABEL_AND_ID(SequenceManager), "DataDrivenSequenceManager:AppSequenceManager") // default is app
	, mSize(*this, false, LABEL_AND_ID(Size), -1,-1)
	, mIsInteractive(*this, false, LABEL_AND_ID(IsInteractive), true)
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
bool Abstract2DLayer::StartDrawing(TravState* state)
{
	KIGS_ASSERT(GetRenderingScreen());
	return GetRenderingScreen()->SetActive(state);

}

//! close rendering screen
void Abstract2DLayer::EndDrawing(TravState* state)
{
	KIGS_ASSERT(GetRenderingScreen());
	GetRenderingScreen()->Release(state);
}
///////////////////////////////////////////
