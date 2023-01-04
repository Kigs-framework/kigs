#include "UI/UIScrollable.h"
#include "TouchInputEventManager.h"
#include "ModuleInput.h"
#include "Timer.h"

#include <cmath>

using namespace Kigs::Draw2D;

IMPLEMENT_CLASS_INFO(UIScrollable)

void UIScrollable::InitModifiable()
{
	ParentClassType::InitModifiable();
	if (IsInit())
	{
		auto ev = (Input::TouchEventStateScroll*)KigsCore::GetModule<Input::ModuleInput>()->getTouchManager()->registerEvent(this, "ManageScrollEvent", Input::Scroll, Input::EmptyFlag);
		(void)ev;
	}
}

bool UIScrollable::addItem(const CMSP& item, ItemPosition pos)
{
	if (mAdjustOnAddItem && item->isSubType("Node2D"))
	{
		if (mSons.find((Node2D*)item.get()) == mSons.end())
		{
			v2f prev;
			item->getArrayValue("Position", &prev.x, 2);
			prev += mCurrentScroll;
			item->setArrayValue("Position", &prev.x, 2);
		}
	}
	return ParentClassType::addItem(item, pos);
}

bool UIScrollable::removeItem(const CMSP& item)
{
	if (mAdjustOnRemoveItem && item->isSubType("Node2D"))
	{
		if (mSons.find((Node2D*)item.get()) != mSons.end())
		{
			v2f prev;
			item->getArrayValue("Position", &prev.x, 2);
			prev -= mCurrentScroll;
			item->setArrayValue("Position", &prev.x, 2);
		}
	}
	return ParentClassType::removeItem(item);
}

void UIScrollable::SetScroll(v2f to)
{
	v2f delta = to - mCurrentScroll;

	v2f extra = { 0,0 };

	v2f minscroll = mMinScroll;
	v2f maxscroll = mMaxScroll;
	
	v2f size = GetSize();
	if (mMinScrollModeX == 0)
		minscroll.x = -FLT_MAX;
	else if (mMinScrollModeX == 2)
		minscroll.x *= size.x;
	else if (mMinScrollModeX == 3)
		minscroll.x += size.x;

	if (mMinScrollModeY == 0)
		minscroll.y = -FLT_MAX;
	else if (mMinScrollModeY == 2)
		minscroll.y *= size.y;
	else if (mMinScrollModeY == 3)
		minscroll.y += size.y;

	if (mMaxScrollModeX == 0)
		maxscroll.x = FLT_MAX;
	else if (mMaxScrollModeX == 2)
		maxscroll.x *= size.x;
	else if (mMaxScrollModeX == 3)
		maxscroll.x += size.x;

	if (mMaxScrollModeY == 0)
		maxscroll.y = FLT_MAX;
	else if (mMaxScrollModeY == 2)
		maxscroll.y *= size.y;
	else if (mMaxScrollModeY == 3)
		maxscroll.y += size.y;


	if (maxscroll.x < minscroll.x)
		maxscroll.x = minscroll.x;
	if (maxscroll.y < minscroll.y)
		maxscroll.y = minscroll.y;


	if (to.x > maxscroll.x)
		extra.x = maxscroll.x - to.x;
	else if (to.x < minscroll.x)
		extra.x = minscroll.x - to.x;

	if (to.y > maxscroll.y)
		extra.y = maxscroll.y - to.y;
	else if (to.y < minscroll.y)
		extra.y = minscroll.y - to.y;

	delta += extra;
	mCurrentScroll = to + extra;

	for (auto son : mSons)
	{
		v2f prev;
		son->getArrayValue("Position", &prev.x, 2);
		prev += delta;
		son->setArrayValue("Position", &prev.x, 2);
	}
}

bool UIScrollable::ManageScrollEvent(Input::ScrollEvent& scroll_event)
{
	if (scroll_event.state == Input::StatePossible)
	{
		if (CanInteract(scroll_event.position.xy))
		{
			//*scroll_event.swallow_mask = 0xFFFFFFFF;
			return true;
		}
		return false;
	}

	if (scroll_event.state == Input::StateBegan)
	{
		mLastOffset = { 0, 0 };
		mThrowSpeed = { 0, 0 };
		//*scroll_event.swallow_mask = 0xFFFFFFFF;
		scroll_event.capture_inputs = true;
	}

	if (scroll_event.state == Input::StateChanged)
	{
		//v2f currentOffset = maindir.xy*offset;
		v2f currentOffset = scroll_event.delta.xy / (scroll_event.interaction ? 8.0f : 1.0f);

		if (!mVerticalScroll)
			currentOffset.y = 0;
		if (!mHorizontalScroll)
			currentOffset.x = 0;

		v2f delta = currentOffset - mLastOffset;
		mLastOffset = currentOffset;

		SetScroll(mCurrentScroll + delta);
		//*scroll_event.swallow_mask = 0xFFFFFFFF;
		scroll_event.capture_inputs = true;
	}

	if (scroll_event.state == Input::StateEnded)
	{
		mThrowSpeed = mThrowSpeedMult*scroll_event.speed.xy;
		if (!mVerticalScroll)
			mThrowSpeed.y = 0;
		if (!mHorizontalScroll)
			mThrowSpeed.x = 0;
		//*scroll_event.swallow_mask = 0xFFFFFFFF;
	}
	return true;
}

void UIScrollable::Update(const Time::Timer& timer, void* addParam)
{
	ParentClassType::Update(timer, addParam);

	auto time = timer.GetTime();
	if (mLastTime == 0) mLastTime = time;
	auto dt = time - mLastTime;

	if(mThrowSpeed.x != 0.0f || mThrowSpeed.y != 0.0f)
		SetScroll(mCurrentScroll + mThrowSpeed*(float)dt);

	double friction = mThrowFriction;

	mThrowSpeed.x *= (float)std::pow(friction, dt);
	mThrowSpeed.y *= (float)std::pow(friction, dt);
	
	mLastTime = time;
}