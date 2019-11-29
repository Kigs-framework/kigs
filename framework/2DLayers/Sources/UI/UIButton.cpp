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
	, myClickDownAction(*this, false, "ClickDownAction", "")
	, myClickUpAction(*this, false, "ClickUpAction", "")
	, myMouseOverAction(*this, false, "MouseOverAction", "")
	, myMouseOutAction(*this, false, "MouseOutAction", "")
	, myUnSelectAction(*this, false, "UnSelectAction", "")
	, myParameter(*this, false, "Parameter", (kstl::string)"")
	, myStayPressed(*this, false, "StayPressed", false)
	, mybKeepClickOutside(*this, false, "KeepClickOutside", false)
	, myDefaultPressed(*this, false, "DefaultPressed", false)
	, myAutoResize(*this, false, "AutoResize", true)
	, mybInside(false)
	, myIsDown(false)
	, myIsMouseOver(false)
{
}

void UIButton::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == myIsEnabled.getLabelID())
	{
		// if disabled "reset" button state
		if (myIsEnabled == false)
		{
			myIsDown = false;
			myIsMouseOver = false;
		}
	}
	else if (labelid == mySizeX.getLabelID() || labelid == mySizeY.getLabelID())
	{
		if (myParent && myParent->isSubType("UIGroupButton"))
			((UIGroupButton*)myParent)->reComputeSize();
	}

	UITexturedItem::NotifyUpdate(labelid);
}


void UIButton::InitModifiable()
{
	UITexturedItem::InitModifiable();
	if (IsInit())
	{
		myIsDown = false;
		myIsMouseOver = false;

		//if (mUseHoverColor) setValue("Color", (v3f)mIdleColor), setValue("Opacity", mIdleColor[3]);

		if (myDefaultPressed && myStayPressed)
			myIsDown = true;

		myIsEnabled.changeNotificationLevel(Owner);
		mySizeX.changeNotificationLevel(Owner);
		mySizeY.changeNotificationLevel(Owner);

		ModuleInput* theInputModule = (ModuleInput*)KigsCore::GetModule("ModuleInput");
		// retreive click for activation
		// theInputModule->getTouchManager()->registerEvent(this, "ManageClickTouchEvent", Click, emptyFlag);
		// retreive direct touch for visual hover / push ...
		if (theInputModule)
		{
			theInputModule->getTouchManager()->registerEvent(this, "ManageDirectTouchEvent", DirectTouch, EmptyFlag);
			theInputModule->getTouchManager()->registerEvent(this, "ManageClickTouchEvent", Click, EmptyFlag);
		}
	}
}

void UIButton::AutoSize()
{
	// auto size button
	if (!myTexture.isNil())
	{
		float width, height;
		myTexture->GetSize(width, height);

		if (mySizeX == 0 && mySizeModeX == 0)
			mySizeX = width;

		if (mySizeY == 0 && mySizeModeY == 0)
			mySizeY = height;

		if (myParent && myParent->isSubType("UIGroupButton"))
			((UIGroupButton*)myParent)->reComputeSize();
	}
}

static void SendButtonNotifications(const kstl::string& actions, CoreModifiable* sender, kstl::vector<CoreModifiableAttribute*>& params)
{
	kstl::vector<kstl::string>	L_ActionVect = SplitStringByCharacter(actions, ':');
	for (unsigned int i = 0; i < L_ActionVect.size(); i++)
		KigsCore::GetNotificationCenter()->postNotificationName(L_ActionVect[i], params, sender);
}

bool UIButton::TriggerMouseMove(bool over, float _MouseDeltaX, float _MouseDeltaY)
{
	if (myIsEnabled == false) // mouse move catch
		return false;

	if (myIsMouseOver != over) // mouse is now over
	{
		kstl::vector<CoreModifiableAttribute*> mySendParams;
		mySendParams.push_back(&myParameter);
		
		myIsMouseOver = over;
		if (myIsMouseOver)
		{
			//EmitSignal(Signals::MouseOver, this, myParameter.const_ref());
			SendButtonNotifications(myMouseOverAction, this, mySendParams);
			kstl::vector<kstl::string>	L_ActionVect = SplitStringByCharacter(myMouseOverAction.const_ref(), ':');

			kstl::vector<CoreModifiableAttribute*> mySendParams;
			mySendParams.push_back(&myParameter);
			for (unsigned int i = 0; i < L_ActionVect.size(); i++)
				KigsCore::GetNotificationCenter()->postNotificationName(L_ActionVect[i], mySendParams, this);

		}
		else if (!myIsMouseOver)
		{
			//EmitSignal(Signals::MouseOut, this, myParameter.const_ref());
			SendButtonNotifications(myMouseOutAction, this, mySendParams);
		}

		if (!myStayPressed)
			myIsDown = false;  // release click if the mouse leave

		else if (mybInside)
		{
			EmitSignal(Signals::ClickUp, this, myParameter.const_ref());
			SendButtonNotifications(myClickUpAction, this, mySendParams);

			//EmitSignal(Signals::MouseOut, this, myParameter.const_ref());
			SendButtonNotifications(myMouseOutAction, this, mySendParams);


			mybInside = false;
		}

		ChangeState();
	}
	return true;
}

bool UIButton::TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)
{
	if (myIsEnabled == false) // mouse click catch
		return false;

	CoreModifiable* father = getFather();
	if (!father || !father->isSubType("UIItem"))
		father = nullptr;

	// catch left click event
	if (buttonEvent&UIInputEvent::LEFT)
	{
		bool down = buttonState&UIInputEvent::LEFT;

		kstl::vector<CoreModifiableAttribute*> mySendParams;
		mySendParams.push_back(&myParameter);
		
		if (!this->GetParents().empty())
		{
			CoreModifiable* parent = this->GetParents()[0];
			if (parent->isSubType("UIItem"))
			{
				//Ask permission at parent
				if (!((UIItem*)parent)->PermissionToClicRequiredFromParent(down, this))
				{
					return false;
				}
			}
		}

		if (!myIsDown && down) // Button Pressed
		{
			if (!myStayPressed && mybKeepClickOutside)
				catchClick = true;
			mybInside = true;
			myIsDown = down;

			// catch down event
			SendButtonNotifications(myClickDownAction, this, mySendParams);
			EmitSignal(Signals::ClickDown, this, myParameter.const_ref());

			/*
			if (myIsMouseOver)
			{
				EmitSignal(Signals::MouseOut, myParameter.const_ref());
				SendButtonNotifications(myMouseOutAction, this, mySendParams);
			}
			*/
		}
		else if (myIsDown && !down && !myStayPressed) // Button Released
		{
			if (!myStayPressed && mybKeepClickOutside)
				catchClick = false;
			mybInside = false;
			myIsDown = false;

			// catch up event
			//EmitSignal(Signals::ClickUp, this, myParameter.const_ref());
			SendButtonNotifications(myClickUpAction, this, mySendParams);
			/*
			if (myIsMouseOver)
			{
				EmitSignal(Signals::MouseOut, myParameter.const_ref());
				SendButtonNotifications(myMouseOutAction, this, mySendParams);
			}
			*/
		}
		else if (myIsDown && down && myStayPressed) // Button Released and it should stay pressed
		{
			mybInside = false;
			myIsDown = false;

			EmitSignal(Signals::UnSelect, this, myParameter.const_ref());
			SendButtonNotifications(myUnSelectAction, this, mySendParams);

			/*
			if (myIsMouseOver)
			{
				EmitSignal(Signals::MouseOut, myParameter.const_ref());
				SendButtonNotifications(myMouseOutAction, this, mySendParams);
			}
			*/
		}
		else if (myIsDown && !down && myStayPressed) // Button pressed and it should not stay pressed 
		{
			mybInside = false;
			myIsDown = true;

			// catch up event
			//EmitSignal(Signals::ClickUp, this, myParameter.const_ref());
			SendButtonNotifications(myClickUpAction, this, mySendParams);

			/*
			if (myIsMouseOver)
			{
				EmitSignal(Signals::MouseOut, myParameter.const_ref());
				SendButtonNotifications(myMouseOutAction, this, mySendParams);
			}
			*/
		}

		//Traitement
		if (father)
			((UIItem*)father)->TreatClick(down, this);


		ChangeState();
	}
	return true;
}

bool UIButton::isAlpha(float X, float Y)
{
	//Try to get my mask
	if (!myAlphaMask)
	{
		kstl::vector<ModifiableItemStruct> sons = getItems();
		for (unsigned int i = 0; i < sons.size(); i++)
		{
			if (sons[i].myItem->isSubType("AlphaMask"))
			{
				myAlphaMask = (AlphaMask*)sons[i].myItem;
				break;
			}
		}
	}

	if (myAlphaMask)
	{
		//Check on my mask the specified location
		return !myAlphaMask->CheckTo(X, Y);
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
		if (!EmitSignal(Signals::ClickUp, this, (usString)myParameter.const_ref())) // if no one connected send classic message
		{
			kstl::vector<CoreModifiableAttribute*> mySendParams;
			mySendParams.push_back(&myParameter);
			SendButtonNotifications(myClickUpAction, this, mySendParams);
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
		myIsMouseOver = false;
		myIsDown = false;
		switch (direct_touch.touch_state)
		{
		case DirectTouchEvent::TouchHover:
			if (mUseHoverColor) setValue("Color", (v3f)mHoverColor), setValue("Opacity", mHoverColor[3]);
			if(!EmitSignal(Signals::MouseOver, this, (usString)myParameter.const_ref())) // if no one connected send classic message
			{
				kstl::vector<CoreModifiableAttribute*> mySendParams;
				mySendParams.push_back(&myParameter);
				SendButtonNotifications(myMouseOverAction, this, mySendParams);
			}
			myIsMouseOver = true;
			break;
		case DirectTouchEvent::TouchDown:
			if (mUseHoverColor)setValue("Color", (v3f)mClickedColor), setValue("Opacity", mClickedColor[3]);
			if(!EmitSignal(Signals::ClickDown, this, (usString)myParameter.const_ref())) // if no one connected send classic message
			{
				kstl::vector<CoreModifiableAttribute*> mySendParams;
				mySendParams.push_back(&myParameter);
				SendButtonNotifications(myClickDownAction, this, mySendParams);
			}
			myIsDown = true;
			break;
		case DirectTouchEvent::TouchUp:
			if (mUseHoverColor) setValue("Color", myIsMouseOver ? (v3f)mHoverColor : (v3f)mIdleColor), setValue("Opacity", myIsMouseOver ? mHoverColor[3] : mIdleColor[3]);
			EmitSignal(Signals::TouchUp, this, (usString)myParameter.const_ref());
			break;
		}
	}
	else if (direct_touch.state == StateChanged)
	{
		
		if (direct_touch.touch_state == DirectTouchEvent::TouchHover)
		{
			// hover moved
			/**direct_touch.swallow_mask |= (
				(1 << InputEventType::DirectTouch) |
				(1 << InputEventType::Click) |
				(1 << InputEventType::Swipe) 
				);*/
			//*direct_touch.swallow_mask = 0xFFFFFFFF;
		}
		else
		{
			myIsMouseOver = false;
			// should not happend
		}
	//	printf("direct touch changed");
	}
	else if (direct_touch.state == StateEnded)
	{
		myIsDown= false;	
		myIsMouseOver = false;
		if (direct_touch.touch_state == DirectTouchEvent::TouchHover)
		{
			// hover ended
			if (mUseHoverColor) setValue("Color", (v3f)mIdleColor), setValue("Opacity", mIdleColor[3]);
			if(!EmitSignal(Signals::MouseOut, this, (usString)myParameter.const_ref())) // if no one connected send classic message
			{
				kstl::vector<CoreModifiableAttribute*> mySendParams;
				mySendParams.push_back(&myParameter);
				SendButtonNotifications(myMouseOutAction, this, mySendParams);
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
