#include "UI/UIScrollView.h"
#include "Timer.h"
#include "Base2DLayer.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIControlBoxForScrollViewUI, UIControlBoxForScrollViewUI, 2DLayers);
IMPLEMENT_CLASS_INFO(UIControlBoxForScrollViewUI)

bool UIControlBoxForScrollViewUI::scrollTo(kfloat deltaPos)
{
	kfloat x, y;
	GetGlobalPosition(x, y);
	return ((UIScrollView*)myParent)->scrollTo(deltaPos);
}

bool UIControlBoxForScrollViewUI::TriggerMouseMove(bool over, float MouseDeltaX, float MouseDeltaY)
{
	if (mMoving)
	{
		hasMoved = true;
		//((UIScrollView*)myParent)->SendFalseClickUpToChildren();
		if (fabs(MouseDeltaY) >= fabs(MouseDeltaX))
		{
			((UIScrollView*)myParent)->isVertical = true;
			scrollTo(MouseDeltaY);
		}
		else
		{
			((UIScrollView*)myParent)->isVertical = false;
			scrollTo(MouseDeltaX);
		}
		return true;
	}
	hasMoved = false;
	((UIScrollView*)myParent)->SendMouseMoveToChildren(over,MouseDeltaX, MouseDeltaY);
	return false;
}



bool UIControlBoxForScrollViewUI::TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)
{
	if (buttonEvent&UIInputEvent::LEFT)
	{
		if (buttonState&UIInputEvent::LEFT)
		{
			mMoving = true;
			catchClick = true;
			((UIScrollView*)myParent)->SendClickDownToChildren(buttonState, buttonEvent, X, Y, catchClick);
		}
		else
		{
			catchClick = false;
			if (!hasMoved)
			{
				((UIScrollView*)myParent)->SendClickUpToChildren(buttonState, buttonEvent, X, Y, catchClick);
			}
			else
			{
				((UIScrollView*)myParent)->SendFalseClickUpToChildren();
			}
			mMoving = false;
		}
	}
	else
	{
		catchClick = false;
		return false;
	}
	return  true;
}

void UIControlBoxForScrollViewUI::Update(const Timer& a_Timer, void* addParam)
{
	switch (mAnimation)
	{
	case ANIMATIONSTATE_INITIALIZATION:
	{
		mStartTime = a_Timer.GetTime();
		static const kfloat k = 4000.f;
		myConstantAcceleration = -k;
		mEndTime = fabs(myInitialVelocity) / k;
		mAnimation = ANIMATIONSTATE_RUNNING;
	}
	break;
	case ANIMATIONSTATE_RUNNING:
	{
		kdouble t = a_Timer.GetTime() - mStartTime;

		if (t > mEndTime)
		{
			double deltaPos = (0.5*(double)myConstantAcceleration*mEndTime*mEndTime + (double)myInitialVelocity*mEndTime);
			scrollTo((float)deltaPos);
			mAnimation = ANIMATIONSTATE_STOPPED;
		}
		else
		{
			double deltaPos = 0.5f*(double)myConstantAcceleration*t*t + (double)myInitialVelocity*t;

			if (scrollTo((float)deltaPos))
			{
				mAnimation = ANIMATIONSTATE_STOPPED;
			}
		}
	}

	break;
	case ANIMATIONSTATE_STOPPED:
	default:
		break;

	}
}

void UIControlBoxForScrollViewUI::TriggerMouseSwipe(int idxButton, kfloat Vx, kfloat Vy)
{
	if (fabsf(Vy) >= fabsf(Vx))
	{
		if (fabsf(Vy)>10.f)
		{
			((UIScrollView*)myParent)->isVertical = true;
			myInitialVelocity = Vy;
			mAnimation = ANIMATIONSTATE_INITIALIZATION;

		}
	}
	else if (fabsf(Vx)>10.f)
	{
		((UIScrollView*)myParent)->isVertical = false;
		myInitialVelocity = Vx;
		mAnimation = ANIMATIONSTATE_INITIALIZATION;
	}
	else
	{
		mAnimation = ANIMATIONSTATE_STOPPED;
	}
}

