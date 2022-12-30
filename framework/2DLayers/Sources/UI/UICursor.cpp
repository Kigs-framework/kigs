#include "UI/UICursor.h"
#include "ModuleInput.h"

using namespace Kigs::Draw2D;
using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(UICursor)

IMPLEMENT_CONSTRUCTOR(UICursor)
{
	auto theInputModule = KigsCore::GetModule<ModuleInput>();
	theInputModule->getTouchManager()->registerEvent(this, "ManageDirectTouchEvent", DirectTouch, IgnoreSwallow);
}

bool UICursor::ManageDirectTouchEvent(DirectTouchEvent& direct_touch)
{
	UIItem *owner = (UIItem*)(CoreModifiable*)mUIOwner;
	bool allow = (owner != nullptr) ? owner->ContainsPoint(direct_touch.position.x, direct_touch.position.y) : false;
	mIsHidden = !allow;

	if (direct_touch.state == StatePossible) // check for hover
	{
		return allow;
	}

	if (direct_touch.state == StateBegan)
	{
		switch (direct_touch.touch_state)
		{
		case DirectTouchEvent::TouchHover:
			break;
		case DirectTouchEvent::TouchDown:
			break;
		case DirectTouchEvent::TouchUp:
			break;
		}
	}
	else if (direct_touch.state == StateChanged)
	{

		if (direct_touch.touch_state == DirectTouchEvent::TouchHover)
		{
			Set_Position(direct_touch.position.x, direct_touch.position.y);
		}
		else
		{
		}
	}
	else if (direct_touch.state == StateEnded)
	{
		if (direct_touch.touch_state == DirectTouchEvent::TouchHover)
		{
		}
		else
		{
		}
	}
	return false;
}
