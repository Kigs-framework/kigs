#include "UI/UICustomInputItem.h"

#include "ModuleInput.h"

using namespace Kigs::Draw2D;
using namespace Kigs::Input;
using namespace Kigs::Core;

IMPLEMENT_CLASS_INFO(UICustomInputItem)

void UICustomInputItem::InitModifiable()
{
	ParentClassType::InitModifiable();

	auto theInputModule = KigsCore::GetModule<ModuleInput>();

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


