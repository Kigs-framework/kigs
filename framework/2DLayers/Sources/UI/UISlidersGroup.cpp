#include "UI/UISlidersGroup.h"
#include "UI/UISlider.h"
#include "NotificationCenter.h"
#include <cstdlib>

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UISlidersGroup, UISlidersGroup, 2DLayers);
IMPLEMENT_CLASS_INFO(UISlidersGroup)

UISlidersGroup::UISlidersGroup(const kstl::string& name,CLASS_NAME_TREE_ARG) : 
UIDrawableItem(name, PASS_CLASS_NAME_TREE_ARG),
mComunalValue(*this,false,LABEL_AND_ID(ComunalValue),0)
{
	mSliderNumber=0;
}

UISlidersGroup::~UISlidersGroup()
{
	mSliderNumber = 0;
	mSliderList.clear();
}


bool	UISlidersGroup::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if(item->isSubType("UIItem") && !item->getItems().empty()) 
	{
		kstl::vector<ModifiableItemStruct> childs = item->getItems();

		kstl::vector<ModifiableItemStruct>::iterator ITStart = childs.begin();
		kstl::vector<ModifiableItemStruct>::iterator ITEnd = childs.end();

		while(ITStart!=ITEnd)
		{
			if( (*ITStart).mItem->isSubType("UISlider"))
			{
				break;
			}
			ITStart++;
		}
		if(ITStart == ITEnd)
			return false;

		
		mSliderList.push_back((UISlider*)(*ITStart).mItem.get());
		mSliderNumber++;

		bool value = UIItem::addItem(item,pos PASS_LINK_NAME(linkName));
		//Recompute group button size
		v2f sizeS(0,0);
		int _Position[2];
		item->getValue("Size", sizeS);
		item->getArrayValue("Position",_Position,2);
		
		
		if(mSliderNumber==1)
		{
			mSize = sizeS;
		}
		else
		{
			//Check X axis
			if ((sizeS.x + _Position[0])>0)
			if(((unsigned int)(sizeS.x + _Position[0])) > mSize[0])
			{
				mSize[0] += ((sizeS.x + _Position[0]) - mSize[0]);
			}
			
			//Check Y axis
			if ((sizeS.y + _Position[1])>0)
			if (((unsigned int)(sizeS.y + _Position[1])) > mSize[1])
			{
				mSize[1] += ((sizeS.y + _Position[1]) - mSize[1]);
			}
		}

		return value;
	}
	else if(item->isSubType("UIItemRenderer"))
	{
		return UIDrawableItem::addItem(item, pos PASS_LINK_NAME(linkName));
	}
	else
	{
#ifdef _DEBUG
		printf("ERROR : children of UISliderGroup must be UISlider  \n");
#endif
		return false;
	}
}

bool	UISlidersGroup::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	if(item->isSubType("UIItem") && !item->getItems().empty()) 
	{
		kstl::vector<ModifiableItemStruct> childs = item->getItems();

		kstl::vector<ModifiableItemStruct>::iterator ITStart = childs.begin();
		kstl::vector<ModifiableItemStruct>::iterator ITEnd = childs.end();

		while(ITStart!=ITEnd)
		{
			if( (*ITStart).mItem->isSubType("UISlider"))
			{
				break;
			}
			ITStart++;
		}
		if(ITStart == ITEnd)
			return false;

		UISlider* temp = (UISlider*)(*ITStart).mItem.get();
		kstl::vector<UISlider*>::iterator ItStart =		mSliderList.begin();
		kstl::vector<UISlider*>::iterator ItEnd =		mSliderList.end();
		kstl::vector<UISlider*>::iterator ItSaved;

		int	Slidercount = 0;
		while(ItStart!=ItEnd)
		{
			
			if((*ItStart) == temp)
			{
				ItSaved = ItStart;
			}
			else
			{
				//Compute new group button size
				Slidercount++;

				v2f sizeS(0,0);
				int _Position[2];
				(*ItStart)->GetParents()[0]->getValue("Size", sizeS);
				(*ItStart)->GetParents()[0]->getArrayValue(LABEL_TO_ID(Position),_Position,2);
		
		

				if(Slidercount==1)
				{
					mSize = sizeS;
				}
				else
				{
					//Check X axis
					if ((sizeS.x + _Position[0])>0)
					if (((unsigned int)(sizeS.x + _Position[0])) > mSize[0])
					{
						mSize[0] += ((sizeS.x + _Position[0]) - mSize[0]);
					}
			
					//Check Y axis
					if ((sizeS.y + _Position[1])>0)
						if (((unsigned int)(sizeS.y + _Position[1])) > mSize[1])
						{
							mSize[1] += ((sizeS.y + _Position[1]) - mSize[1]);
					}
				}
			}
			ItStart++;
		}

		mSliderList.erase(ItSaved);
		mSliderNumber--;


		return UIDrawableItem::removeItem(item PASS_LINK_NAME(linkName));
	}
	else if (item->isSubType("UIItemRenderer"))
	{
		return UIDrawableItem::removeItem(item PASS_LINK_NAME(linkName));
	}

	return false;
}


void	UISlidersGroup::InitModifiable()
{
	UIDrawableItem::InitModifiable();
	if(_isInit)
	{
		mRemainingValue = mComunalValue;
	}
}

void	UISlidersGroup::Reset()
{
	mRemainingValue = mComunalValue;

	kstl::vector<UISlider*>::iterator ItStart =		mSliderList.begin();
	kstl::vector<UISlider*>::iterator ItEnd =		mSliderList.end();

	while(ItStart != ItEnd)
	{
		(*ItStart)->ForcedRatio((*ItStart)->Get_InitialRatio());
		ItStart++;
	}

}
