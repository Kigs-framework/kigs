#include "UI/UIFlowLayout.h"
#include "RenderingScreen.h"

using namespace Kigs::Draw2D;
using namespace Kigs::Core;

IMPLEMENT_CLASS_INFO(UIFlowLayout)

UIFlowLayout::UIFlowLayout(const std::string& name, CLASS_NAME_TREE_ARG) :
UILayout(name, PASS_CLASS_NAME_TREE_ARG)
, mPadding(*this,  "Padding", 0, 0)
, mSortByPriority(*this, "SortByPriority", false)
, mRescaleToFit(*this,  "RescaleToFit")
{
	setOwnerNotification("Padding", true);
	setOwnerNotification("SortByPriority", true);

}

void UIFlowLayout::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == KigsID("SortByPriority")._id ) ||
		(labelid == KigsID("Padding")._id))
	{
		mNeedRecompute = true;
	}
	UILayout::NotifyUpdate(labelid);
}

void UIFlowLayout::RecomputeLayout() 
{
	if (!GetSons().empty())
	{
		std::vector<CoreModifiable*> items;
		if (!mSortByPriority)
		{
			// Get sons ordered by insertion
			auto& sons = getItems();
			for (auto son : sons)
			{
				if (son.mItem->isSubType(Node2D::mClassID))
				{
					items.push_back(son.mItem.get());
				}
			}
		}
		else 
		{
			// Get sons ordered by priority
			for (auto son : mSons)
			{
				items.push_back(son);
			}
		}

		int padding_x = mPadding[0];
		int padding_y = mPadding[1];

		float rowHeight = 0;
		float heightAccumulator = 0;
		float widthAccumulator = 0;

		for(auto item : items)
        {
			v2f size = item->as<Node2D>()->GetSize();

			// cannot add another item on the row >> new line
			if (widthAccumulator + size.x > mRealSize.x)
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