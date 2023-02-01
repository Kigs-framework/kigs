#include "UI/UIGridLayout.h"
#include "Base2DLayer.h"
#include "RenderingScreen.h"

using namespace Kigs::Draw2D;
using namespace Kigs::Core;

IMPLEMENT_CLASS_INFO(UIGridLayout)

UIGridLayout::UIGridLayout(const std::string& name, CLASS_NAME_TREE_ARG) :
	UILayout(name, PASS_CLASS_NAME_TREE_ARG)
	, mColumns(*this, "Columns", 0)
	, mRowHeight(*this, "RowHeight", 0)
	, mColumnWidth(*this, "ColumnWidth", 0)
	, mHeaderRow(*this, "HeaderRow", false)
	, mPadding(*this, "Padding", 0, 0)
	, mSortByPriority(*this, "SortByPriority", false)
	, mResizeElements(*this, "ResizeElements", false)
	, mAutoResize(*this, "AutoResize", true)
{
	setOwnerNotification("Columns", true);
	setOwnerNotification("Padding", true);
	setOwnerNotification("HeaderRow", true);
	setOwnerNotification("RowHeight", true);
	setOwnerNotification("ColumnWidth", true);
	setOwnerNotification("SortByPriority", true);
	setOwnerNotification("ResizeElements", true);
	setOwnerNotification("AutoResize", true);

}

void UIGridLayout::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == KigsID("Columns")._id ) ||
		(labelid == KigsID("HeaderRow")._id ) ||
		(labelid == KigsID("RowHeight")._id ) ||
		(labelid == KigsID("ColumnWidth")._id ) ||
		(labelid == KigsID("SortByPriority")._id ) ||
		(labelid == KigsID("ResizeElements")._id ) ||
		(labelid == KigsID("AutoResize")._id ) ||
		(labelid == KigsID("Padding")._id ))
	{
		mNeedRecompute = true;
	}
	UILayout::NotifyUpdate(labelid);
}

void UIGridLayout::RecomputeLayout()
{
	if (!mNeedRecompute)
		return;

	if (!GetSons().empty())
	{

		int padding_x = mPadding[0];
		int padding_y = mPadding[1];


		std::vector<CoreModifiable*> items;

		if (!mSortByPriority) {
			// Get sons ordered by insertion
			const std::vector<ModifiableItemStruct> sons = getItems();
			std::vector<ModifiableItemStruct>::const_iterator itsons;
			for (itsons = sons.begin(); itsons != sons.end(); itsons++)
			{
				if ((*itsons).mItem->isSubType(Node2D::mClassID))
				{
					items.push_back((CoreModifiable*)(*itsons).mItem.get());
				}
			}
		}
		else {
			// Get sons ordered by priority
			std::set<Node2D*, Node2D::PriorityCompare> sons = GetSons();
			std::set<Node2D*, Node2D::PriorityCompare>::reverse_iterator itsons = sons.rbegin();
			std::set<Node2D*, Node2D::PriorityCompare>::reverse_iterator itend = sons.rend();
			while (itsons != itend) {
				items.push_back(*itsons);
				itsons++;
			}
		}


		std::vector<CoreModifiable*>::iterator it = items.begin();
		std::vector<CoreModifiable*>::iterator end = items.end();

		auto nb_items = items.size();

		float heightAccumulator = 0;
		float maxWidth = 0;
		float maxHeight = 0;


		int r = 0;
		unsigned int columns = mColumns == 0 ? (unsigned int)items.size() : mColumns;
		while (it != end) {

			float widthAccumulator = 0;
			unsigned int i = 0;
			float rowHeight = (float)mRowHeight;

			while (i < columns && it != end) 
			{
				Node2D * node = (Node2D*)(*it);
				node->SetUpNodeIfNeeded();
				v2f size = node->GetSize();

				//v2f pos = node->GetPosition();

				node->setArrayValue("Position", widthAccumulator, heightAccumulator);

				if (mResizeElements && size.x != 0 && size.y != 0) 
				{
					float allowedWidth = (float)mColumnWidth;
					if (i == 0 && r == 0 && mHeaderRow) allowedWidth = (float)mColumnWidth*(float)mColumns + (float)(padding_x*((int)mColumns - 1));

					float ratioW = allowedWidth > 0 ? (float)allowedWidth / size.x : 1.0f;
					float ratioH = mRowHeight > 0 ? (float)mRowHeight / size.y : 1.0f;

					float ratio = ratioW < ratioH ? ratioW : ratioH;

					if (ratio > 0.0 /*&& ratio<=1.0*/) 
					{
						size.x = size.x*ratio;
						size.y = size.y*ratio;

						v2f prescale = node->GetPreScale();

						if ((prescale.x != ratio) || (prescale.y != ratio))
						{
							node->setValue("PreScale", v2f(ratio,ratio));
						}
					}
				}
				else 
				{
					v2f prescale = node->GetPreScale();

					if ((prescale.x != 1.0f) || (prescale.y != 1.0f))
						node->setValue("PreScale", v2f(1.0f,1.0f));
				}

				// Add padding
				if ((i<(columns - 1)) && (i < nb_items))
					widthAccumulator += padding_x;


				if (mRowHeight == 0 && size.y > rowHeight) rowHeight = size.y;


				// Find the real width and height of the current grid
				unsigned int width = widthAccumulator + (size.x > mColumnWidth ? size.x : mColumnWidth);
				unsigned int height = heightAccumulator + (size.y > mRowHeight ? size.y : mRowHeight);

				// Increment wdith accumulator fot he current row
				if (mColumnWidth > 0)
					widthAccumulator += mColumnWidth;
				else
					widthAccumulator += size.x;


				if ((float)width > maxWidth) maxWidth = (float)width;
				if ((float)height > maxHeight) maxHeight = (float)height;

				if (i == 0 && r == 0 && mHeaderRow) i = columns;
				i++;
				it++;
			}

			heightAccumulator += padding_y + rowHeight;
			r++;
		}

		if (mAutoResize) 
		{
			mSize = v2f(maxWidth,maxHeight);
		}
	}

}