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

	myColor.changeNotificationLevel(Owner);
	myOpacity.changeNotificationLevel(Owner);

	myAngleStart.changeNotificationLevel(Owner);
	myAngleWide.changeNotificationLevel(Owner);
	mySlotCount.changeNotificationLevel(Owner);
	myIsClockwise.changeNotificationLevel(Owner);
	myRadius.changeNotificationLevel(Owner);
	myRadiusOffset.changeNotificationLevel(Owner);

	voidZone = (myRadius + myRadiusOffset) *0.2f;
	voidZone *= voidZone;

	ModuleInput* theInputModule = (ModuleInput*)KigsCore::GetModule("ModuleInput");
	theInputModule->getTouchManager()->registerEvent(this, "ManageDirectTouchEvent", DirectTouch, EmptyFlag);
}


void UIRoundHUD::ProtectedDraw(TravState* state)
{
	v2f p = myRealSize*0.5f;
	TransformPoints(&p, 1);

	if (p.x != myMidPoint.x || p.y != myMidPoint.y)
	{
		myNeedUpdateSlots = true;
		myMidPoint = p;
	}

	if (myNeedUpdateSlots)
	{
		UpdateSlots();
		myNeedUpdateSlots = false;
	}

	ParentClassType::ProtectedDraw(state);
}

bool UIRoundHUD::addItem(CoreModifiable* item, ItemPosition pos)
{
	myNeedUpdateSlots = true;
	return ParentClassType::addItem(item, pos);
}

bool UIRoundHUD::removeItem(CoreModifiable* item)
{
	myNeedUpdateSlots = true;
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
		labelid == myColor.getLabelID() ||
		labelid == myOpacity.getLabelID() ||
		labelid == myAngle.getLabelID() ||
		labelid == myAngleStart.getLabelID() ||
		labelid == myAngleWide.getLabelID() ||
		labelid == mySlotCount.getLabelID() ||
		labelid == myIsClockwise.getLabelID() ||
		labelid == myRadiusOffset.getLabelID() ||
		labelid == myRadius.getLabelID())
		myNeedUpdateSlots = true;
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
	myRealSlotCount = (mySlotCount >= 0) ? mySlotCount : GetSons().size();
	
	// create slot position array
	mySlot.reserve(myRealSlotCount + 2);
	generateCircle(mySlot, myRadius, (myAngleWide == 360.0f) ? myRealSlotCount : myRealSlotCount - 1, myAngleWide, myAngleStart);

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
	generateCircle(points, myRadius+ myRadiusOffset, CircleDiv,myAngleWide,myAngleStart);

	v4f color(myColor[0], myColor[1], myColor[2], GetOpacity());
	color *= 255;

	auto data = reinterpret_cast<VInfo2D::Data*>(mVI.Buffer());
	for (int i = 0; i < CircleDiv; ++i)
	{
		data[idx++].setValue(myMidPoint.x, myMidPoint.y, color.x, color.y, color.z, color.w, 0, 0);
		data[idx++].setValue(myMidPoint.x + points[i].x, myMidPoint.y + points[i].y, color.x, color.y, color.z, color.w, 0, 0);
		data[idx++].setValue(myMidPoint.x + points[i + 1].x, myMidPoint.y + points[i + 1].y, color.x, color.y, color.z, color.w, 0, 0);
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
	myItemList.clear();
	for (auto i = mySons.rbegin(); i != mySons.rend(); ++i) {

		if (idx >= myRealSlotCount)
		{
			(*i)->setValue("IsHidden",true);
			continue;
		}

		myItemList.push_back(*i);
		(*i)->setValue("IsHidden",false);
		(*i)->setValue("Position", myMidPoint + mySlot[idx++]);
	}
}

bool UIRoundHUD::ManageDirectTouchEvent(DirectTouchEvent& direct_touch)
{
	bool ret = false;
	int slot = mySelectedSlot;
	float dist = NormSquare(myMidPoint - direct_touch.position.xy);

	bool can_interact = myIsEnabled && mybIsTouchable && !myIsHidden && !IsHiddenFlag() && IsInClip(direct_touch.position.xy);
	float allowedRadius = (myRadius + myRadiusOffset) * (myRadius + myRadiusOffset);
	// touch is possible in the radius or when isDown
	if (direct_touch.state == StatePossible) // check for hover
	{
		return (can_interact && dist<allowedRadius) || isDown;
	}

	if (can_interact && dist < allowedRadius)
		*direct_touch.swallow_mask |= 0xFFFFFFFF;

	
	if (direct_touch.state == StateBegan)
	{
		if (direct_touch.touch_state == DirectTouchEvent::TouchUp)
		{
			// emit signal if a slot is selected
			if (mySelectedSlot != -1 && mySelectedSlot < myItemList.size() && can_interact && isDown)
			{
				auto cm = myItemList.at(mySelectedSlot);
				if (cm->getValue<bool>("IsEnabled"))
					EmitSignal(Signals::TouchUp, this, cm);
			}
				

			isDown = false;
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
		if (dist < voidZone)
		{
			slot = -1;
		}
		else
		{
			float nearest = Float_Max;
			for (int i = 0; i < myRealSlotCount; i++)
			{
				v2f center = myMidPoint + mySlot[i];
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
		isDown = true;
	else if (direct_touch.touch_state == DirectTouchEvent::TouchUp)
		isDown = false;


//#if DEBUG_DRAW==2
	if (mySelectedSlot != slot)
	{
		if (slot != -1 && !myItemList[slot]->getValue<bool>("IsEnabled"))
			slot = -1;

		EmitSignal(Signals::SlotChanged, this, slot, (slot>-1) ? myItemList.at(slot) : nullptr);
		
		if (slot != -1)
			myItemList[slot]->SimpleCall("Highlight", false);
		if (mySelectedSlot != -1)
			myItemList[mySelectedSlot]->SimpleCall("Highlight", true);

		if ((CoreModifiable*)myLabel)
		{
			kstl::string str="";
			if ((slot != -1 && slot < myItemList.size()))
			{
				if (!myItemList[slot]->getValue("Label", str))
					str = myItemList[slot]->getName();
			}

			((CoreModifiable*)myLabel)->setValue("Text", str.c_str());
		}

		mySelectedSlot = slot;
		myNeedUpdateSlots = true;
	}
//#endif
	return ret;
}