#include "UI/UILayout.h"
#include "RenderingScreen.h"

using namespace Kigs::Draw2D;

IMPLEMENT_CLASS_INFO(UILayout)

UILayout::UILayout(const std::string& name, CLASS_NAME_TREE_ARG) :
	UIDrawableItem(name, PASS_CLASS_NAME_TREE_ARG)
	, mNeedRecompute(true)
{

}


bool UILayout::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	bool ret = UIDrawableItem::addItem(item, pos PASS_LINK_NAME(linkName));
	NeedRecomputeLayout();
	return ret;
}

bool UILayout::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	bool ret = UIDrawableItem::removeItem(item PASS_LINK_NAME(linkName));
	NeedRecomputeLayout();
	return ret;
}

void UILayout::NotifyUpdate(const unsigned int labelid)
{
	UIDrawableItem::NotifyUpdate(labelid);
}

void UILayout::Update(const Time::Timer& timer, void*)
{
	if (mNeedRecompute)
	{
		RecomputeLayout();
		mNeedRecompute = false;
	}
}

void UILayout::ProtectedDraw(Scene::TravState* state)
{
	if (mNeedRecompute)
	{
		RecomputeLayout();
		mNeedRecompute = false;
	}
	ParentClassType::ProtectedDraw(state);
}
