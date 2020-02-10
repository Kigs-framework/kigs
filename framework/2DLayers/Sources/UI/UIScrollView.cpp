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
	UIPanel(name, PASS_CLASS_NAME_TREE_ARG)
{
	xScroll = 0;
	yScroll = 0;

	myVision = KigsCore::GetInstanceOf(getName() + "_MYUIControlBoxForScrollViewUI", "UIControlBoxForScrollViewUI");
	myVision->Init();
	addItem((CMSP&)myVision);
}

void UIScrollView::InitModifiable()
{
	UIItem::InitModifiable();
	if (_isInit)
	{
		UpdateContentSize();

		Point2D posViewPort(myPosition[0], myPosition[1]);
		myVision->Set_Position(posViewPort);
		myVision->setValue("SizeX", mySizeX);
		myVision->setValue("SizeY", mySizeY);

		kstl::set<CoreModifiable*>	instances;
		GetInstances("ModuleInput", instances);
		KIGS_ASSERT(instances.size() == 1);
		myInput = (ModuleInput*)(*instances.begin());

		return;
	}
	UIItem::UninitModifiable();
}

UIScrollView::~UIScrollView()
{
	myVision = nullptr;
}


bool UIScrollView::scrollTo(kfloat deltaPos)
{
	bool stopped = false;

	if (isVertical)
	{
		if (yScroll + deltaPos > 0)
		{
			deltaPos = 0 - yScroll;
			stopped = true;
		}
		else if ((float)(deltaPos + yScroll) < (float)(-maxY + (int)mySizeY))
		{
			deltaPos = (float)(-maxY + (int)mySizeY) - (float)(yScroll);
			stopped = true;

		}


		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mySons.begin();
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mySons.end();
		for (; itS != itE; ++itS)
		{
			if ((*itS) == myVision.get())
				continue;

			kfloat x, y;
			((Node2D*)(*itS))->GetPosition(x, y);
			((Node2D*)(*itS))->setArrayValue("Position", x, y + deltaPos);
		}

		yScroll += deltaPos;
	}
	else
	{
		if (xScroll + deltaPos < 0)
		{
			deltaPos = 0 - xScroll;
			stopped = true;
		}
		else if ((float)(deltaPos + xScroll) >(float)(-maxX + (int)mySizeX))
		{
			deltaPos = 0;
			stopped = true;

		}
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mySons.begin();
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mySons.end();
		while (itS != itE)
		{
			kfloat x, y;
			((Node2D*)(*itS))->GetPosition(x, y);
			((Node2D*)(*itS))->setArrayValue("Position", x + deltaPos, y);
			itS++;
		}
		xScroll += deltaPos;

	}
	return stopped;

}
void UIScrollView::SetUpNodeIfNeeded()
{
	UIPanel::SetUpNodeIfNeeded();

	UpdateContentSize();
}

bool UIScrollView::Draw(TravState* state)
{
	ModuleSpecificRenderer* renderer = (ModuleSpecificRenderer*)state->GetRenderer();
	kfloat screenW, screenH;

	kfloat positionX, positionY;
	GetGlobalPosition(positionX, positionY);

	renderer->getFirstRenderingScreen()->GetSize(screenW, screenH);

	UIItem* myFirstFather = (UIItem*)getFirstParent(LABEL_TO_ID(UIItem));
	v2f fsize = myFirstFather->GetSize();
	float ratioH = screenH / fsize.y;
	float ratioW = screenW / fsize.x;

	renderer->SetScissorValue((positionX)*ratioW, (int)(screenH - (positionY + myRealSize.y)*ratioH), myRealSize.x*ratioW, myRealSize.y*ratioH);
	return UIPanel::Draw(state);
}

void UIScrollView::UpdateContentSize()
{
	maxY = 0;
	maxX = 0;
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mySons.begin();
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mySons.end();
	while (itS != itE)
	{
		kfloat x, y;
		((Node2D*)(*itS))->GetGlobalPosition(x, y);
		v2f size = ((Node2D*)(*itS))->GetSize();
		if (size.y + y > maxY)
		{
			maxY = size.y + y;
		}
		if (size.x + x > maxX)
		{
			maxX = size.x + x;
		}
		itS++;
	}
}

/*void UIScrollView::SendClickUpToChildren(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)
{
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mySons.begin();
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mySons.end();
	while (itS != itE)
	{
		if (*itS == myVision.get())
		{
			itS++;
			continue;
		}
		if (((UIItem*)(*itS))->ContainsPoint(X,Y))
		{
			((UIItem*)(*itS))->TriggerMouseClick(buttonState, buttonEvent, X, Y, catchClick);
		}
		itS++;
	}
}*/

void UIScrollView::SendFalseClickUpToChildren()
{
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mySons.begin();
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mySons.end();
	while (itS != itE)
	{
		if ((*itS)->isSubType(UIButton::myClassID))
		{
			((UIButton*)(*itS))->ForcedChangeState(false);
		}
		itS++;
	}
}

void UIScrollView::SendClickDownToChildren(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)
{
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mySons.begin();
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mySons.end();
	while (itS != itE)
	{
		if (*itS == myVision.get())
		{
			itS++;
			continue;
		}
		if (((UIItem*)(*itS))->ContainsPoint(X, Y) && (*itS)->isSubType(UIButton::myClassID))
		{
			((UIButton*)(*itS))->ForcedChangeState(buttonState&buttonEvent&UIInputEvent::LEFT);
		}
		itS++;
	}
}

/*void UIScrollView::SendMouseMoveToChildren(bool over,float MouseDeltaX, float MouseDeltaY)
{
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itS = mySons.begin();
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator itE = mySons.end();
	kfloat x, y;
	myInput->GetMouse()->getPos(x, y);
	getRootLayerFather()->GetRenderingScreen()->GetMousePosInScreen(x, y, x, y);
	while (itS != itE)
	{
		if ((*itS) == myVision.get())
		{
			itS++;
			continue;
		}
		if (((UIItem*)(*itS))->ContainsPoint(x,y))
		{
			((UIItem*)(*itS))->TriggerMouseMove(over, MouseDeltaX, MouseDeltaY);
		}
		else
		{
			((UIItem*)(*itS))->TriggerMouseMove(false, MouseDeltaX, MouseDeltaY);
		}
		itS++;
	}

}

*/

