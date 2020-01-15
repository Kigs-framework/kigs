#include "UI/UIBoxLayout.h"
#include "Base2DLayer.h"
#include "RenderingScreen.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIBoxLayout, UIBoxLayout, 2DLayers);;
IMPLEMENT_CLASS_INFO(UIBoxLayout)

IMPLEMENT_CONSTRUCTOR(UIBoxLayout)
{
	myPadding.changeNotificationLevel(Owner);
	mySortByPriority.changeNotificationLevel(Owner);
	myVertical.changeNotificationLevel(Owner);
	myAlignment.changeNotificationLevel(Owner);
	myResizeLayoutX.changeNotificationLevel(Owner);
	myResizeLayoutY.changeNotificationLevel(Owner);
}

void UIBoxLayout::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == mySortByPriority.getLabelID()) ||
		(labelid == myVertical.getLabelID()) ||
		(labelid == myAlignment.getLabelID()) ||
		(labelid == myPadding.getLabelID()) ||
		(labelid == myResizeLayoutX.getLabelID()) ||
		(labelid == myResizeLayoutY.getLabelID()))
	{
		myNeedRecompute = true;
	}
	UILayout::NotifyUpdate(labelid);
}

void UIBoxLayout::RecomputeLayout()
{

	if (!GetSons().empty())
	{

		kstl::vector<CoreModifiable*> items;

		if (!mySortByPriority)
		{
			// Get sons ordered by insertion
			const kstl::vector<ModifiableItemStruct> sons = getItems();
			kstl::vector<ModifiableItemStruct>::const_iterator itsons;
			for (itsons = sons.begin(); itsons != sons.end(); itsons++)
			{
				if ((*itsons).myItem->isSubType(Node2D::myClassID))
				{
					auto node = (SP<Node2D>&)(itsons->myItem);
					bool is_enabled = false;
					node->getValue("IsHidden", is_enabled);
					if (!is_enabled)
						items.push_back(node.get());
				}
			}
		}
		else
		{
			// Get sons ordered by priority
			kstl::set<Node2D*, Node2D::PriorityCompare> sons = GetSons();
			kstl::set<Node2D*, Node2D::PriorityCompare>::reverse_iterator itsons = sons.rbegin();
			kstl::set<Node2D*, Node2D::PriorityCompare>::reverse_iterator itend = sons.rend();
			while (itsons != itend)
			{
				bool is_enabled = false;
				(*itsons)->getValue("IsHidden", is_enabled);
				if (!is_enabled)
					items.push_back(*itsons);
				itsons++;
			}
		}


		kstl::vector<CoreModifiable*>::iterator it = items.begin();
		kstl::vector<CoreModifiable*>::iterator end = items.end();

		//int size = items.size();

		unsigned int accumulator = 0;

		int padding_x = myPadding[0];
		int padding_y = myPadding[1];

		float sx, sy, cx, cy;

		sx = myRealSize.x;
		sy = myRealSize.y;

		cx = sx / 2;
		cy = sy / 2;

		float biggest = 0;

		while (it != end)
		{
			Node2D* node = (Node2D*)*it;
			node->ComputeRealSize();
			v2f size = node->GetSize();

			int px, py;
			if (myVertical)
			{
				biggest = std::max(biggest, size.x);
				py = accumulator;
				px = 0;

				if (myAlignment == 1)
					px = (s32)(cx - size.x *0.5f);
				else if (myAlignment == 2)
					px = (s32)(sx - size.x);
			}
			else
			{
				biggest = std::max(biggest, size.y);
				px = accumulator;
				py = 0;
				if (myAlignment == 1)
					py = (s32)(cy - size.y *0.5f);
				else if (myAlignment == 2)
					py = (s32)(sy - size.y);
			}
			node->setArrayValue("Position", px, py);

			if (myVertical)
				accumulator += (u32)(size.y + padding_y);
			else
				accumulator += (u32)(size.x + padding_x);
			it++;
		}


		if (myVertical)
		{
			if (myResizeLayoutX && mySizeModeX == 0)
				//if (mySizeX == 0)
			{
				myRealSize.x = biggest;
				if (mySizeX != biggest)
					setValue("SizeX", biggest);
			}

			if (myResizeLayoutY && mySizeModeY == 0)
			{
				myRealSize.y = (float)accumulator;
				if (mySizeY != (float)accumulator)
					setValue("SizeY", (float)accumulator);
			}
		}
		else
		{
			if (myResizeLayoutY && mySizeModeY == 0)
			{
				myRealSize.y = biggest;
				if (mySizeY != biggest)
					setValue("SizeY", biggest);
			}
			if (myResizeLayoutX && mySizeModeX == 0)
			{
				myRealSize.x = (float)accumulator;
				if (mySizeX != (float)accumulator)
					setValue("SizeX", (float)accumulator);
			}
		}

	}
}