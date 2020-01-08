#include "UI/UICustomDraw.h"
#include "NotificationCenter.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UICustomDraw, UICustomDraw, 2DLayers);
IMPLEMENT_CLASS_INFO(UICustomDraw)

bool UICustomDraw::TriggerMouseMove(bool over, float _MouseDeltaX, float _MouseDeltaY)
{
	if (myIsMouseOver != over)
	{
		myIsMouseOver = over;

		if (!myStayPressed)
			myIsDown = false;  // release click if the mouse leave
		else if (mybInside && myClickUpAction.const_ref() != "")
		{
			kstl::vector<kstl::string>	L_ActionVect = SplitStringByCharacter(myClickUpAction.const_ref(), ':');
			if (myParameter == usString(""))
			{
				for (unsigned int i = 0; i < L_ActionVect.size(); i++)
					KigsCore::GetNotificationCenter()->postNotificationName(L_ActionVect[i], this);
			}
			else
			{
				kstl::vector<CoreModifiableAttribute*> mySendParams;
				mySendParams.push_back(&myParameter);
				for (unsigned int i = 0; i < L_ActionVect.size(); i++)
					KigsCore::GetNotificationCenter()->postNotificationName(L_ActionVect[i], mySendParams, this);
			}
			L_ActionVect.clear();

			mybInside = false;
		}
	}
	return true;
}

bool UICustomDraw::TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)
{
	if (buttonEvent&UIInputEvent::LEFT)
	{
		bool down = buttonState&UIInputEvent::LEFT;
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
			mybInside = true;
			myIsDown = down;

			// catch down event
			if (myClickDownAction.const_ref() != "")
			{
				kstl::vector<kstl::string>	L_ActionVect = SplitStringByCharacter(myClickDownAction.const_ref(), ':');
				kstl::vector<CoreModifiableAttribute*> mySendParams;
				mySendParams.push_back(&myParameter);
				for (unsigned int i = 0; i < L_ActionVect.size(); i++)
					KigsCore::GetNotificationCenter()->postNotificationName(L_ActionVect[i], mySendParams, this);
			}
		}
		else if (myIsDown && !down && !myStayPressed) // Button Released
		{
			mybInside = false;
			myIsDown = false;

			// catch up event
			if (myClickUpAction.const_ref() != "")
			{
				kstl::vector<kstl::string>	L_ActionVect = SplitStringByCharacter(myClickUpAction.const_ref(), ':');
				kstl::vector<CoreModifiableAttribute*> mySendParams;
				mySendParams.push_back(&myParameter);

				for (unsigned int i = 0; i < L_ActionVect.size(); i++)
					KigsCore::GetNotificationCenter()->postNotificationName(L_ActionVect[i], mySendParams, this);

			}
		}
		else if (myIsDown && down && myStayPressed) // Button Released and it should stay pressed
		{
			mybInside = false;
			myIsDown = false;
		}
		else if (myIsDown && !down && myStayPressed) // Button pressed and it should not stay pressed 
		{
			mybInside = false;
			myIsDown = true;

			// catch up event
			if (myClickUpAction.const_ref() != "")
			{
				kstl::vector<kstl::string>	L_ActionVect = SplitStringByCharacter(myClickUpAction.const_ref(), ':');
				kstl::vector<CoreModifiableAttribute*> mySendParams;
				mySendParams.push_back(&myParameter);

				for (unsigned int i = 0; i < L_ActionVect.size(); i++)
					KigsCore::GetNotificationCenter()->postNotificationName(L_ActionVect[i], mySendParams, this);

			}
		}

		//Traitement
		if (!this->GetParents().empty())
		{
			CoreModifiable* parent = this->GetParents()[0];
			if (parent->isSubType("UIItem"))
			{
				//Ask permission at parent
				((UIItem*)parent)->TreatClick(down, this);
			}
		}
	}
	return true;
}

bool UICustomDraw::isAlpha(float X, float Y)
{
	if (myDelegate)
		return myDelegate->IsAlphafunc(X, Y);

	return false;
}


void UICustomDraw::ProtectedDraw(TravState* state)
{
	if (myDelegate)
		myDelegate->Drawfunc(state, this);
}