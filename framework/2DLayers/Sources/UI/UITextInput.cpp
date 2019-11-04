#include "UI/UITextInput.h"
#include "NotificationCenter.h"
#include "ModuleInput.h"
#include "TouchInputEventManager.h"
#include "KeyboardDevice.h"
#include "Platform/Input/KeyDefine.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UITextInput, UITextInput, 2DLayers);
IMPLEMENT_CLASS_INFO(UITextInput)

void UITextInput::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == mText.getLabelID())
	{
		//myIsCueBanner = (mText.const_ref().length() == 0);
		//if (myIsCueBanner)
		//	mText = myCueBanner;
	}
	ParentClassType::NotifyUpdate(labelid);

	//if (myIsCueBanner)
	//	mText = "";
}

void UITextInput::InitModifiable()
{
	ParentClassType::InitModifiable();
	if (IsInit())
	{
		mEventState->flags() |= IgnoreSwallow;
		KigsCore::GetNotificationCenter()->addObserver(this, "FocusChanged", "GetFocus");
	}
}

DEFINE_METHOD(UITextInput, FocusChanged)
{
	if (sender != this && myFocus)
		LoseFocus();

	return false;
}

void UITextInput::UpdateKeyboard(kstl::vector<KeyEvent>& keys)
{
	if (!keys.empty() && HasFocus())
	{
		usString newString = myIsCueBanner ? "" : mText.const_ref();
		newString.reserve(newString.length() + myKeycodeList.size());

		for (auto& key : keys)
		{
			if (key.Action != key.ACTION_DOWN)
				continue;

			int cursor_pos = mSelectedCharacter - 1;
			if (cursor_pos < -1) cursor_pos = -1;
			if (cursor_pos > (int)newString.length()) cursor_pos = newString.length() - 1;
			if (key.KeyCode == CM_KEY_DELETE) // Suppr
			{
				if (!newString.empty())
				{
					newString.removeRange(cursor_pos+1, 1);
				}
			}
			else if (key.KeyCode == CM_KEY_BACK)//Return
			{
				if (!newString.empty() && cursor_pos>=0)
				{
					newString.removeRange(std::max(0,cursor_pos), 1);
					mSelectedCharacter = cursor_pos;
				}
			}
			else if (key.KeyCode == CM_KEY_LEFT)//Left
			{
				mSelectedCharacter = std::max(mSelectedCharacter - 1, 0);
			}
			else if (key.KeyCode == CM_KEY_RIGHT)//Right
			{
				mSelectedCharacter = std::min(mSelectedCharacter + 1, mLastMaxIndex);
			}
			else if (key.KeyCode > 0)
			{
				if(key.Unicode != 0 && key.Unicode <= USHRT_MAX)// && newChar < mFontMap->mFontInfo.numGlyphs)
				{
					usString before = newString.substr(0, cursor_pos+1);
					usString after = newString.substr(cursor_pos+1, INT_MAX);

					newString = before;
					newString += (unsigned short)key.Unicode;
					newString += after;

					mSelectedCharacter = cursor_pos+2;
				}
			}
		}
		setValue("Text", newString);

		myKeycodeList.clear();

		EmitSignal(Signals::TextChanged, this);
	}
}

UITextInput::~UITextInput()
{
	if(myFocus) LoseFocus();
}


void UITextInput::GetFocus()
{
	//Get KeyBoard
	ModuleInput* theInputModule= KigsCore::GetModule<ModuleInput>();
	KeyboardDevice* theKeyboard = theInputModule->GetKeyboard();
	KigsCore::Connect(theKeyboard, "KeyboardEvent", this, "UpdateKeyboard");
	myFocus = true;
	KigsCore::GetNotificationCenter()->postNotificationName("GetFocus", this);
	theKeyboard->Show();
	setValue("ShowCursor", true);
}

void UITextInput::LoseFocus()
{
	//Get KeyBoard
	ModuleInput* theInputModule= KigsCore::GetModule<ModuleInput>();
	KeyboardDevice* theKeyboard = theInputModule->GetKeyboard();
	KigsCore::Disconnect(theKeyboard, "KeyboardEvent", this, "UpdateKeyboard");

	myFocus = false;
	KigsCore::GetNotificationCenter()->removeObserver(this,"UpdateTextInput");

	if(myReleaseAction.const_ref() != "")
	{
		KigsCore::GetNotificationCenter()->postNotificationName(myReleaseAction.const_ref(),this);
	}
	theKeyboard->Hide();
	setValue("ShowCursor", false);
}

bool UITextInput::ManageClickTouchEvent(ClickEvent& click_event)
{
	bool swallow = (*click_event.swallow_mask & (1 << click_event.type)) != 0;
	
	bool result = false;
	if(!swallow)
		result = ParentClassType::ManageClickTouchEvent(click_event);

	if (click_event.state == StatePossible && !CanInteract(click_event.position.xy))
	{
		LoseFocus();
	}

	if (click_event.state == StateRecognized && !swallow)
	{
		if (!myFocus && (click_event.button_state_mask & ClickEvent::LeftButton) == ClickEvent::LeftButton)
		{	
			GetFocus();
		}
		return true;
	}
	return result;
}
