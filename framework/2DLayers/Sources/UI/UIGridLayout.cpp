#include "UI/UIGridLayout.h"
#include "Base2DLayer.h"
#include "RenderingScreen.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIGridLayout, UIGridLayout, 2DLayers);
IMPLEMENT_CLASS_INFO(UIGridLayout)

UIGridLayout::UIGridLayout(const kstl::string& name, CLASS_NAME_TREE_ARG) :
	UILayout(name, PASS_CLASS_NAME_TREE_ARG)
	//, myRows(*this, false, LABEL_AND_ID(Rows), 0)
	, myColumns(*this, false, LABEL_AND_ID(Columns), 0)
	, myRowHeight(*this, false, LABEL_AND_ID(RowHeight), 0)
	, myColumnWidth(*this, false, LABEL_AND_ID(ColumnWidth), 0)
	, myHeaderRow(*this, false, LABEL_AND_ID(HeaderRow), false)
	, myPadding(*this, false, LABEL_AND_ID(Padding), 0, 0)
	, mySortByPriority(*this, false, LABEL_AND_ID(SortByPriority), false)
	, myResizeElements(*this, false, LABEL_AND_ID(ResizeElements), false)
	, myResizeMe(*this, false, LABEL_AND_ID(AutoResize), true)
{
	//myRows.changeNotificationLevel(Owner);
	myColumns.changeNotificationLevel(Owner);
	myPadding.changeNotificationLevel(Owner);
	myHeaderRow.changeNotificationLevel(Owner);
	myRowHeight.changeNotificationLevel(Owner);
	myColumnWidth.changeNotificationLevel(Owner);
	mySortByPriority.changeNotificationLevel(Owner);
	myResizeElements.changeNotificationLevel(Owner);
	myResizeMe.changeNotificationLevel(Owner);
}

void UIGridLayout::NotifyUpdate(const unsigned int labelid)
{
	if (/*(labelid == myRows.getLabelID()) ||*/
		(labelid == myColumns.getLabelID()) ||
		(labelid == myHeaderRow.getLabelID()) ||
		(labelid == myRowHeight.getLabelID()) ||
		(labelid == myColumnWidth.getLabelID()) ||
		(labelid == mySortByPriority.getLabelID()) ||
		(labelid == myResizeElements.getLabelID()) ||
		(labelid == myResizeMe.getLabelID()) ||
		(labelid == myPadding.getLabelID()))
	{
		myNeedRecompute = true;
	}
	UILayout::NotifyUpdate(labelid);
}

void UIGridLayout::RecomputeLayout()
{
	if (!myNeedRecompute)
		return;

	if (!GetSons().empty())
	{

		int padding_x = myPadding[0];
		int padding_y = myPadding[1];


		kstl::vector<CoreModifiable*> items;

		if (!mySortByPriority) {
			// Get sons ordered by insertion
			const kstl::vector<ModifiableItemStruct> sons = getItems();
			kstl::vector<ModifiableItemStruct>::const_iterator itsons;
			for (itsons = sons.begin(); itsons != sons.end(); itsons++)
			{
				if ((*itsons).myItem->isSubType(Node2D::myClassID))
				{
					items.push_back((CoreModifiable*)(*itsons).myItem);
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
		unsigned int columns = myColumns == 0 ? items.size() : myColumns;
		while (it != end) {

			float widthAccumulator = 0;
			unsigned int i = 0;
			float rowHeight = (float)myRowHeight;

			while (i < columns && it != end) 
			{
				Node2D * node = (Node2D*)(*it);
				node->SetUpNodeIfNeeded();
				v2f size = node->GetSize();

				//v2f pos = node->GetPosition();

				node->setArrayValue("Position", widthAccumulator, heightAccumulator);

				if (myResizeElements && size.x != 0 && size.y != 0) 
				{
					float allowedWidth = (float)myColumnWidth;
					if (i == 0 && r == 0 && myHeaderRow) allowedWidth = (float)myColumnWidth*(float)myColumns + (float)(padding_x*((int)myColumns - 1));

					float ratioW = allowedWidth > 0 ? (float)allowedWidth / size.x : 1.0f;
					float ratioH = myRowHeight > 0 ? (float)myRowHeight / size.y : 1.0f;

					float ratio = ratioW < ratioH ? ratioW : ratioH;

					if (ratio > 0.0 /*&& ratio<=1.0*/) 
					{
						size.x = size.x*ratio;
						size.y = size.y*ratio;

						v2f prescale = node->GetPreScale();

						if (prescale.x != ratio)
							node->setValue("PreScaleX", ratio);
						if (prescale.y != ratio)
							node->setValue("PreScaleY", ratio);
					}
				}
				else 
				{
					v2f prescale = node->GetPreScale();

					if (prescale.x != 1.0f)
						node->setValue("PreScaleX", 1.0f);
					if (prescale.y != 1.0f)
						node->setValue("PreScaleY", 1.0f);
				}

				// Add padding
				if ((i<(columns - 1)) && (i < nb_items))
					widthAccumulator += padding_x;


				if (myRowHeight == 0 && size.y > rowHeight) rowHeight = size.y;


				// Find the real width and height of the current grid
				unsigned int width = widthAccumulator + (size.x > myColumnWidth ? size.x : myColumnWidth);
				unsigned int height = heightAccumulator + (size.y > myRowHeight ? size.y : myRowHeight);

				// Increment wdith accumulator fot he current row
				if (myColumnWidth > 0)
					widthAccumulator += myColumnWidth;
				else
					widthAccumulator += size.x;


				if ((float)width > maxWidth) maxWidth = (float)width;
				if ((float)height > maxHeight) maxHeight = (float)height;

				if (i == 0 && r == 0 && myHeaderRow) i = columns;
				i++;
				it++;
			}

			heightAccumulator += padding_y + rowHeight;
			r++;
		}

		if (myResizeMe) 
		{
			mySizeX = maxWidth;
			mySizeY = maxHeight;
		}
	}

}