#include "UI/UIGridLayout.h"
#include "Base2DLayer.h"
#include "RenderingScreen.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIGridLayout, UIGridLayout, 2DLayers);
IMPLEMENT_CLASS_INFO(UIGridLayout)

UIGridLayout::UIGridLayout(const kstl::string& name, CLASS_NAME_TREE_ARG) :
	UILayout(name, PASS_CLASS_NAME_TREE_ARG)
	, mColumns(*this, false, LABEL_AND_ID(Columns), 0)
	, mRowHeight(*this, false, LABEL_AND_ID(RowHeight), 0)
	, mColumnWidth(*this, false, LABEL_AND_ID(ColumnWidth), 0)
	, mHeaderRow(*this, false, LABEL_AND_ID(HeaderRow), false)
	, mPadding(*this, false, LABEL_AND_ID(Padding), 0, 0)
	, mSortByPriority(*this, false, LABEL_AND_ID(SortByPriority), false)
	, mResizeElements(*this, false, LABEL_AND_ID(ResizeElements), false)
	, mAutoResize(*this, false, LABEL_AND_ID(AutoResize), true)
{
	mColumns.changeNotificationLevel(Owner);
	mPadding.changeNotificationLevel(Owner);
	mHeaderRow.changeNotificationLevel(Owner);
	mRowHeight.changeNotificationLevel(Owner);
	mColumnWidth.changeNotificationLevel(Owner);
	mSortByPriority.changeNotificationLevel(Owner);
	mResizeElements.changeNotificationLevel(Owner);
	mAutoResize.changeNotificationLevel(Owner);
}

void UIGridLayout::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == mColumns.getLabelID()) ||
		(labelid == mHeaderRow.getLabelID()) ||
		(labelid == mRowHeight.getLabelID()) ||
		(labelid == mColumnWidth.getLabelID()) ||
		(labelid == mSortByPriority.getLabelID()) ||
		(labelid == mResizeElements.getLabelID()) ||
		(labelid == mAutoResize.getLabelID()) ||
		(labelid == mPadding.getLabelID()))
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


		kstl::vector<CoreModifiable*> items;

		if (!mSortByPriority) {
			// Get sons ordered by insertion
			const kstl::vector<ModifiableItemStruct> sons = getItems();
			kstl::vector<ModifiableItemStruct>::const_iterator itsons;
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
			kstl::set<Node2D*, Node2D::PriorityCompare> sons = GetSons();
			kstl::set<Node2D*, Node2D::PriorityCompare>::reverse_iterator itsons = sons.rbegin();
			kstl::set<Node2D*, Node2D::PriorityCompare>::reverse_iterator itend = sons.rend();
			while (itsons != itend) {
				items.push_back(*itsons);
				itsons++;
			}
		}


		kstl::vector<CoreModifiable*>::iterator it = items.begin();
		kstl::vector<CoreModifiable*>::iterator end = items.end();

		auto nb_items = items.size();

		float heightAccumulator = 0;
		float maxWidth = 0;
		float maxHeight = 0;


		int r = 0;
		unsigned int columns = mColumns == 0 ? items.size() : mColumns;
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