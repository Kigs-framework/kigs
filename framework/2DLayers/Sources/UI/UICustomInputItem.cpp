#include "UI/UICustomInputItem.h"

#include "ModuleInput.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UICustomInputItem, UICustomInputItem, 2DLayers);
IMPLEMENT_CLASS_INFO(UICustomInputItem)

void UICustomInputItem::InitModifiable()
{
	ParentClassType::InitModifiable();

	ModuleInput* theInputModule = (ModuleInput*)KigsCore::GetModule("ModuleInput");

	if (!HasMethod("ManageDirectTouchEvent") && !HasMethod("ManageClickTouchEvent"))
	{
		UninitModifiable();
		return;
	}

	if (theInputModule)
	{
		if(HasMethod("ManageDirectTouchEvent"))
			static_cast<TouchEventStateDirectTouch*>(theInputModule->getTouchManager()->registerEvent(this, "ManageDirectTouchEvent", DirectTouch, EmptyFlag))->setAutoTouchDownDistance(mAutoTouchDistance);
		if(HasMethod("ManageClickTouchEvent"))
			static_cast<TouchEventStateClick*>(theInputModule->getTouchManager()->registerEvent(this, "ManageClickTouchEvent", Click, EmptyFlag))->setAutoClickDistance(mAutoTouchDistance);
	}
}


