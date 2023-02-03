#include "UI/UITextInput.h"
#include "NotificationCenter.h"
#include "ModuleInput.h"
#include "TouchInputEventManager.h"
#include "KeyboardDevice.h"
#include "Platform/Input/KeyDefine.h"

using namespace Kigs::Draw2D;

IMPLEMENT_CLASS_INFO(UITextInput)

void UITextInput::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == KigsID("Text")._id)
	{
		//mIsCueBanner = (mText.length() == 0);
		//if (mIsCueBanner)
		//	mText = mCueBanner;
	}
	ParentClassType::NotifyUpdate(labelid);

	//if (mIsCueBanner)
	//	mText = "";
}

void UITextInput::InitModifiable()
{
	ParentClassType::InitModifiable();
	if (IsInit())
	{
		mEventState->flags() |= Input::IgnoreSwallow;
		KigsCore::GetNotificationCenter()->addObserver(this, "FocusChanged", "GetFocus");
	}
}

DEFINE_METHOD(UITextInput, FocusChanged)
{
	if (sender != this && GetNodeFlag(UIItem_HasFocus))
		LoseFocus();

	return false;
}

void UITextInput::UpdateKeyboard(std::vector<KeyEvent>& keys)
{
	if (!keys.empty() && HasFocus())
	{
		usString newString = mIsCueBanner ? usString("") : mText;
		newString.reserve((int)(newString.length() + mKeycodeList.size()));

		for (auto& key : keys)
		{
			if (key.Action != key.ACTION_DOWN)
				continue;

			int cursor_pos = mSelectedCharacter - 1;
			if (cursor_pos < -1) cursor_pos = -1;
			if (cursor_pos > (int)newString.length()) cursor_pos = newString.length() - 1;
			if (key.KeyCode == VK_DELETE) // Suppr
			{
				if (!newString.empty())
				{
					newString.removeRange(cursor_pos+1, 1);
				}
			}
			else if (key.KeyCode == VK_BACK)//Return
			{
				if (!newString.empty() && cursor_pos>=0)
				{
					newString.removeRange(std::max(0,cursor_pos), 1);
					mSelectedCharacter = cursor_pos;
				}
			}
			else if (key.KeyCode == VK_LEFT)//Left
			{
				mSelectedCharacter = std::max(mSelectedCharacter - 1, 0);
			}
			else if (key.KeyCode == VK_RIGHT)//Right
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

		mKeycodeList.clear();

		EmitSignal(Signals::TextChanged, this);
	}
}

UITextInput::~UITextInput()
{
	if(GetNodeFlag(UIItem_HasFocus)) LoseFocus();
}


void UITextInput::GetFocus()
{
	//Get KeyBoard
	auto theInputModule = KigsCore::GetModule<Input::ModuleInput>();
	Input::KeyboardDevice* theKeyboard = theInputModule->GetKeyboard();
	KigsCore::Connect(theKeyboard, "KeyboardEvent", this, "UpdateKeyboard");
	SetNodeFlag(UIItem_HasFocus);
	KigsCore::GetNotificationCenter()->postNotificationName("GetFocus", this);
	theKeyboard->Show();
	setValue("ShowCursor", true);
}

void UITextInput::LoseFocus()
{
	//Get KeyBoard
	auto theInputModule= KigsCore::GetModule<Input::ModuleInput>();
	Input::KeyboardDevice* theKeyboard = theInputModule->GetKeyboard();
	KigsCore::Disconnect(theKeyboard, "KeyboardEvent", this, "UpdateKeyboard");

	ClearNodeFlag(UIItem_HasFocus);
	KigsCore::GetNotificationCenter()->removeObserver(this,"UpdateTextInput");

	if(mReleaseAction != "")
	{
		KigsCore::GetNotificationCenter()->postNotificationName(mReleaseAction,this);
	}
	theKeyboard->Hide();
	setValue("ShowCursor", false);
}

bool UITextInput::ManageClickTouchEvent(Input::ClickEvent& click_event)
{
	bool swallow = (*click_event.swallow_mask & (1 << click_event.type)) != 0;
	
	bool result = false;
	if(!swallow)
		result = ParentClassType::ManageClickTouchEvent(click_event);

	if (click_event.state == Input::StatePossible && !CanInteract(click_event.position.xy))
	{
		LoseFocus();
	}

	if (click_event.state == Input::StateRecognized && !swallow)
	{
		if (!GetNodeFlag(UIItem_HasFocus) && (click_event.button_state_mask & Input::ClickEvent::LeftButton) == Input::ClickEvent::LeftButton)
		{	
			GetFocus();
		}
		return true;
	}
	return result;
}
