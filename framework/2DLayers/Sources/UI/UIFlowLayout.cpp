#include "UI/UIFlowLayout.h"
#include "Base2DLayer.h"
#include "RenderingScreen.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIFlowLayout, UIFlowLayout, 2DLayers);
IMPLEMENT_CLASS_INFO(UIFlowLayout)

UIFlowLayout::UIFlowLayout(const kstl::string& name, CLASS_NAME_TREE_ARG) :
UILayout(name, PASS_CLASS_NAME_TREE_ARG)
, myPadding(*this, false, LABEL_AND_ID(Padding), 0, 0)
, mySortByPriority(*this, false, LABEL_AND_ID(SortByPriority), false)
, myRescaleToFit(*this, false, LABEL_AND_ID(RescaleToFit))
{
	myPadding.changeNotificationLevel(Owner);
	mySortByPriority.changeNotificationLevel(Owner);
}

void UIFlowLayout::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == mySortByPriority.getLabelID()) ||
		(labelid == myPadding.getLabelID()))
	{
		myNeedRecompute = true;
	}
	UILayout::NotifyUpdate(labelid);
}

void UIFlowLayout::RecomputeLayout() 
{
	if (!GetSons().empty())
	{
		kstl::vector<CoreModifiable*> items;
		if (!mySortByPriority)
		{
			// Get sons ordered by insertion
			auto& sons = getItems();
			for (auto son : sons)
			{
				if (son.myItem->isSubType(Node2D::myClassID))
				{
					items.push_back(son.myItem.get());
				}
			}
		}
		else 
		{
			// Get sons ordered by priority
			for (auto son : mySons)
			{
				items.push_back(son);
			}
		}

		int padding_x = myPadding[0];
		int padding_y = myPadding[1];

		float rowHeight = 0;
		float heightAccumulator = 0;
		float widthAccumulator = 0;

		for(auto item : items)
        {
			v2f size = item->as<Node2D>()->GetSize();

			// cannot add another item on the row >> new line
			if (widthAccumulator + size.x > myRealSize.x)
			{
				widthAccumulator = 0;
				heightAccumulator += rowHeight + padding_y;

				item->setArrayValue("Position", widthAccumulator, heightAccumulator);
				widthAccumulator += size.x + padding_x;
			}
			// add the item on the row
			else
			{
				item->setArrayValue("Position", widthAccumulator, heightAccumulator);
				widthAccumulator += size.x + padding_x;

				rowHeight = (rowHeight < size.y) ? size.y : rowHeight;
			}
		}
	}
}