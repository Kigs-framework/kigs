#include "UI/UIScrollView.h"
#include "Core.h"
#include "Timer.h"
#include "NotificationCenter.h"
#include <stdlib.h>
#include "ModuleRenderer.h"
#include "UI/UIButton.h"
#include "Abstract2DLayer.h"

using namespace Kigs::Draw2D;

IMPLEMENT_CLASS_INFO(UIScrollView)

UIScrollView::UIScrollView(const std::string& name, CLASS_NAME_TREE_ARG) :
	UIDrawableItem(name, PASS_CLASS_NAME_TREE_ARG)
{
	mXScroll = 0;
	mYScroll = 0;

	mVision = KigsCore::GetInstanceOf(getName() + "_MYUIControlBoxForScrollViewUI", "UIControlBoxForScrollViewUI");
	mVision->Init();
	addItem(mVision);
}

void UIScrollView::InitModifiable()
{
	UIItem::InitModifiable();
	if (_isInit)
	{
		UpdateContentSize();

		v2f posViewPort(mPosition[0], mPosition[1]);
		mVision->Set_Position(posViewPort);
		mVision->setValue("Size", (v2f)mSize);

		std::vector<CMSP>	instances=	GetInstances("ModuleInput");
		KIGS_ASSERT(instances.size() == 1);
		mInput = (Input::ModuleInput*)(instances[0].get());

		return;
	}
	UIItem::UninitModifiable();
}

UIScrollView::~UIScrollView()
{
	mVision = nullptr;
}


bool UIScrollView::scrollTo(float deltaPos)
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


		std::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mSons.begin();
		std::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mSons.end();
		for (; itS != itE; ++itS)
		{
			if ((*itS) == mVision.get())
				continue;

			float x, y;
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
		std::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mSons.begin();
		std::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mSons.end();
		while (itS != itE)
		{
			float x, y;
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

bool UIScrollView::Draw(Scene::TravState* state)
{
	ModuleSpecificRenderer* renderer = (ModuleSpecificRenderer*)state->GetRenderer();
	float screenW, screenH;

	float positionX, positionY;
	GetGlobalPosition(positionX, positionY);

	renderer->getFirstRenderingScreen()->GetSize(screenW, screenH);

	UIItem* firstFather = (UIItem*)getFirstParent("UIItem");
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
	std::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mSons.begin();
	std::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mSons.end();
	while (itS != itE)
	{
		float x, y;
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
	std::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mSons.begin();
	std::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mSons.end();
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
	std::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mSons.begin();
	std::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mSons.end();
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


