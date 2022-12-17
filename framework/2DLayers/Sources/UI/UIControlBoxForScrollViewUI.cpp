#include "UI/UIScrollView.h"
#include "Timer.h"
#include "Base2DLayer.h"


IMPLEMENT_CLASS_INFO(UIControlBoxForScrollViewUI)

bool UIControlBoxForScrollViewUI::scrollTo(float deltaPos)
{
	float x, y;
	GetGlobalPosition(x, y);
	return ((UIScrollView*)mParent)->scrollTo(deltaPos);
}

void UIControlBoxForScrollViewUI::Update(const Timer& a_Timer, void* addParam)
{
	switch (mAnimation)
	{
	case ANIMATIONSTATE_INITIALIZATION:
	{
		mStartTime = a_Timer.GetTime();
		static const float k = 4000.f;
		mConstantAcceleration = -k;
		mEndTime = fabs(mInitialVelocity) / k;
		mAnimation = ANIMATIONSTATE_RUNNING;
	}
	break;
	case ANIMATIONSTATE_RUNNING:
	{
		double t = a_Timer.GetTime() - mStartTime;

		if (t > mEndTime)
		{
			double deltaPos = (0.5*(double)mConstantAcceleration*mEndTime*mEndTime + (double)mInitialVelocity*mEndTime);
			scrollTo((float)deltaPos);
			mAnimation = ANIMATIONSTATE_STOPPED;
		}
		else
		{
			double deltaPos = 0.5f*(double)mConstantAcceleration*t*t + (double)mInitialVelocity*t;

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
