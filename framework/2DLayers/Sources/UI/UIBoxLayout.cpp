#include "UI/UIBoxLayout.h"
#include "RenderingScreen.h"

using namespace Kigs::Draw2D;

IMPLEMENT_CLASS_INFO(UIBoxLayout)

IMPLEMENT_CONSTRUCTOR(UIBoxLayout)
{
	setOwnerNotification("Padding", true);
	setOwnerNotification("SortByPriority", true);
	setOwnerNotification("Vertical", true);
	setOwnerNotification("Alignment", true);
	setOwnerNotification("ResizeLayoutX", true);
	setOwnerNotification("ResizeLayoutY", true);
}

void UIBoxLayout::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == KigsID("SortByPriority")._id ) ||
		(labelid == KigsID("Vertical")._id ) ||
		(labelid == KigsID("Alignment")._id) ||
		(labelid == KigsID("Padding")._id ) ||
		(labelid == KigsID("ResizeLayoutX")._id) ||
		(labelid == KigsID("ResizeLayoutY")._id))
	{
		mNeedRecompute = true;
	}
	UILayout::NotifyUpdate(labelid);
}

void UIBoxLayout::RecomputeLayout()
{

	if (!GetSons().empty())
	{

		std::vector<CoreModifiable*> items;

		if (!mSortByPriority)
		{
			// Get sons ordered by insertion
			const std::vector<ModifiableItemStruct> sons = getItems();
			std::vector<ModifiableItemStruct>::const_iterator itsons;
			for (itsons = sons.begin(); itsons != sons.end(); itsons++)
			{
				if ((*itsons).mItem->isSubType(Node2D::mClassID))
				{
					SP<Node2D> node = itsons->mItem;
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
			std::set<Node2D*, Node2D::PriorityCompare> sons = GetSons();
			std::set<Node2D*, Node2D::PriorityCompare>::reverse_iterator itsons = sons.rbegin();
			std::set<Node2D*, Node2D::PriorityCompare>::reverse_iterator itend = sons.rend();
			while (itsons != itend)
			{
				bool is_enabled = false;
				(*itsons)->getValue("IsHidden", is_enabled);
				if (!is_enabled)
					items.push_back(*itsons);
				itsons++;
			}
		}


		std::vector<CoreModifiable*>::iterator it = items.begin();
		std::vector<CoreModifiable*>::iterator end = items.end();

		//int size = items.size();

		unsigned int accumulator = 0;

		int padding_x = mPadding[0];
		int padding_y = mPadding[1];

		float sx, sy, cx, cy;

		sx = mRealSize.x;
		sy = mRealSize.y;

		cx = sx / 2;
		cy = sy / 2;

		float biggest = 0;

		while (it != end)
		{
			Node2D* node = (Node2D*)*it;
			node->SetUpNodeIfNeeded();
			v2f size = node->GetSize();

			int px, py;
			if (mVertical)
			{
				biggest = std::max(biggest, size.x);
				py = accumulator;
				px = 0;

				if (mAlignment == 1)
					px = (s32)(cx - size.x *0.5f);
				else if (mAlignment == 2)
					px = (s32)(sx - size.x);
			}
			else
			{
				biggest = std::max(biggest, size.y);
				px = accumulator;
				py = 0;
				if (mAlignment == 1)
					py = (s32)(cy - size.y *0.5f);
				else if (mAlignment == 2)
					py = (s32)(sy - size.y);
			}
			node->setArrayValue("Position", px, py);

			if (mVertical)
				accumulator += (u32)(size.y + padding_y);
			else
				accumulator += (u32)(size.x + padding_x);
			it++;
		}


		if (mVertical)
		{
			if (mResizeLayoutX && mSizeModeX == 0)
			{
				mRealSize.x = biggest;
				if (mSize[0] != biggest)
					setValue("Size", v2f(biggest, mSize[1]));
			}

			if (mResizeLayoutY && mSizeModeY == 0)
			{
				mRealSize.y = (float)accumulator;
				if (mSize[1] != (float)accumulator)
					setValue("Size", v2f(mSize[0],(float)accumulator));
			}
		}
		else
		{
			if (mResizeLayoutY && mSizeModeY == 0)
			{
				mRealSize.y = biggest;
				if (mSize[1] != biggest)
					setValue("Size", v2f(mSize[0],biggest));
			}
			if (mResizeLayoutX && mSizeModeX == 0)
			{
				mRealSize.x = (float)accumulator;
				if (mSize[0]!= (float)accumulator)
					setValue("Size", v2f((float)accumulator, mSize[1]));
			}
		}

	}
}