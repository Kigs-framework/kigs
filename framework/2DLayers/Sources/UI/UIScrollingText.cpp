#include "UI/UIScrollingText.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "AlphaMask.h"
#include "LocalizationManager.h"
#include "Timer.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIScrollingText, UIScrollingText, 2DLayers);
IMPLEMENT_CLASS_INFO(UIScrollingText)

UIScrollingText::UIScrollingText(const std::string& name,CLASS_NAME_TREE_ARG) : 
UIText(name, PASS_CLASS_NAME_TREE_ARG)
,mCaracterPerSeconde(*this,false,"CaracterPerSeconde",0.0f)
,mScrollIndex(1)
,mElapsedTime(0)
{
	mLength = mScrollIndex;
}


void UIScrollingText::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == mText.getLabelID())
	{
		mScrollIndex = 1;
	}
	UIText::NotifyUpdate(labelid);
}

void	UIScrollingText::Update(const Timer& a_timer, void* addParam)
{
	mElapsedTime += ((Timer&)a_timer).GetDt(this);
	if(mElapsedTime >= mCaracterPerSeconde)
	{
		mElapsedTime = 0;
		mScrollIndex++;
		mLength = mScrollIndex;
		ChangeText(mText.const_ref());
	}
}
