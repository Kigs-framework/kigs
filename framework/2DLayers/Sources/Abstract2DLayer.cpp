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
	, mSizeX(*this, false, LABEL_AND_ID(SizeX), -1)
	, mSizeY(*this, false, LABEL_AND_ID(SizeY), -1)
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
		if (mSizeX.const_ref() == -1 || mSizeY.const_ref() == -1)
		{
			float sX, sY;
			GetRenderingScreen()->GetDesignSize(sX, sY);

			mSizeX = (unsigned int)sX;
			mSizeY = (unsigned int)sY;
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
