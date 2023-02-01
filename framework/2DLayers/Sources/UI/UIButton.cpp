#include "UI/UIButton.h"

#include "TextureFileManager.h"
#include "Texture.h"
#include "NotificationCenter.h"
#include "AlphaMask.h"
#include "UI/UIGroupButton.h"

#include "AttributePacking.h"
#include "ModuleInput.h"
#include "TouchInputEventManager.h"

#include "TecLibs/Math/Algorithm.h"

using namespace Kigs::Draw2D;
using namespace Kigs::Core;
using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(UIButton)

IMPLEMENT_CONSTRUCTOR(UIButton)
	, mClickDownAction(*this, "ClickDownAction", "")
	, mClickUpAction(*this, "ClickUpAction", "")
	, mMouseOverAction(*this, "MouseOverAction", "")
	, mMouseOutAction(*this, "MouseOutAction", "")
	, mUnSelectAction(*this, "UnSelectAction", "")
	, mParameter(*this, "Parameter", (std::string)"")
	, mStayPressed(*this, "StayPressed", false)
	, mKeepClickOutside(*this, "KeepClickOutside", false)
	, mDefaultPressed(*this, "DefaultPressed", false)
	, mAutoResize(*this, "AutoResize", true)
	, mInside(false)
	, mIsDown(false)
	, mIsMouseOver(false)
{
}

void UIButton::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == KigsID("IsEnabled")._id )
	{
		// if disabled "reset" button state
		if (mIsEnabled == false)
		{
			mIsDown = false;
			mIsMouseOver = false;
		}
	}
	else if (labelid == KigsID("Size")._id )
	{
		if (mParent && mParent->isSubType("UIGroupButton"))
			((UIGroupButton*)mParent)->reComputeSize();
	}

	UITexturedItem::NotifyUpdate(labelid);
}


void UIButton::InitModifiable()
{
	UITexturedItem::InitModifiable();
	if (IsInit())
	{
		mIsDown = false;
		mIsMouseOver = false;

		//if (mUseHoverColor) setValue("Color", (v3f)mIdleColor), setValue("Opacity", mIdleColor[3]);

		if (mDefaultPressed && mStayPressed)
			mIsDown = true;

		setOwnerNotification("IsEnabled", true);
		setOwnerNotification("Size", true);

		auto theInputModule = KigsCore::GetModule<Input::ModuleInput>();
		// retreive click for activation
		// theInputModule->getTouchManager()->registerEvent(this, "ManageClickTouchEvent", Click, emptyFlag);
		// retreive direct touch for visual hover / push ...
		if (theInputModule)
		{
			static_cast<Input::TouchEventStateDirectTouch*>(theInputModule->getTouchManager()->registerEvent(this, "ManageDirectTouchEvent", Input::DirectTouch, Input::EmptyFlag))->setAutoTouchDownDistance(0.05f);
			static_cast<Input::TouchEventStateClick*>(theInputModule->getTouchManager()->registerEvent(this, "ManageClickTouchEvent", Input::Click, Input::EmptyFlag))->setAutoClickDistance(0.05f);
		}
	}
}

void UIButton::AutoSize()
{
	// auto size button
	if (mTexturePointer)
	{
		v2f autosize;
		mTexturePointer->GetSize(autosize.x,autosize.y);
		
		if (!((int)mSize[0] == 0 && mSizeModeX == SizeMode::DEFAULT))
		{
			autosize.x = mSize[0];
		}

		if (!((int)mSize[1] == 0 && mSizeModeY == SizeMode::DEFAULT))
		{
			autosize.y = mSize[1];
		}

		if((v2f)mSize != autosize)
			setValue("Size", autosize);

		if (mParent && mParent->isSubType("UIGroupButton"))
			((UIGroupButton*)mParent)->reComputeSize();
	}
}

static void SendButtonNotifications(const std::string& actions, CoreModifiable* sender, std::vector<CoreModifiableAttribute*>& params)
{
	std::vector<std::string>	L_ActionVect = SplitStringByCharacter(actions, ':');
	for (unsigned int i = 0; i < L_ActionVect.size(); i++)
		KigsCore::GetNotificationCenter()->postNotificationName(L_ActionVect[i], params, sender);
}

bool UIButton::isAlpha(float X, float Y)
{
	//Try to get mask
	if (!mAlphaMask)
	{
		std::vector<ModifiableItemStruct> sons = getItems();
		for (unsigned int i = 0; i < sons.size(); i++)
		{
			if (sons[i].mItem->isSubType("AlphaMask"))
			{
				mAlphaMask = sons[i].mItem;
				break;
			}
		}
	}

	if (mAlphaMask)
	{
		//Check on mask the specified location
		return !mAlphaMask->CheckTo(X, Y);
	}

	return false;
}


bool UIButton::ManageClickTouchEvent(Input::ClickEvent& click_event)
{
	bool allow = CanInteract(click_event.position.xy);
	
	if (allow)
	{
		v2f pts[4];
		GetTransformedPoints(pts);
		v2f position_in_button;
		position_in_button.x = ProjectOnLineScalar(click_event.position.xy, pts[0], pts[3]);
		position_in_button.y = ProjectOnLineScalar(click_event.position.xy, pts[0], pts[1]);
		allow = allow && !isAlpha(position_in_button.x, position_in_button.y);
	}

	if (click_event.state == StatePossible || click_event.state == StateChanged)
	{
		if (allow)
		{
			//kigsprintf("test\n");
			//*click_event.swallow_mask = 0xFFFFFFFF;
		}
		return allow;
	}
	else if (click_event.state == StateRecognized)
	{
		if (!EmitSignal(Signals::ClickUp, this, (usString)mParameter.const_ref())) // if no one connected send classic message
		{
			std::vector<CoreModifiableAttribute*> sendParams;
			sendParams.push_back(&mParameter);
			SendButtonNotifications(mClickUpAction, this, sendParams);
		}
		*click_event.swallow_mask |= (1 << InputEventType::Click);
	}
	return true;
}

bool UIButton::ManageDirectTouchEvent(DirectTouchEvent& direct_touch)
{
	bool allow = CanInteract(direct_touch.position.xy);

	if (allow)
	{
		v2f pts[4];
		GetTransformedPoints(pts);
		v2f position_in_button;
		position_in_button.x = ProjectOnLineScalar(direct_touch.position.xy, pts[0], pts[3]);
		position_in_button.y = ProjectOnLineScalar(direct_touch.position.xy, pts[0], pts[1]);
		allow = allow && !isAlpha(position_in_button.x, position_in_button.y);
	}

	if (direct_touch.state == StatePossible) // check for hover
	{
		return allow;
	}
	
	if (direct_touch.state == StateBegan)
	{
		mIsMouseOver = false;
		mIsDown = false;
		switch (direct_touch.touch_state)
		{
		case DirectTouchEvent::TouchHover:
			if (mUseHoverColor) setValue("Color", (v3f)mHoverColor), setValue("Opacity", mHoverColor[3]);
			if(!EmitSignal(Signals::MouseOver, this, (usString)mParameter.const_ref())) // if no one connected send classic message
			{
				std::vector<CoreModifiableAttribute*> sendParams;
				sendParams.push_back(&mParameter);
				SendButtonNotifications(mMouseOverAction, this, sendParams);
			}
			mIsMouseOver = true;
			break;
		case DirectTouchEvent::TouchDown:
			if (mUseHoverColor)setValue("Color", (v3f)mClickedColor), setValue("Opacity", mClickedColor[3]);
			if(!EmitSignal(Signals::ClickDown, this, (usString)mParameter.const_ref())) // if no one connected send classic message
			{
				std::vector<CoreModifiableAttribute*> sendParams;
				sendParams.push_back(&mParameter);
				SendButtonNotifications(mClickDownAction, this, sendParams);
			}
			mIsDown = true;
			break;
		case DirectTouchEvent::TouchUp:
			if (mUseHoverColor) setValue("Color", mIsMouseOver ? (v3f)mHoverColor : (v3f)mIdleColor), setValue("Opacity", mIsMouseOver ? mHoverColor[3] : mIdleColor[3]);
			EmitSignal(Signals::TouchUp, this, (usString)mParameter.const_ref());
			break;
		}
	}
	else if (direct_touch.state == StateChanged)
	{
		
		if (direct_touch.touch_state == DirectTouchEvent::TouchHover)
		{
			// hover moved
			if (mSwallowEvents)
				*direct_touch.swallow_mask = 0xFFFFFFFF;
			/**direct_touch.swallow_mask |= (
				(1 << InputEventType::DirectTouch) |
				(1 << InputEventType::Click) |
				(1 << InputEventType::Swipe) 
				);*/
			//*direct_touch.swallow_mask = 0xFFFFFFFF;
		}
		else
		{
			mIsMouseOver = false;
			// should not happend
		}
	//	printf("direct touch changed");
	}
	else if (direct_touch.state == StateEnded)
	{
		mIsDown= false;	
		mIsMouseOver = false;
		if (direct_touch.touch_state == DirectTouchEvent::TouchHover)
		{
			// hover ended
			if (mUseHoverColor) setValue("Color", (v3f)mIdleColor), setValue("Opacity", mIdleColor[3]);
			if(!EmitSignal(Signals::MouseOut, this, (usString)mParameter.const_ref())) // if no one connected send classic message
			{
				std::vector<CoreModifiableAttribute*> sendParams;
				sendParams.push_back(&mParameter);
				SendButtonNotifications(mMouseOutAction, this, sendParams);
			}
		}
		else
		{
			// should not happend
		}
	//	printf("direct touch ended");
	}

	ChangeState();
	return false;
}
