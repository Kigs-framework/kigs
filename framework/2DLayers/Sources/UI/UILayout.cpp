#include "UI/UILayout.h"
#include "Base2DLayer.h"
#include "RenderingScreen.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UILayout, UILayout, 2DLayers);
IMPLEMENT_CLASS_INFO(UILayout)

UILayout::UILayout(const kstl::string& name, CLASS_NAME_TREE_ARG) :
	UIDrawableItem(name, PASS_CLASS_NAME_TREE_ARG)
	, myNeedRecompute(true)
{

}


bool UILayout::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	//myNeedRecompute = true;
	bool ret = UIDrawableItem::addItem(item, pos PASS_LINK_NAME(linkName));
	NeedRecomputeLayout();
	return ret;
}

bool UILayout::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	//myNeedRecompute = true;
	bool ret = UIDrawableItem::removeItem(item PASS_LINK_NAME(linkName));
	NeedRecomputeLayout();
	return ret;
}

void UILayout::NotifyUpdate(const unsigned int labelid)
{
	UIDrawableItem::NotifyUpdate(labelid);
}

void UILayout::Update(const Timer& timer, void*)
{
	if (myNeedRecompute)
	{
		RecomputeLayout();
		myNeedRecompute = false;
	}
}

void UILayout::ProtectedDraw(TravState* state)
{
	if (myNeedRecompute)
	{
		RecomputeLayout();
		myNeedRecompute = false;
	}
	ParentClassType::ProtectedDraw(state);
}
