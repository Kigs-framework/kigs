#include "UI/UIGroupButton.h"
#include "UI/UIButton.h"
#include "NotificationCenter.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIGroupButton, UIGroupButton, 2DLayers);
IMPLEMENT_CLASS_INFO(UIGroupButton)

UIGroupButton::UIGroupButton(const kstl::string& name, CLASS_NAME_TREE_ARG) :
	UIItem(name, PASS_CLASS_NAME_TREE_ARG)
	, myKeepOnePressed(*this, false, "KeepOnePressed", true)
	, myOnlyOnePressed(*this, false, "OnlyOnePressed", true)
	, myNoSelectedAction(*this, false, "NoSelectedAction", "")
	, myParameter(*this, false, "Parameter", (kstl::string)"")
{
	mySizeX = 0;
	mySizeY = 0;
	myButtonNumber = 0;
}

//-------------------------------------------------------------------------
//UIGroupButton

UIGroupButton::~UIGroupButton()
{
	myButtonNumber = 0;
	myButtonList.clear();
}

//-------------------------------------------------------------------------
//computeSize

void	UIGroupButton::computeSize(int _buttonNumber, CoreModifiable* a_item)
{
	//Recompute group button size
	int sizeX, sizeY = 0;
	int _Position[2];
	a_item->getValue(LABEL_TO_ID(SizeX), sizeX);
	a_item->getValue(LABEL_TO_ID(SizeY), sizeY);
	a_item->getArrayValue(LABEL_TO_ID(Position), _Position, 2);

	float tmpX = (float)(sizeX + _Position[0]);
	float tmpY = (float)(sizeY + _Position[1]);
	if (_buttonNumber == 1)
	{
		mySizeX = tmpX;
		mySizeY = tmpY;
	}
	else
	{
		//Check X axis
		if (tmpX > mySizeX)
			mySizeX += (tmpX - mySizeX);

		//Check Y axis
		if (tmpY > mySizeY)
			mySizeY += (tmpY - mySizeY);
	}
}

//-------------------------------------------------------------------------
//reComputeSize

void	UIGroupButton::reComputeSize()
{
	kstl::vector<UIButton*>::iterator ItStart = myButtonList.begin();
	kstl::vector<UIButton*>::iterator ItEnd = myButtonList.end();

	int buttoncount = 0;
	while (ItStart != ItEnd)
	{
		//Compute group button size
		buttoncount++;
		computeSize(buttoncount, (*ItStart));
		ItStart++;
	}
}


//-------------------------------------------------------------------------
//addItem

bool	UIGroupButton::addItem(CoreModifiable *item, ItemPosition pos DECLARE_LINK_NAME)
{
	if (item->isSubType("UIButton"))
	{
		myButtonList.push_back((UIButton*)item);
		myButtonNumber++;

		bool value = UIItem::addItem(item, pos PASS_LINK_NAME(linkName));
		computeSize(myButtonNumber, item);

		return value;
	}
	else if (item->isSubType("UIItemRenderer"))
	{
		return UIItem::addItem(item, pos PASS_LINK_NAME(linkName));
	}
	else
	{
#ifdef _DEBUG
		printf("ERROR : children of UIRadioButton must be UIButton  \n");
#endif
		return false;
	}
}

//-------------------------------------------------------------------------
//removeItem

bool	UIGroupButton::removeItem(CoreModifiable *item DECLARE_LINK_NAME)
{
	if (item->isSubType("UIButton"))
	{
		kstl::vector<UIButton*>::iterator ItStart = myButtonList.begin();
		kstl::vector<UIButton*>::iterator ItEnd = myButtonList.end();
		kstl::vector<UIButton*>::iterator ItSaved;

		int buttoncount = 0;
		while (ItStart != ItEnd)
		{

			if ((*ItStart) == (UIButton*)item)
			{
				ItSaved = ItStart;
			}
			else
			{
				//Compute new group button size
				buttoncount++;

				computeSize(buttoncount, (*ItStart));
			}
			ItStart++;
		}

		myButtonList.erase(ItSaved);
		myButtonNumber--;


		return UIItem::removeItem(item PASS_LINK_NAME(linkName));
	}
	else if (item->isSubType("UIItemRenderer"))
	{
		return UIItem::removeItem(item PASS_LINK_NAME(linkName));
	}

	return false;
}

//-------------------------------------------------------------------------
//InitModifiable

void	UIGroupButton::InitModifiable()
{
	UIItem::InitModifiable();
	if (_isInit)
	{

	}
}

//-------------------------------------------------------------------------
//TreatClick

void	UIGroupButton::TreatClick(bool _toPressed, UIItem* _sender)
{
	//Press needed and all button can be pressed
	if ((!myOnlyOnePressed && _toPressed) || !_toPressed)
		return;

	unsigned short pressedNumber = 0;

	if (!myKeepOnePressed)
	{
		for (unsigned int i = 0; i < myButtonNumber; i++)
		{
			pressedNumber += myButtonList[i]->isPressed();
		}
	}

	//Press needed
	if (_toPressed == true)
	{
		//Through buttons
		for (unsigned int i = 0; i < myButtonNumber; i++)
		{
			if (myButtonList[i]->isPressed())
			{
				if (myOnlyOnePressed && (_sender != myButtonList[i]))
				{
					//Release this button
					myButtonList[i]->ForcedChangeState(false);
					if (!myKeepOnePressed)
						pressedNumber--;
				}
			}
		}
	}

	if (!myKeepOnePressed)
	{
		if (pressedNumber == 0 && myNoSelectedAction.const_ref() != "")
		{
			kstl::vector<kstl::string>	L_ActionVect = SplitStringByCharacter(myNoSelectedAction.const_ref(), ':');
			if (myParameter == "")
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
		}
	}
}

//-------------------------------------------------------------------------
//PermissionToClicRequiredFromParent

bool	UIGroupButton::PermissionToClicRequiredFromParent(bool _toPressed, UIItem* _sender)
{
	//Press needed and all button can be pressed
	if (!myOnlyOnePressed && _toPressed)
		return true;

	bool value = true;


	//Through buttons
	for (unsigned int i = 0; i < myButtonNumber; i++)
	{
		//Press needed
		if (_toPressed == true)
		{
			if (myButtonList[i]->isPressed())
			{

				if (myKeepOnePressed && (_sender == myButtonList[i]))
				{
					value = false;
				}
			}
		}
		//Release needed
		else if (myKeepOnePressed)
		{
			value = false;
			//check if a child stay pressed
			if (myButtonList[i]->isPressed())
			{
				value = true;
				break;
			}
		}
	}

	return value;
}

