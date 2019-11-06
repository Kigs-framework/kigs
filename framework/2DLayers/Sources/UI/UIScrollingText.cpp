#include "UI/UIScrollingText.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "AlphaMask.h"
#include "LocalizationManager.h"
#include "Timer.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIScrollingText, UIScrollingText, 2DLayers);
IMPLEMENT_CLASS_INFO(UIScrollingText)

UIScrollingText::UIScrollingText(const kstl::string& name,CLASS_NAME_TREE_ARG) : 
UIText(name, PASS_CLASS_NAME_TREE_ARG)
,myCaracterPerSeconde(*this,false,LABEL_AND_ID(CaracterPerSeconde),KFLOAT_ZERO)
,myScrollIndex(1)
,mydElapsedTime(0)
{
	myLength = myScrollIndex;
}


void UIScrollingText::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == myText.getLabelID())
	{
		myScrollIndex = 1;
	}
	UIText::NotifyUpdate(labelid);
}

void	UIScrollingText::Update(const Timer& a_timer, void* addParam)
{
	mydElapsedTime += ((Timer&)a_timer).GetDt(this);
	if(mydElapsedTime >= myCaracterPerSeconde)
	{
		mydElapsedTime = 0;
		myScrollIndex++;
		myLength = myScrollIndex;
		ChangeText(myText.const_ref());
	}
}
