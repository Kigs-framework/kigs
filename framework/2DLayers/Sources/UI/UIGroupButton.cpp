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

bool	UIGroupButton::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if (item->isSubType("UIButton"))
	{
		myButtonList.push_back((UIButton*)item.get());
		myButtonNumber++;

		bool value = UIItem::addItem(item, pos PASS_LINK_NAME(linkName));
		computeSize(myButtonNumber, item.get());

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
		kstl::vector<UIButton*>::iterator ItStart = myButtonList.begin();
		kstl::vector<UIButton*>::iterator ItEnd = myButtonList.end();
		kstl::vector<UIButton*>::iterator ItSaved;

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
