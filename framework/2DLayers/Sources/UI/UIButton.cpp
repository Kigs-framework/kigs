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


//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIButton, UIButton, 2DLayers);
IMPLEMENT_CLASS_INFO(UIButton)

IMPLEMENT_CONSTRUCTOR(UIButton)
	, mClickDownAction(*this, false, "ClickDownAction", "")
	, mClickUpAction(*this, false, "ClickUpAction", "")
	, mMouseOverAction(*this, false, "MouseOverAction", "")
	, mMouseOutAction(*this, false, "MouseOutAction", "")
	, mUnSelectAction(*this, false, "UnSelectAction", "")
	, mParameter(*this, false, "Parameter", (kstl::string)"")
	, mStayPressed(*this, false, "StayPressed", false)
	, mKeepClickOutside(*this, false, "KeepClickOutside", false)
	, mDefaultPressed(*this, false, "DefaultPressed", false)
	, mAutoResize(*this, false, "AutoResize", true)
	, mInside(false)
	, mIsDown(false)
	, mIsMouseOver(false)
{
}

void UIButton::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == mIsEnabled.getLabelID())
	{
		// if disabled "reset" button state
		if (mIsEnabled == false)
		{
			mIsDown = false;
			mIsMouseOver = false;
		}
	}
	else if (labelid == mSizeX.getLabelID() || labelid == mSizeY.getLabelID())
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

		mIsEnabled.changeNotificationLevel(Owner);
		mSizeX.changeNotificationLevel(Owner);
		mSizeY.changeNotificationLevel(Owner);

		ModuleInput* theInputModule = (ModuleInput*)KigsCore::GetModule("ModuleInput");
		// retreive click for activation
		// theInputModule->getTouchManager()->registerEvent(this, "ManageClickTouchEvent", Click, emptyFlag);
		// retreive direct touch for visual hover / push ...
		if (theInputModule)
		{
			static_cast<TouchEventStateDirectTouch*>(theInputModule->getTouchManager()->registerEvent(this, "ManageDirectTouchEvent", DirectTouch, EmptyFlag))->setAutoTouchDownDistance(0.05f);
			static_cast<TouchEventStateClick*>(theInputModule->getTouchManager()->registerEvent(this, "ManageClickTouchEvent", Click, EmptyFlag))->setAutoClickDistance(0.05f);
		}
	}
}

void UIButton::AutoSize()
{
	// auto size button
	if (!mTexturePointer.isNil())
	{
		float width, height;
		mTexturePointer->GetSize(width, height);

		if (mSizeX == 0 && mSizeModeX == 0)
			mSizeX = width;

		if (mSizeY == 0 && mSizeModeY == 0)
			mSizeY = height;

		if (mParent && mParent->isSubType("UIGroupButton"))
			((UIGroupButton*)mParent)->reComputeSize();
	}
}

static void SendButtonNotifications(const kstl::string& actions, CoreModifiable* sender, kstl::vector<CoreModifiableAttribute*>& params)
{
	kstl::vector<kstl::string>	L_ActionVect = SplitStringByCharacter(actions, ':');
	for (unsigned int i = 0; i < L_ActionVect.size(); i++)
		KigsCore::GetNotificationCenter()->postNotificationName(L_ActionVect[i], params, sender);
}

bool UIButton::isAlpha(float X, float Y)
{
	//Try to get mask
	if (!mAlphaMask)
	{
		kstl::vector<ModifiableItemStruct> sons = getItems();
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


bool UIButton::ManageClickTouchEvent(ClickEvent& click_event)
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
			kstl::vector<CoreModifiableAttribute*> sendParams;
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
				kstl::vector<CoreModifiableAttribute*> sendParams;
				sendParams.push_back(&mParameter);
				SendButtonNotifications(mMouseOverAction, this, sendParams);
			}
			mIsMouseOver = true;
			break;
		case DirectTouchEvent::TouchDown:
			if (mUseHoverColor)setValue("Color", (v3f)mClickedColor), setValue("Opacity", mClickedColor[3]);
			if(!EmitSignal(Signals::ClickDown, this, (usString)mParameter.const_ref())) // if no one connected send classic message
			{
				kstl::vector<CoreModifiableAttribute*> sendParams;
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
				kstl::vector<CoreModifiableAttribute*> sendParams;
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
