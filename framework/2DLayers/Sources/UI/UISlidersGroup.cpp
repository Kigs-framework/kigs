#include "UI/UISlidersGroup.h"
#include "UI/UISlider.h"
#include "NotificationCenter.h"
#include <cstdlib>

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UISlidersGroup, UISlidersGroup, 2DLayers);
IMPLEMENT_CLASS_INFO(UISlidersGroup)

UISlidersGroup::UISlidersGroup(const kstl::string& name,CLASS_NAME_TREE_ARG) : 
UIDrawableItem(name, PASS_CLASS_NAME_TREE_ARG),
myComunalValue(*this,false,LABEL_AND_ID(ComunalValue),0)
{
	mySliderNumber=0;
}

UISlidersGroup::~UISlidersGroup()
{
	mySliderNumber = 0;
	mySliderList.clear();
}


bool	UISlidersGroup::addItem(CoreModifiable *item, ItemPosition pos DECLARE_LINK_NAME)
{
	if(item->isSubType("UIItem") && !item->getItems().empty()) 
	{
		kstl::vector<ModifiableItemStruct> childs = item->getItems();

		kstl::vector<ModifiableItemStruct>::iterator ITStart = childs.begin();
		kstl::vector<ModifiableItemStruct>::iterator ITEnd = childs.end();

		while(ITStart!=ITEnd)
		{
			if( (*ITStart).myItem->isSubType("UISlider"))
			{
				break;
			}
			ITStart++;
		}
		if(ITStart == ITEnd)
			return false;

		UISlider* temp = (UISlider*)(*ITStart).myItem;
		mySliderList.push_back(temp);
		mySliderNumber++;

		bool value = UIItem::addItem(item,pos PASS_LINK_NAME(linkName));
		//Recompute group button size
		int sizeX, sizeY = 0;
		int _Position[2];
		item->getValue(LABEL_TO_ID(SizeX),sizeX);
		item->getValue(LABEL_TO_ID(SizeY),sizeY);
		item->getArrayValue(LABEL_TO_ID(Position),_Position,2);
		
		
		if(mySliderNumber==1)
		{
			mySizeX = sizeX;
			mySizeY = sizeY;
		}
		else
		{
			//Check X axis
			if ((sizeX + _Position[0])>0)
			if(((unsigned int)(sizeX + _Position[0])) > mySizeX)
			{
				mySizeX += ((sizeX + _Position[0]) - mySizeX);
			}
			
			//Check Y axis
			if ((sizeY + _Position[1])>0)
			if (((unsigned int)(sizeY + _Position[1])) > mySizeY)
			{
				mySizeY += ((sizeY + _Position[1]) - mySizeY);
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

bool	UISlidersGroup::removeItem(CoreModifiable *item DECLARE_LINK_NAME)
{
	if(item->isSubType("UIItem") && !item->getItems().empty()) 
	{
		kstl::vector<ModifiableItemStruct> childs = item->getItems();

		kstl::vector<ModifiableItemStruct>::iterator ITStart = childs.begin();
		kstl::vector<ModifiableItemStruct>::iterator ITEnd = childs.end();

		while(ITStart!=ITEnd)
		{
			if( (*ITStart).myItem->isSubType("UISlider"))
			{
				break;
			}
			ITStart++;
		}
		if(ITStart == ITEnd)
			return false;

		UISlider* temp = (UISlider*)(*ITStart).myItem;
		kstl::vector<UISlider*>::iterator ItStart =		mySliderList.begin();
		kstl::vector<UISlider*>::iterator ItEnd =		mySliderList.end();
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

				int sizeX, sizeY = 0;
				int _Position[2];
				(*ItStart)->GetParents()[0]->getValue(LABEL_TO_ID(SizeX),sizeX);
				(*ItStart)->GetParents()[0]->getValue(LABEL_TO_ID(SizeY),sizeY);
				(*ItStart)->GetParents()[0]->getArrayValue(LABEL_TO_ID(Position),_Position,2);
		
		

				if(Slidercount==1)
				{
					mySizeX = sizeX;
					mySizeY = sizeY;
				}
				else
				{
					//Check X axis
					if ((sizeX + _Position[0])>0)
					if (((unsigned int)(sizeX + _Position[0])) > mySizeX)
					{
						mySizeX += ((sizeX + _Position[0]) - mySizeX);
					}
			
					//Check Y axis
					if ((sizeY + _Position[1])>0)
					if (((unsigned int)(sizeY + _Position[1])) > mySizeY)
					{
						mySizeY += ((sizeY + _Position[1]) - mySizeY);
					}
				}
			}
			ItStart++;
		}

		mySliderList.erase(ItSaved);
		mySliderNumber--;


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
		myRemainingValue = myComunalValue;
	}
}

void	UISlidersGroup::Reset()
{
	myRemainingValue = myComunalValue;

	kstl::vector<UISlider*>::iterator ItStart =		mySliderList.begin();
	kstl::vector<UISlider*>::iterator ItEnd =		mySliderList.end();

	while(ItStart != ItEnd)
	{
		(*ItStart)->ForcedRatio((*ItStart)->Get_InitialRatio());
		ItStart++;
	}

}

void	UISlidersGroup::TreatMove(int X, int Y, int oldValue, int desiredValue, UIItem* _sender)
{
	UISlider* current=(UISlider*)_sender;

	// compute consumed by other sliders
	int ConsumedValue = 0;
	kstl::vector<UISlider*>::iterator ItStart =		mySliderList.begin();
	kstl::vector<UISlider*>::iterator ItEnd =		mySliderList.end();

	while(ItStart != ItEnd)
	{
		if((*ItStart)!=_sender)
			ConsumedValue += abs((*ItStart)->Get_CurrentRatio() - (*ItStart)->Get_InitialRatio()) * (*ItStart)->Get_Multiplier();
	
		ItStart++;
	}
	
	// then check if current should be borned
	if((ConsumedValue+100)>myComunalValue)
	{
		// compute min and max
		int remain=(((int)myComunalValue)-ConsumedValue)/current->Get_Multiplier();

		int ratiomin=((int)current->Get_InitialRatio())-remain;
		int ratiomax=((int)current->Get_InitialRatio())+remain;

		if(desiredValue<=ratiomin)
		{
			((UISlider*)_sender)->ForcedRatio(ratiomin);
			myRemainingValue = 0;
		}
		else if(desiredValue>=ratiomax)
		{
			((UISlider*)_sender)->ForcedRatio(ratiomax);
			myRemainingValue = 0;
		}
		else
		{
			int CurrentValue = std::abs(desiredValue - current->Get_InitialRatio()) * current->Get_Multiplier();
			myRemainingValue = myComunalValue - ConsumedValue - CurrentValue;
		}
	}
	else
	{
		int CurrentValue = std::abs(desiredValue - current->Get_InitialRatio()) * current->Get_Multiplier();
		myRemainingValue = myComunalValue - ConsumedValue - CurrentValue;
	}
}

