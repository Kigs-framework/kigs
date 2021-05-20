#include "PrecompiledHeaders.h"
#include "Timer.h"
#include "ModuleSceneGraph.h"
#include "Module2DLayers.h"
#include "CoreBaseApplication.h"
#include "Core.h"
#include "LocalizationManager.h"

// registered class includes
#include "AlphaMask.h"
#include "MouseVelocityComputer.h"
#include "Node2D.h"

#include "BaseUI2DLayer.h"

// registered UI class includes
#include "UI/PassiveColoredMask.h"
#include "UI/UI3DLinkedItem.h"
//#include "UI/UIAnimatedButton.h"
//#include "UI/UIAnimatedSprite.h"
#include "UI/UIBoxLayout.h"
#include "UI/UIButton.h"
#include "UI/UIButtonImage.h"
#include "UI/UIButtonText.h"
//#include "UI/UIControlBoxForScrollViewUI.h"
#include "UI/UICursor.h"
#include "UI/UICustomDraw.h"
#include "UI/UICustomInputItem.h"
#include "UI/UIDrawableItem.h"
#include "UI/UIDynamicText.h"
#include "UI/UIFlowLayout.h"
#include "UI/UIGridLayout.h"
#include "UI/UIGroupButton.h"
#include "UI/UIImage.h"
#include "UI/UIImageHighlight.h"
#include "UI/UIItem.h"
#include "UI/UILayout.h"
//#include "UI/UIPanel.h"
#include "UI/UIPopUp.h"
#include "UI/UIRatioKeeper.h"
#include "UI/UIRenderingScreen.h"
#include "UI/UIRoundHUD.h"
#include "UI/UIScrollable.h"
#include "UI/UIScrollingMultilineText.h"
#include "UI/UIScrollingText.h"
#include "UI/UIScrollView.h"
#include "UI/UISlider.h"
#include "UI/UISliderFill.h"
#include "UI/UISlidersGroup.h"
//#include "UI/UISprite.h"
#include "UI/UIStream.h"
#include "UI/UIText.h"
#include "UI/UITextArea.h"
#include "UI/UITextInput.h"
//#include "UI/UITexture.h"
#include "UI/UITexturedItem.h"
//#include "UI/UIVideo.h"
#include "UI/UINode3DLayer.h"
#include "UI/UIShapeDisc.h"
#include "UI/UIShapePolygon.h"

IMPLEMENT_CLASS_INFO(Module2DLayers)


bool	Module2DLayers::mRotate180 = false;

Module2DLayers::Module2DLayers(const kstl::string& name, CLASS_NAME_TREE_ARG) : ModuleBase(name, PASS_CLASS_NAME_TREE_ARG)
{}

Module2DLayers::~Module2DLayers()
{}

void Module2DLayers::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	BaseInit(core, "2DLayers", params);

	REGISTER_UPGRADOR(PopUpUpgrador);
	
	DECLARE_FULL_CLASS_INFO(core, AlphaMask, AlphaMask, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, MouseVelocityComputer, MouseVelocityComputer, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, Node2D, Node2D, 2DLayers);

	// UI
	DECLARE_FULL_CLASS_INFO(core, BaseUI2DLayer, UI2DLayer, 2DLayers);

	DECLARE_FULL_CLASS_INFO(core, UI3DLinkedItem, UI3DLinkedItem, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, PassiveColoredMask, PassiveColoredMask, 2DLayers);
	//DECLARE_FULL_CLASS_INFO(core, UIAnimatedButton, UIAnimatedButton, 2DLayers);
	//DECLARE_FULL_CLASS_INFO(core, UIAnimatedSprite, UIAnimatedSprite, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIBoxLayout, UIBoxLayout, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIButton, UIButton, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIButtonImage, UIButtonImage, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIButtonText, UIButtonText, 2DLayers);
	//DECLARE_FULL_CLASS_INFO(core, UIControlBoxForScrollViewUI, UIControlBoxForScrollViewUI, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UICursor, UICursor, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UICustomDraw, UICustomDraw, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UICustomInputItem, UICustomInputItem, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIDrawableItem, UIDrawableItem, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIDynamicText, UIDynamicText, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, FontMapManager, FontMapManager, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIFlowLayout, UIFlowLayout, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIGridLayout, UIGridLayout, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIGroupButton, UIGroupButton, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIImage, UIImage, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIImageHighlight, UIImageHighlight, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIItem, UIItem, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UILayout, UILayout, 2DLayers);
	//DECLARE_FULL_CLASS_INFO(core, UIPanel, UIPanel, 2DLayers);
	// a UIPanel is just a UIDrawableItem item
	DECLARE_CLASS_ALIAS(core, UIPanel, UIDrawableItem);
	//DECLARE_FULL_CLASS_INFO(core, UIPopUp, UIPopUp, 2DLayers);
	// a UIPopUp is a UIItem with PopUpUpgrador upgrade
	DECLARE_CLASS_ALIAS_AND_UPGRADE(core, UIPopUp, UIItem, PopUpUpgrador);
	DECLARE_FULL_CLASS_INFO(core, UIRatioKeeper, UIRatioKeeper, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIRenderingScreen, UIRenderingScreen, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIRoundHUD, UIRoundHUD, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIScrollable, UIScrollable, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIScrollingMultilineText, UIScrollingMultilineText, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIScrollingText, UIScrollingText, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIScrollView, UIScrollView, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UISlider, UISlider, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UISliderFill, UISliderFill, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UISlidersGroup, UISlidersGroup, 2DLayers);
	//DECLARE_FULL_CLASS_INFO(core, UISprite, UISprite, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIStream, UIStream, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIText, UIText, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UITextArea, UITextArea, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UITextInput, UITextInput, 2DLayers);
	//DECLARE_FULL_CLASS_INFO(core, UITexture, UITexture, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UITexturedItem, UITexturedItem, 2DLayers);
	//DECLARE_FULL_CLASS_INFO(core, UIVideo, UIVideo, 2DLayers);

	DECLARE_FULL_CLASS_INFO(core, UINode3DLayer, UINode3DLayer, 2DLayers);

	DECLARE_FULL_CLASS_INFO(core, UIShapeDisc, UIShapeDisc, 2DLayers);
	DECLARE_FULL_CLASS_INFO(core, UIShapePolygon, UIShapePolygon, 2DLayers);

	core->RegisterMainModuleList(this, Layer2DModuleCoreIndex);
	
}

void Module2DLayers::Close()
{
	BaseClose();
}

void Module2DLayers::Update(const Timer& timer, void* addParam)
{	
	BaseUpdate(timer,addParam);
}

kstl::set<CoreModifiable*>	Module2DLayers::GetLayerList()
{
	SP<ModuleSceneGraph> scenegraph = CoreGetModule(ModuleSceneGraph);
	kstl::set<CoreModifiable*> layerlist;
	layerlist.clear();

	const kstl::vector<ModifiableItemStruct>& items=scenegraph->getItems();
	kstl::vector<ModifiableItemStruct>::const_iterator	it = items.begin();
	kstl::vector<ModifiableItemStruct>::const_iterator	itend = items.end();

	for (; it != itend; ++it)
	{
		if ((*it).mItem->isSubType("Abstract2DLayer"))
		{
			layerlist.insert((CoreModifiable*)(*it).mItem.get());
		}
	}

	return layerlist;
}
