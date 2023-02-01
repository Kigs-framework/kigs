#include "UI/UIGroupButton.h"
#include "UI/UIButton.h"
#include "NotificationCenter.h"

using namespace Kigs::Draw2D;

IMPLEMENT_CLASS_INFO(UIGroupButton)

UIGroupButton::UIGroupButton(const std::string& name, CLASS_NAME_TREE_ARG) :
	UIItem(name, PASS_CLASS_NAME_TREE_ARG)
	, mKeepOnePressed(*this, "KeepOnePressed", true)
	, mOnlyOnePressed(*this, "OnlyOnePressed", true)
	, mNoSelectedAction(*this, "NoSelectedAction", "")
	, mParameter(*this, "Parameter", (std::string)"")
{
	mSize = v2f(0,0);
	mButtonNumber = 0;
}

//-------------------------------------------------------------------------
//UIGroupButton

UIGroupButton::~UIGroupButton()
{
	mButtonNumber = 0;
	mButtonList.clear();
}

//-------------------------------------------------------------------------
//computeSize

void	UIGroupButton::computeSize(int _buttonNumber, CoreModifiable* a_item)
{
	//Recompute group button size
	v2f lsize(0,0);
	int _Position[2];
	a_item->getValue("Size",lsize);
	a_item->getArrayValue("Position", _Position, 2);

	v2f tmpS((float)(lsize.x + _Position[0]),(float)(lsize.y + _Position[1]));
	if (_buttonNumber == 1)
	{
		mSize = tmpS;
	}
	else
	{
		//Check X axis
		if (tmpS[0] > mSize[0])
			mSize[0] += (tmpS[0] - mSize[0]);

		//Check Y axis
		if (tmpS[1] > mSize[1])
			mSize[1] += (tmpS[1] - mSize[1]);
	}
}

//-------------------------------------------------------------------------
//reComputeSize

void	UIGroupButton::reComputeSize()
{
	std::vector<UIButton*>::iterator ItStart = mButtonList.begin();
	std::vector<UIButton*>::iterator ItEnd = mButtonList.end();

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

bool	UIGroupButton::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if (item->isSubType("UIButton"))
	{
		mButtonList.push_back((UIButton*)item.get());
		mButtonNumber++;

		bool value = UIItem::addItem(item, pos PASS_LINK_NAME(linkName));
		computeSize(mButtonNumber, item.get());

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

bool	UIGroupButton::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	if (item->isSubType("UIButton"))
	{
		std::vector<UIButton*>::iterator ItStart = mButtonList.begin();
		std::vector<UIButton*>::iterator ItEnd = mButtonList.end();
		std::vector<UIButton*>::iterator ItSaved;

		int buttoncount = 0;
		while (ItStart != ItEnd)
		{

			if ((*ItStart) == item.get())
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

		mButtonList.erase(ItSaved);
		mButtonNumber--;


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
