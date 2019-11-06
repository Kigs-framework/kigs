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
	, myRenderingScreen(*this, true, LABEL_AND_ID(RenderingScreen))
	, myManager(*this, true, LABEL_AND_ID(SequenceManager), "DataDrivenSequenceManager:AppSequenceManager") // default is app
	, mySizeX(*this, false, LABEL_AND_ID(SizeX), -1)
	, mySizeY(*this, false, LABEL_AND_ID(SizeY), -1)
	, myIsInteractive(*this, false, LABEL_AND_ID(IsInteractive), true)
{
}

void Abstract2DLayer::InitModifiable()
{
	Scene3D::InitModifiable();
	if (IsInit())
	{
		// uninit CoreModifiable if the RenderingScreen ref is not valid
		if (((CoreModifiable*)myRenderingScreen) == 0)
		{
			UninitModifiable();
			return;
		}

		// Use the RenderingScreen size if one of the size is -1
		if (mySizeX.const_ref() == -1 || mySizeY.const_ref() == -1)
		{
			float sX, sY;
			GetRenderingScreen()->GetDesignSize(sX, sY);

			mySizeX = (unsigned int)sX;
			mySizeY = (unsigned int)sY;
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
