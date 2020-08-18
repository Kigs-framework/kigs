#include "UI/UIRoundHUD.h"
#include "UIVerticesInfo.h"
#include "ModuleInput.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIRoundHUD, UIRoundHUD, 2DLayers);
IMPLEMENT_CLASS_INFO(UIRoundHUD)

IMPLEMENT_CONSTRUCTOR(UIRoundHUD)
{
}

UIRoundHUD::~UIRoundHUD()
{
}

void UIRoundHUD::InitModifiable()
{
	ParentClassType::InitModifiable();

	mColor.changeNotificationLevel(Owner);
	mOpacity.changeNotificationLevel(Owner);

	mAngleStart.changeNotificationLevel(Owner);
	mAngleWide.changeNotificationLevel(Owner);
	mSlotCount.changeNotificationLevel(Owner);
	mIsClockwise.changeNotificationLevel(Owner);
	mRadius.changeNotificationLevel(Owner);
	mRadiusOffset.changeNotificationLevel(Owner);

	mVoidZone = (mRadius + mRadiusOffset) *0.2f;
	mVoidZone *= mVoidZone;

	ModuleInput* theInputModule = (ModuleInput*)KigsCore::GetModule("ModuleInput");
	theInputModule->getTouchManager()->registerEvent(this, "ManageDirectTouchEvent", DirectTouch, EmptyFlag);
}


void UIRoundHUD::ProtectedDraw(TravState* state)
{
	v2f p = mRealSize*0.5f;
	TransformPoints(&p, 1);

	if (p.x != mMidPoint.x || p.y != mMidPoint.y)
	{
		mNeedUpdateSlots = true;
		mMidPoint = p;
	}

	if (mNeedUpdateSlots)
	{
		UpdateSlots();
		mNeedUpdateSlots = false;
	}

	ParentClassType::ProtectedDraw(state);
}

bool UIRoundHUD::addItem(const CMSP& item, ItemPosition pos)
{
	mNeedUpdateSlots = true;
	return ParentClassType::addItem(item, pos);
}

bool UIRoundHUD::removeItem(const CMSP& item)
{
	mNeedUpdateSlots = true;
	return ParentClassType::removeItem(item);
}

/*
maFloat myAngleStart = BASE_ATTRIBUTE(AngleStart, 0.0f);
maFloat myAngleWide = BASE_ATTRIBUTE(AngleWide, 360.0f);
maInt mySlotCount = BASE_ATTRIBUTE(SlotCount, -1); // -1 for dynamic
maBool myIsClockWise = BASE_ATTRIBUTE(IsClockwise, true);
*/
void UIRoundHUD::NotifyUpdate(const unsigned int labelid)
{
	ParentClassType::NotifyUpdate(labelid);

	if (
		labelid == mColor.getLabelID() ||
		labelid == mOpacity.getLabelID() ||
		labelid == mRotationAngle.getLabelID() ||
		labelid == mAngleStart.getLabelID() ||
		labelid == mAngleWide.getLabelID() ||
		labelid == mSlotCount.getLabelID() ||
		labelid == mIsClockwise.getLabelID() ||
		labelid == mRadiusOffset.getLabelID() ||
		labelid == mRadius.getLabelID())
		mNeedUpdateSlots = true;
}

#define DEBUG_DRAW 1
#define CircleDiv 50

void UIRoundHUD::UpdateSlots()
{
	auto generateCircle = [this](std::vector<v2f> &points, float radius, int divCount, float angleWide = 360.0f, float angleStart = 0.0f)
	{
		if (divCount == 0)
		{
			float radians = deg2rad(angleStart);
			points.push_back({ sinf(radians)*radius,cosf(radians)*radius });
		}
		else
		{
			float ratio = (angleWide / 360.0f) *(fPI * 2.0f) / divCount;
			for (int i = 0; i < divCount+1; ++i)
			{
				float radians = deg2rad(angleStart) + ratio * (float)i;
				points.push_back({ sinf(radians)*radius,cosf(radians)*radius });
			}
		}
	};

	int idx = 0;

	// get slot count (fixed count or dynamic)
	mRealSlotCount = (mSlotCount >= 0) ? mSlotCount : GetSons().size();
	
	// create slot position array
	mSlot.reserve(mRealSlotCount + 2);
	generateCircle(mSlot, mRadius, (mAngleWide == 360.0f) ? mRealSlotCount : mRealSlotCount - 1, mAngleWide, mAngleStart);

#if DEBUG_DRAW>0

#if DEBUG_DRAW==0
#else
	// recreate vertices infos
	VInfo2D vi;
#if DEBUG_DRAW==1
	mVI = UIVerticesInfo(&vi, CircleDiv * 3);
#elif DEBUG_DRAW==2
	mVI = UIVerticesInfo(&vi, (mySelectedSlot >= 0)?CircleDiv * 3 * 2: CircleDiv * 3);
#elif DEBUG_DRAW==3
	mVI = UIVerticesInfo(&vi, CircleDiv * 3 * (myRealSlotCount + 1));
#endif
	mVI.Flag |= UIVerticesInfo_Vertex;
	mVI.Flag |= UIVerticesInfo_Color;
#endif

	std::vector<v2f> points;
	points.reserve(CircleDiv + 1);
	generateCircle(points, mRadius+ mRadiusOffset, CircleDiv,mAngleWide,mAngleStart);

	v4f color(mColor[0], mColor[1], mColor[2], GetOpacity());
	color *= 255;

	auto data = reinterpret_cast<VInfo2D::Data*>(mVI.Buffer());
	for (int i = 0; i < CircleDiv; ++i)
	{
		data[idx++].setValue(mMidPoint.x, mMidPoint.y, color.x, color.y, color.z, color.w, 0, 0);
		data[idx++].setValue(mMidPoint.x + points[i].x, mMidPoint.y + points[i].y, color.x, color.y, color.z, color.w, 0, 0);
		data[idx++].setValue(mMidPoint.x + points[i + 1].x, mMidPoint.y + points[i + 1].y, color.x, color.y, color.z, color.w, 0, 0);
	}

#if DEBUG_DRAW==2
	if (mySelectedSlot >= 0)
	{
		generateCircle(points, myRadius*0.1f, CircleDiv);

		auto data = reinterpret_cast<VInfo2D::Data*>(mVI.Buffer());
		v2f center = myMidPoint + mySlot[mySelectedSlot];
		for (int i = 0; i < CircleDiv; ++i)
		{
			data[idx++].setValue(center.x, center.y, 0, 0, 0, color.w, 0, 0);
			data[idx++].setValue(center.x + points[i].x, center.y + points[i].y, 0, 0, 0, color.w, 0, 0);
			data[idx++].setValue(center.x + points[i + 1].x, center.y + points[i + 1].y, 0, 0, 0, color.w, 0, 0);
		}
	}
#elif DEBUG_DRAW==3
	for (int slot = 0; slot < myRealSlotCount; slot++)
	{
		generateCircle(points, myRadius*0.1f, CircleDiv);

		v2f center = myMidPoint + mySlot[slot];
		auto data = reinterpret_cast<VInfo2D::Data*>(mVI.Buffer());
		for (int i = 0; i < CircleDiv; ++i)
		{
			data[idx++].setValue(center.x, center.y, 0, 0, 0, color.w, 0, 0);
			data[idx++].setValue(center.x + points[i].x, center.y + points[i].y, 0, 0, 0, color.w, 0, 0);
			data[idx++].setValue(center.x + points[i + 1].x, center.y + points[i + 1].y, 0, 0, 0, color.w, 0, 0);
		}
	}
#endif
#endif


	// update sons position
	idx = 0;
	mItemList.clear();
	for (auto i = mSons.rbegin(); i != mSons.rend(); ++i) {

		if (idx >= mRealSlotCount)
		{
			(*i)->setValue("IsHidden",true);
			continue;
		}

		mItemList.push_back(*i);
		(*i)->setValue("IsHidden",false);
		(*i)->setValue("Position", mMidPoint + mSlot[idx++]);
	}
}

bool UIRoundHUD::ManageDirectTouchEvent(DirectTouchEvent& direct_touch)
{
	bool ret = false;
	int slot = mSelectedSlot;
	float dist = NormSquare(mMidPoint - direct_touch.position.xy);

	bool can_interact = mIsEnabled && mIsTouchable && !mIsHidden && !IsHiddenFlag() && IsInClip(direct_touch.position.xy);
	float allowedRadius = (mRadius + mRadiusOffset) * (mRadius + mRadiusOffset);
	// touch is possible in the radius or when isDown
	if (direct_touch.state == StatePossible) // check for hover
	{
		return (can_interact && dist<allowedRadius) || mIsDown;
	}

	if (can_interact && dist < allowedRadius)
		*direct_touch.swallow_mask |= 0xFFFFFFFF;

	
	if (direct_touch.state == StateBegan)
	{
		if (direct_touch.touch_state == DirectTouchEvent::TouchUp)
		{
			// emit signal if a slot is selected
			if (mSelectedSlot != -1 && mSelectedSlot < mItemList.size() && can_interact && mIsDown)
			{
				auto cm = mItemList.at(mSelectedSlot);
				if (cm->getValue<bool>("IsEnabled"))
					EmitSignal(Signals::TouchUp, this, cm);
			}
				

			mIsDown = false;
			return false;
		}
		else
		{
			// should not happend
		}
		//	printf("direct touch ended");
	}
	else
	{
		if (dist < mVoidZone)
		{
			slot = -1;
		}
		else
		{
			float nearest = Float_Max;
			for (int i = 0; i < mRealSlotCount; i++)
			{
				v2f center = mMidPoint + mSlot[i];
				float tmpDist = NormSquare(center - direct_touch.position.xy);

				/*if (tmpDist > myRadius*myRadius)
					continue;*/

				if (tmpDist < nearest)
				{
					nearest = tmpDist;
					slot = i;
				}
			}
		}
	}

	if (direct_touch.touch_state == DirectTouchEvent::TouchDown)
		mIsDown = true;
	else if (direct_touch.touch_state == DirectTouchEvent::TouchUp)
		mIsDown = false;


//#if DEBUG_DRAW==2
	if (mSelectedSlot != slot)
	{
		if (slot != -1 && !mItemList[slot]->getValue<bool>("IsEnabled"))
			slot = -1;

		EmitSignal(Signals::SlotChanged, this, slot, (slot>-1) ? mItemList.at(slot) : nullptr);
		
		if (slot != -1)
			mItemList[slot]->SimpleCall("Highlight", false);
		if (mSelectedSlot != -1)
			mItemList[mSelectedSlot]->SimpleCall("Highlight", true);

		if ((CoreModifiable*)mLabel)
		{
			kstl::string str="";
			if ((slot != -1 && slot < mItemList.size()))
			{
				if (!mItemList[slot]->getValue("Label", str))
					str = mItemList[slot]->getName();
			}

			((CoreModifiable*)mLabel)->setValue("Text", str.c_str());
		}

		mSelectedSlot = slot;
		mNeedUpdateSlots = true;
	}
//#endif
	return ret;
}