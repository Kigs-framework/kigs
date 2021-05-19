#include "UI/UIScrollView.h"
#include "Core.h"
#include "Timer.h"
#include "NotificationCenter.h"
#include <stdlib.h>
#include "ModuleRenderer.h"
#include "UI/UIButton.h"
#include "Base2DLayer.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIScrollView, UIScrollView, 2DLayers);
IMPLEMENT_CLASS_INFO(UIScrollView)

UIScrollView::UIScrollView(const kstl::string& name, CLASS_NAME_TREE_ARG) :
	UIDrawableItem(name, PASS_CLASS_NAME_TREE_ARG)
{
	mXScroll = 0;
	mYScroll = 0;

	mVision = KigsCore::GetInstanceOf(getName() + "_MYUIControlBoxForScrollViewUI", "UIControlBoxForScrollViewUI");
	mVision->Init();
	addItem((CMSP&)mVision);
}

void UIScrollView::InitModifiable()
{
	UIItem::InitModifiable();
	if (_isInit)
	{
		UpdateContentSize();

		Point2D posViewPort(mPosition[0], mPosition[1]);
		mVision->Set_Position(posViewPort);
		mVision->setValue("Size", (v2f)mSize);

		kstl::vector<CMSP>	instances=	GetInstances("ModuleInput");
		KIGS_ASSERT(instances.size() == 1);
		mInput = (ModuleInput*)(instances[0].get());

		return;
	}
	UIItem::UninitModifiable();
}

UIScrollView::~UIScrollView()
{
	mVision = nullptr;
}


bool UIScrollView::scrollTo(kfloat deltaPos)
{
	bool stopped = false;

	if (isVertical)
	{
		if (mYScroll + deltaPos > 0)
		{
			deltaPos = 0 - mYScroll;
			stopped = true;
		}
		else if ((float)(deltaPos + mYScroll) < (float)(-mMaxY + (int)mSize[1]))
		{
			deltaPos = (float)(-mMaxY + (int)mSize[1]) - (float)(mYScroll);
			stopped = true;

		}


		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mSons.begin();
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mSons.end();
		for (; itS != itE; ++itS)
		{
			if ((*itS) == mVision.get())
				continue;

			kfloat x, y;
			((Node2D*)(*itS))->GetPosition(x, y);
			((Node2D*)(*itS))->setArrayValue("Position", x, y + deltaPos);
		}

		mYScroll += deltaPos;
	}
	else
	{
		if (mXScroll + deltaPos < 0)
		{
			deltaPos = 0 - mXScroll;
			stopped = true;
		}
		else if ((float)(deltaPos + mXScroll) >(float)(-mMaxX + (int)mSize[0]))
		{
			deltaPos = 0;
			stopped = true;

		}
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mSons.begin();
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mSons.end();
		while (itS != itE)
		{
			kfloat x, y;
			((Node2D*)(*itS))->GetPosition(x, y);
			((Node2D*)(*itS))->setArrayValue("Position", x + deltaPos, y);
			itS++;
		}
		mXScroll += deltaPos;

	}
	return stopped;

}
void UIScrollView::SetUpNodeIfNeeded()
{
	UIDrawableItem::SetUpNodeIfNeeded();

	UpdateContentSize();
}

bool UIScrollView::Draw(TravState* state)
{
	ModuleSpecificRenderer* renderer = (ModuleSpecificRenderer*)state->GetRenderer();
	kfloat screenW, screenH;

	kfloat positionX, positionY;
	GetGlobalPosition(positionX, positionY);

	renderer->getFirstRenderingScreen()->GetSize(screenW, screenH);

	UIItem* firstFather = (UIItem*)getFirstParent(LABEL_TO_ID(UIItem));
	v2f fsize = firstFather->GetSize();
	float ratioH = screenH / fsize.y;
	float ratioW = screenW / fsize.x;

	renderer->SetScissorValue((positionX)*ratioW, (int)(screenH - (positionY + mRealSize.y)*ratioH), mRealSize.x*ratioW, mRealSize.y*ratioH);
	return UIDrawableItem::Draw(state);
}

void UIScrollView::UpdateContentSize()
{
	mMaxY = 0;
	mMaxX = 0;
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mSons.begin();
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mSons.end();
	while (itS != itE)
	{
		kfloat x, y;
		((Node2D*)(*itS))->GetGlobalPosition(x, y);
		v2f size = ((Node2D*)(*itS))->GetSize();
		if (size.y + y > mMaxY)
		{
			mMaxY = size.y + y;
		}
		if (size.x + x > mMaxX)
		{
			mMaxX = size.x + x;
		}
		itS++;
	}
}


void UIScrollView::SendFalseClickUpToChildren()
{
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mSons.begin();
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mSons.end();
	while (itS != itE)
	{
		if ((*itS)->isSubType(UIButton::mClassID))
		{
			((UIButton*)(*itS))->ForcedChangeState(false);
		}
		itS++;
	}
}

void UIScrollView::SendClickDownToChildren(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)
{
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mSons.begin();
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mSons.end();
	while (itS != itE)
	{
		if (*itS == mVision.get())
		{
			itS++;
			continue;
		}
		if (((UIItem*)(*itS))->ContainsPoint(X, Y) && (*itS)->isSubType(UIButton::mClassID))
		{
			((UIButton*)(*itS))->ForcedChangeState(buttonState&buttonEvent&UIInputEvent::LEFT);
		}
		itS++;
	}
}


